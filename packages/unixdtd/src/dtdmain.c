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
 */

/*
 * Implementation of Direct-To-Disk handling by a separate server process.
 *
 * On Irix, the Posix thread implementation has prooved real-time problems due to 
 * IRIX Posix thread implementation that interacts strangely with isolated processors 
 * management.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <audiofile.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fts/fts.h>

#include "dtddefs.h"
#include "dtdfifo.h"

/*
 * Define this if you want a lot of debug printout, in particular timing
 */
#undef DTD_SERVER_ENABLE_DEBUG

#ifdef DTD_SERVER_ENABLE_DEBUG
#include <stdarg.h>
#include "libtime.h"
#define DTD_DEBUG(x) (x)

static int _dbg( const char *format, ...)
{
  va_list ap;
  char buf[1024];
  double now, elapsed;

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

/*
 * Constants used by the server
 */
static int _block_frames;
static int _max_channels;
static int _fifo_blocks;
static int _preload_frames;
static int _loop_milliseconds;

static void dtd_init_params( int argc, char **argv)
{
  sscanf( argv[1], "%d", &_block_frames);
  sscanf( argv[2], "%d", &_max_channels);
  sscanf( argv[3], "%d", &_fifo_blocks);
  sscanf( argv[4], "%d", &_preload_frames);
  sscanf( argv[5], "%d", &_loop_milliseconds);
}

typedef struct {
  dtdfifo_t *fifo;
  AFfilehandle file;
  enum { handle_state_read, handle_state_write, handle_state_closed} state;
  int n_channels;
} dtdhandle_t;

static dtdhandle_t handle_table[DTD_MAX_FIFOS];

/* One block for all */
static short *block;

#define N 2048

static int dtd_read_block( AFfilehandle file, dtdfifo_t *fifo, short *buffer, int n_frames, int n_channels)
{
  int frames_to_read, n_read, index, buffer_size, size, n;
  volatile float *dst;

  n = dtdfifo_get_write_level( fifo) / (n_channels * sizeof( float));
  frames_to_read = ( n < n_frames) ? n : n_frames;

  if (frames_to_read == 0)
    return 0;

  n_read = afReadFrames( file, AF_DEFAULT_TRACK, buffer, frames_to_read);

  if (n_read < 0)
    return -1;

  index = dtdfifo_get_write_index( fifo)/sizeof( float);
  buffer_size = dtdfifo_get_buffer_size( fifo)/sizeof( float);
  size = n_read * n_channels;

  if ( index + size < buffer_size )
    {
      dst = (volatile float *)dtdfifo_get_write_pointer( fifo);

      for ( n = 0; n < size; n++)
	{
	  *dst++ = ((float)*buffer++) / 32767.0f;
	}
    }
  else
    {
      dst = (volatile float *)dtdfifo_get_write_pointer( fifo);

      for ( n = 0; n < buffer_size - index; n++)
	{
	  *dst++ = ((float)*buffer++) / 32767.0f;
	}

      dst = (volatile float *)dtdfifo_get_buffer( fifo);

      for ( ; n < size; n++)
	{
	  *dst++ = ((float)*buffer++) / 32767.0f;
	}
    }

  dtdfifo_incr_write_index( fifo, size * sizeof( float));

  if (n_read < frames_to_read)
    {
      dtdfifo_set_used( fifo, DTD_SIDE, 0);
    }

  return n_read;
}

static int dtd_write_block( AFfilehandle file, dtdfifo_t *fifo, short *buffer, int n_frames, int n_channels)
{
  int frames_to_write, n_write, index, buffer_size, size, n;
  volatile float *src;
  short *p;

  n = dtdfifo_get_read_level( fifo) / (n_channels * sizeof( float));
  frames_to_write = ( n < n_frames) ? n : n_frames;

  if (frames_to_write == 0)
    return 0;

  src = (volatile float *)dtdfifo_get_read_pointer( fifo);

  index = dtdfifo_get_read_index( fifo)/sizeof( float);
  buffer_size = dtdfifo_get_buffer_size( fifo)/sizeof( float);
  size = frames_to_write * n_channels;

  p = buffer;
  if ( index + size < buffer_size )
    {
      for ( n = 0; n < size; n++)
	{
	  *p++ = (short) (*src++ * 32767.0f);
	}
    }
  else
    {
      for ( n = 0; n < buffer_size - index; n++)
	{
	  *p++ = (short) (*src++ * 32767.0f);
	}

      src = (volatile float *)dtdfifo_get_buffer( fifo);

      for ( ; n < size; n++)
	{
	  *p++ = (short) (*src++ * 32767.0f);
	}
    }

  n_write = afWriteFrames( file, AF_DEFAULT_TRACK, buffer, frames_to_write);

  if (n_write < frames_to_write)
    return -1;

  dtdfifo_incr_read_index( fifo, size * sizeof( float));

  return n_write;
}

static AFfilehandle dtd_open_file_read( const char *filename, const char *path, int n_channels)
{
  char full_path[N+N];
  AFfilehandle file;
  int file_channels, sampfmt, sampwidth;

  if ( !fts_file_search_in_path( filename, path, full_path) )
    {
      printf( "[dtdserver] cannot open sound file %s\n", filename);
      fflush( stdout);

      return AF_NULL_FILEHANDLE;
    }

  if ( (file = afOpenFile( full_path, "r", NULL)) == AF_NULL_FILEHANDLE)
    {
      printf( "[dtdserver] cannot open sound file %s\n", filename);
      fflush( stdout);

      return AF_NULL_FILEHANDLE;
    }

  DTD_DEBUG( _dbg( "opened `%s'", full_path) );

  file_channels = afGetChannels( file, AF_DEFAULT_TRACK);
  if ( file_channels != n_channels)
    {
      printf( "[dtdserver] invalid number of channels (%d)\n", file_channels);
      fflush( stdout);

      return AF_NULL_FILEHANDLE;
    }

  afGetSampleFormat( file, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);
  if ((sampfmt != AF_SAMPFMT_TWOSCOMP) || (sampwidth != 16))
    {
      printf( "[dtdserver] invalid format\n");
      fflush( stdout);

      return AF_NULL_FILEHANDLE;
    }

  return file;
}

static AFfilehandle dtd_open_file_write( const char *filename, const char *path, int format, double sr, int n_channels)
{
  char full_path[N+N];
  AFfilehandle file;
  AFfilesetup setup;
  int file_channels, sampfmt, sampwidth;

  if (filename[0] == '/')
    strcpy( full_path, filename);
  else
    {
      strcpy( full_path, path);
      strcat( full_path, "/");
      strcat( full_path, filename);
    }

  setup = afNewFileSetup();

  afInitFileFormat( setup, format);
  afInitRate( setup, AF_DEFAULT_TRACK, sr);
  afInitChannels( setup, AF_DEFAULT_TRACK, n_channels);

  if ( (file = afOpenFile( full_path, "w", setup)) == AF_NULL_FILEHANDLE)
    {
      printf( "[dtdserver] cannot open sound file %s\n", filename);
      fflush( stdout);

      return AF_NULL_FILEHANDLE;
    }

  DTD_DEBUG( _dbg( "opened `%s'", full_path) );

  afFreeFileSetup(setup);

  return file;
}

static void dtd_open_read( const char *line)
{
  AFfilehandle file;
  int id, n_channels, i;
  char filename[N], path[N];
  dtdhandle_t *handle;

  sscanf( line, "%*s%d%s%s%d", &id, filename, path, &n_channels);

  handle = &handle_table[id];
  
  dtdfifo_set_used( handle->fifo, DTD_SIDE, 1);

  if ((handle->file = dtd_open_file_read( filename, path, n_channels)) == AF_NULL_FILEHANDLE)
    return;

  handle->state = handle_state_read;
  handle->n_channels = n_channels;

  for ( i = 0; i < _block_frames / _preload_frames; i++)
    {
      dtd_read_block( handle->file, handle->fifo, block, _preload_frames, n_channels);
    }

  DTD_DEBUG( _dbg( "preloaded `%s'", filename) );
}

static struct extension2format {
  char *extension;
  int format;
} extension2format_table[] = {
  { ".wav", AF_FILE_WAVE},
  { ".aiff", AF_FILE_AIFF},
  { ".snd", AF_FILE_NEXTSND},
  { ".voc", AF_FILE_VOC},
  { ".sf", AF_FILE_IRCAM}
};

static void dtd_open_write( const char *line)
{
  AFfilehandle file;
  int id, n_channels, af_format;
  unsigned int i;
  char filename[N], path[N];
  dtdhandle_t *handle;
  double sr;
  char *extension;

  sscanf( line, "%*s%d%s%s%lf%d", &id, filename, path, &sr, &n_channels);

  handle = &handle_table[id];
  
  dtdfifo_set_used( handle->fifo, DTD_SIDE, 1);

  extension = strrchr( filename, '.');
  af_format = AF_FILE_RAWDATA;

  for ( i = 0; i < sizeof( extension2format_table)/sizeof( struct extension2format); i++)
    if (!strcmp( extension, extension2format_table[i].extension))
      af_format = extension2format_table[i].format;
  
  if ((handle->file = dtd_open_file_write( filename, path, af_format, sr, n_channels)) == AF_NULL_FILEHANDLE)
    return;

  handle->state = handle_state_write;
  handle->n_channels = n_channels;
}

static void dtd_close( const char *line)
{
  int id;
  dtdhandle_t *handle;

  sscanf( line, "%*s%d", &id);

  handle = &handle_table[id];

  if ( !dtdfifo_is_used( handle->fifo, DTD_SIDE) )
    return;

  if (handle->state == handle_state_write)
    {
      /* empty fifo */
      int r;

      do
	{
	  r = dtd_write_block( handle->file, handle->fifo, block, _block_frames, handle->n_channels);
	}
      while ( r > 0);
    }

  dtdfifo_set_used( handle->fifo, DTD_SIDE, 0);

  if ( handle->file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( handle->file);
      handle->file = AF_NULL_FILEHANDLE;
      handle->state = handle_state_closed;
    }
}

static void dtd_mmap( const char *line)
{
  int id, fifo_size;
  char filename[N];
  dtdfifo_t *fifo;

  sscanf( line, "%*s%d%s", &id, filename);

  fifo_size = _block_frames * _max_channels * _fifo_blocks * sizeof( float);
  fifo = dtdfifo_mmap( filename, fifo_size);

  if (fifo)
    {
      handle_table[ id ].fifo = fifo;
      handle_table[ id ].file = AF_NULL_FILEHANDLE;
    }
}

/*
 * Commands are:
 * mmap <id> <filename>
 * openread <id> <sound_file_name> <search_path> <n_channels>
 * openwrite <id> <sound_file_name> <path> <sampling_rate> <n_channels>
 * close <id>
 */
static void dtd_process_command( const char *line)
{
  char command[N];

  DTD_DEBUG( _dbg( "got command `%s'",line) );

  sscanf( line, "%s", command);

  if ( !strcmp( "openread", command))
    dtd_open_read( line);
  else if ( !strcmp( "openwrite", command))
    dtd_open_write( line);
  else if ( !strcmp( "close", command))
    dtd_close( line);
  else if ( !strcmp( "mmap", command))
    dtd_mmap( line);
}

static void dtd_process_fifos( void)
{
  int id;

  for ( id = 0; id < DTD_MAX_FIFOS; id++)
    {
      int n_channels, block_size;
      dtdhandle_t *h;

      h = &handle_table[id];

      if ( !h || h->file==AF_NULL_FILEHANDLE || !dtdfifo_is_used( h->fifo, 0) || !dtdfifo_is_used( h->fifo, 1))
	continue;

      n_channels = h->n_channels;
      block_size = _block_frames * n_channels * sizeof( float);

      DTD_DEBUG( _dbg( "polling fifo %d channels=%d level=%d block_size=%d", id, n_channels, dtdfifo_get_write_level( h->fifo), block_size));

      if ( h->state == handle_state_read && dtdfifo_get_write_level( h->fifo) >= block_size )
	{
	  int ret = dtd_read_block( h->file, h->fifo, block, _block_frames, n_channels);
	  DTD_DEBUG( _dbg("filled %d samples in fifo %d", ret, id));
	}
      else if ( h->state == handle_state_write && dtdfifo_get_read_level( h->fifo) >= block_size )
	{
	  int ret = dtd_write_block( h->file, h->fifo, block, _block_frames, n_channels);
	  DTD_DEBUG( _dbg("writen %d samples from fifo %d", ret, id));
	}
      else
	{
	  DTD_DEBUG( _dbg( "fifo %d full/empty", id));
	}
    }
}


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
static void dtd_main_loop( int fd)
{
  DTD_DEBUG( _dbg( "DTD server running") );

  while (1)
    {
      fd_set rfds;
      struct timeval tv;
      int retval;
      char line[N];
      
      tv.tv_sec = 0;
      tv.tv_usec = _loop_milliseconds * 1000;

      FD_ZERO( &rfds);

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

  DTD_DEBUG( _dbg( "DTD server exiting") );
}

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

  /* Get rid of root privilege if we have them */
  if (setreuid(getuid(), getuid()) == -1)
    {
    }
}

int main( int argc, char **argv)
{
  int socket, port;

  dtd_init_params( argc, argv);

  if ((socket = dtd_create_socket( &port)) < 0)
    {
      fprintf( stderr, "[dtdserver] cannot open socket (%s)\n", strerror( errno));
      return 1;
    }

  /* Write the port number for FTS */
  write( 1, &port, 4);

  dtd_no_real_time();

  block = (short *)malloc( _block_frames * _max_channels * sizeof( short));

  dtd_main_loop( socket);

  return 0;
}
