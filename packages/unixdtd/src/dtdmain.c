/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 */

/*
 * This file's authors: Francois Dechelle.
 */

/*
 * Implementation of Direct-To-Disk handling by a separate server process.
 *
 * The Posix thread implementation has prooved real-time problems due to IRIX Posix thread
 * implementation that interacts strangely with isolated processors management.
 */

/* #define one of these to get a server that reads commands via udp or on a pipe */
#define USE_UDP
#undef USE_PIPE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <sys/stat.h>
#include <signal.h>
#include <audiofile.h>
#ifdef USE_UDP
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "dtddefs.h"
#include "dtdfifo.h"

/*
 * Define this if you want a lot of debug printout, in particular timing
 * The debug is then enabled by setting the environment variable DTDSERVER_DEBUG
 */
#define DTD_SERVER_ENABLE_DEBUG

#ifdef DTD_SERVER_ENABLE_DEBUG

#include <stdarg.h>
#include "libtime.h"

static int __debug_value = 0;

#define DTD_DEBUG(x) (x)

static int __debug( const char *format, ...)
{
  va_list ap;
  char buf[1024];
  double now, elapsed;

  if (!__debug_value)
    return -1;

  va_start( ap, format);
  vsprintf( buf, format, ap);
  va_end( ap);

  get_current_milliseconds( &now, &elapsed);

  fprintf( stderr, "[dtdserver DEBUG %17.3f, %8.3f] %s\n", now, elapsed, buf);

  return 0;
}

#else
#define DTD_DEBUG(x)
#endif

#define ZZZ

static struct dtd_handle {
  dtdfifo_t *fifo;
  AFfilehandle file;
  int n_channels;
#ifdef ZZZ
  int samples_count;
#endif
} dtd_handle_table[N_FIFOS];

/* One read block for all */
static short read_block[BLOCK_FRAMES*BLOCK_MAX_CHANNELS];

#define N 256

static void dtd_init_fifos( void)
{
  int n;

  /* For now, the number of fifos, the block size and the number of blocks are fixed */
  for ( n = 0; n < N_FIFOS; n++)
    {
      dtd_handle_table[n].fifo = dtdfifo_new( n, BLOCK_FRAMES * BLOCK_MAX_CHANNELS * BLOCKS_PER_FIFO * sizeof( float));
      dtd_handle_table[n].file = AF_NULL_FILEHANDLE;
    }
}

static int dtd_read_block( AFfilehandle file, dtdfifo_t *fifo, short *buffer, int n_frames, int n_channels)
{
  int n_read, index, buffer_size, size, n;
  volatile float *dst;

  /* This should never happen because it is tested before the call to dtd_read_block() */
  if ( (unsigned int)dtdfifo_get_write_level( fifo) < n_frames * n_channels * sizeof( float))
    return -1;

  n_read = afReadFrames( file, AF_DEFAULT_TRACK, buffer, n_frames);

  if (n_read < 0)
    return -1;

  dst = (volatile float *)dtdfifo_get_write_pointer( fifo);

  index = dtdfifo_get_write_index( fifo)/sizeof( float);
  buffer_size = dtdfifo_get_buffer_size( fifo)/sizeof( float);
  size = n_read * n_channels;

  if ( index + size < buffer_size )
    {
      for ( n = 0; n < size; n++)
	{
	  *dst++ = *buffer++ / 32767.0f;
	}
    }
  else
    {
      for ( n = 0; n < buffer_size - index; n++)
	{
	  *dst++ = *buffer++ / 32767.0f;
	}

      dst = (volatile float *)dtdfifo_get_buffer( fifo);

      for ( ; n < size; n++)
	{
	  *dst++ = *buffer++ / 32767.0f;
	}
    }

  if (n_read < n_frames)
    {
      dtdfifo_set_eof( fifo, 1);
    }

  dtdfifo_incr_write_index( fifo, size * sizeof( float));

  return n_read;
}

static const char *splitpath( const char *path, char *result, char sep)
{
  if ( *path == '\0')
    return 0;

  while ( *path != sep && *path != '\0')
    {
      *result++ = *path++;
    }

  *result = '\0';

  if ( *path)
    return path+1;

  return path;
}

static int file_exists( const char *filename)
{
  struct stat statbuf;

  return ( stat( filename, &statbuf) == 0) && (statbuf.st_mode & S_IFREG);
}

static int search_file_in_path( const char *filename, const char *path, char *full_path)
{
  char pathelem[N];

  if (*filename == '/')
    {
      strcpy( full_path, filename);

      return file_exists( full_path);
    }

  while ( (path = splitpath( path, pathelem, ':')) )
    {
      strcpy( full_path, pathelem);
      strcat( full_path, "/");
      strcat( full_path, filename);

      if (file_exists( full_path))
	  return 1;
    }

  return 0;
}

static AFfilehandle dtd_open_and_check_file( const char *filename, const char *path, int n_channels)
{
  char full_path[N+N];
  AFfilehandle file;
  int file_channels, sampfmt, sampwidth;

  if ( !search_file_in_path( filename, path, full_path) )
    {
      fprintf( stderr, "[dtdserver] cannot open sound file %s\n", filename);
      return AF_NULL_FILEHANDLE;
    }

  if ( (file = afOpenFile( full_path, "r", 0)) == AF_NULL_FILEHANDLE)
    {
      fprintf( stderr, "[dtdserver] cannot open sound file %s\n", filename);
      return AF_NULL_FILEHANDLE;
    }

  file_channels = afGetChannels( file, AF_DEFAULT_TRACK);

  if ( file_channels != n_channels)
    {
      fprintf( stderr, "[dtdserver] invalid number of channels (%d)\n", file_channels);
      return AF_NULL_FILEHANDLE;
    }

  afGetSampleFormat( file, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

  if ((sampfmt != AF_SAMPFMT_TWOSCOMP) || (sampwidth != 16))
    {
      fprintf( stderr, "[dtdserver] invalid format\n");
      return AF_NULL_FILEHANDLE;
    }

  return file;
}

static void dtd_open( const char *line)
{
  AFfilehandle file;
  dtdfifo_t *fifo;
  int fifo_number, n_channels, i;
  char command[N], filename[N], path[N];
  
  sscanf( line, "%s%d%s%s%d", command, &fifo_number, filename, path, &n_channels);

  fifo = dtd_handle_table[fifo_number].fifo;
  file = dtd_handle_table[fifo_number].file;
  dtd_handle_table[fifo_number].n_channels = n_channels;

#ifdef ZZZ
  dtd_handle_table[fifo_number].samples_count = 0;
#endif

  if ( dtdfifo_is_write_used( fifo))
    {
      fprintf( stderr, "[dtdserver] warning: opening an already active fifo\n");
    }

  dtdfifo_set_write_used( fifo, 1);

  /* This should not happen */
  if ( file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( file);
      dtd_handle_table[fifo_number].file = AF_NULL_FILEHANDLE;
    }

  if ((file = dtd_open_and_check_file( filename, path, n_channels)) == AF_NULL_FILEHANDLE)
    return;

  dtd_handle_table[fifo_number].file = file;

  for ( i = 0; i < BLOCK_FRAMES/PRELOAD_BLOCK_FRAMES; i++)
    {
      int ret;

      ret = dtd_read_block( file, fifo, read_block, PRELOAD_BLOCK_FRAMES, dtd_handle_table[fifo_number].n_channels);

#ifdef ZZZ
      dtd_handle_table[fifo_number].samples_count += ret;
#endif
    }

  DTD_DEBUG( __debug( "opened `%s'", filename) );
}

static void dtd_close( const char *line)
{
  int fifo_number;
  char command[N];

  sscanf( line, "%s%d", command, &fifo_number);

  /*
   * A "close" is send by FTS when it releases the fifo.
   * It will not reallocate the fifo till it is marked
   * as write_used, so we can safely reinitialize it here.
   */
  dtdfifo_set_eof( dtd_handle_table[fifo_number].fifo, 0);
  dtdfifo_set_read_index( dtd_handle_table[fifo_number].fifo, 0);
  dtdfifo_set_write_index( dtd_handle_table[fifo_number].fifo, 0);

  dtdfifo_set_write_used( dtd_handle_table[fifo_number].fifo, 0);

  if ( dtd_handle_table[fifo_number].file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( dtd_handle_table[fifo_number].file);
      dtd_handle_table[fifo_number].file = AF_NULL_FILEHANDLE;
    }
}

/*
 * Commands are:
 * open <fifo_number> <sound_file_name> <search_path> <number_of_channels>
 * close <fifo_number>
 */
static void dtd_process_command( const char *line)
{
  char command[N];

  DTD_DEBUG( __debug( "got command `%s'",line) );

  sscanf( line, "%s", command);

  if ( !strcmp( "open", command))
    dtd_open( line);
  else if ( !strcmp( "close", command))
    dtd_close( line);
}

static void dtd_process_fifos( void)
{
  int i;

  for ( i = 0; i < N_FIFOS; i++)
    {
      AFfilehandle file;
      dtdfifo_t *fifo;

      file = dtd_handle_table[i].file;
      fifo = dtd_handle_table[i].fifo;

      if ( file != AF_NULL_FILEHANDLE 
	   && dtdfifo_is_read_used( fifo) 
	   && dtdfifo_is_write_used( fifo)
	   && !dtdfifo_is_eof( fifo))
	{
	  int n_channels, block_size;

	  n_channels = dtd_handle_table[i].n_channels;

	  block_size = BLOCK_FRAMES * n_channels * sizeof( float);

	  DTD_DEBUG( __debug( "polling fifo %d", i));

	  if ( dtdfifo_get_write_level( fifo) >= block_size )
	    {
	      int ret;

	      ret = dtd_read_block( file, fifo, read_block, BLOCK_FRAMES, n_channels);

	      DTD_DEBUG( __debug("filled %d samples in fifo %d", ret, i));

	      if (dtdfifo_is_eof( fifo))
		DTD_DEBUG( __debug("EOF on fifo %d", i));

#ifdef ZZZ
	      dtd_handle_table[i].samples_count += ret;
	      if ( ret<BLOCK_FRAMES)
		DTD_DEBUG( __debug("fifo %d: total read %d frames", i, dtd_handle_table[i].samples_count));
#endif
	    }
	  else
	    {
	      DTD_DEBUG( __debug( "fifo %d full", i) );
	    }
	}
    }
}


#ifdef USE_UDP
static int dtd_get_line( int fd, char *line, int n)
{
  int size;

  size = recvfrom( fd, line, n, 0, NULL, NULL);

  if ( size < 0)
    {
      fprintf( stderr, "[dtdserver] error in recvfrom (%s)\n", strerror( errno));
      return -1;
    }

  return 0;
}
#endif

#ifdef USE_PIPE
static int dtd_get_line( int fd, char *line, int n)
{
  if ( fgets( line, n, stdin) == NULL)
    return -1;

  line[ strlen(line) - 1] = '\0';

  return 0;
}
#endif

static void dtd_main_loop( int fd)
{
  DTD_DEBUG( __debug( "DTD server running") );

  while (1)
    {
      fd_set rfds;
      struct timeval tv;
      int retval;
      char line[N];
      
      tv.tv_sec = DTD_SERVER_SELECT_TIMEOUT_SEC;
      tv.tv_usec = DTD_SERVER_SELECT_TIMEOUT_USEC;

      FD_ZERO( &rfds);

      FD_SET( 0, &rfds);
      FD_SET( fd, &rfds);
      
      retval = select( fd+1, &rfds, NULL, NULL, &tv);

      if (retval < 0)
	{
	  if (errno != EINTR)
	    {
	      fprintf( stderr, "[dtdserver] error in select (%s)\n", strerror( errno));
	      break;
	    }
	}
      else if (retval)
	{
	  if (FD_ISSET( fd, &rfds))
	    {
	      if (dtd_get_line( fd, line, N) < 0)
		break;

	      dtd_process_command( line);
	    }
	}
      
      dtd_process_fifos();
    }

  DTD_DEBUG( __debug( "DTD server exiting") );
}

#ifdef USE_UDP
static int dtd_create_socket( int *pport)
{
  struct sockaddr_in my_addr;
  int sock;
  int len;

  sock = socket( PF_INET, SOCK_DGRAM, 0);

  if (sock == -1)
    {
      fprintf( stderr, "[dtdserver] cannot open socket\n");
      return -1;
    }

  memset( &my_addr, 0, sizeof(struct sockaddr_in));
  my_addr.sin_family = PF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = 0;

  if ( bind( sock, &my_addr, sizeof(struct sockaddr_in)) == -1)
    {
      fprintf( stderr, "[dtdserver] cannot bind socket (%d, %s)\n", errno, strerror( errno));
      return -1;
    }

  len = sizeof(struct sockaddr_in);
  if ( getsockname( sock, &my_addr, &len) < 0)
    {
      fprintf( stderr, "[dtdserver] cannot get socket name\n");
      return -1;
    }
    

  *pport = ntohs( my_addr.sin_port);

  return sock;
}
#endif

static void dtd_no_real_time( void)
{
  int sched_policy;
  struct sched_param sp;

  sched_policy = sched_getscheduler(0);

  if (sched_policy != SCHED_OTHER)
    {
      sp.sched_priority = sched_get_priority_min(SCHED_OTHER);
      if ( sched_setscheduler( 0, SCHED_OTHER, &sp) < 0)
	{
	  fprintf( stderr, "[dtdserver] cannot give up real-time priority (%s)\n", strerror( errno));
	}
    }
}

static void signal_handler( int sig)
{
  int new_debug_value;

  new_debug_value = !__debug_value;

  if (new_debug_value)
    {
      __debug_value = new_debug_value;
      DTD_DEBUG( __debug( "debug enabled") );
    }
  else
    {
      DTD_DEBUG( __debug( "debug disabled") );
      __debug_value = new_debug_value;
    }
}

int main( int argc, char **argv)
{
#ifdef USE_UDP
  int socket, port;

  if ((socket = dtd_create_socket( &port)) < 0)
    {
      fprintf( stderr, "[dtdserver] cannot open socket (%s)\n", strerror( errno));
      return 1;
    }

  /* Prints the port number for FTS */
  printf( "%d\n", port);
  fflush( stdout);

#endif
#ifdef USE_PIPE
  int socket = 0; /* standard input */
#endif

#ifdef DTD_SERVER_ENABLE_DEBUG
  if (getenv( "DTDSERVER_DEBUG"))
    __debug_value = 1;
#endif

  dtd_no_real_time();

  dtd_init_fifos();

  signal( SIGUSR1, signal_handler);

  dtd_main_loop( socket);

  return 0;
}
