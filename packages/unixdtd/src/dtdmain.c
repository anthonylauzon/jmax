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
 * Authors: Francois Dechelle.
 *
 */

/*
 * Implementation of Direct-To-Disk handling by a separate server process.
 *
 * The Posix thread implementation has prooved real-time problems due to IRIX Posix thread
 * implementation that interacts strangely with isolated processors management.
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <sys/stat.h>
#include <audiofile.h>

#include "dtddefs.h"
#include "dtdfifo.h"

static struct dtd_handle {
  dtdfifo_t *fifo;
  AFfilehandle file;
  int n_channels;
} dtd_handle_table[N_FIFOS];

/* One read block for all (... one read block per thread) */
static short read_block[BLOCK_FRAMES*BLOCK_MAX_CHANNELS];

#define N 256

static void dtd_init( void)
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
  int n_read, index, buffer_size, size, n, n1, n2;
  volatile float *dst;

  if ( (unsigned int)dtdfifo_get_write_level( fifo) < n_frames * n_channels * sizeof( float))
    return 0;

#ifdef DEBUG
  dtdfifo_debug( fifo, "[dtdserver]");
#endif

  n_read = afReadFrames( file, AF_DEFAULT_TRACK, buffer, n_frames);

  if (n_read < 0)
    return -1;

  dst = (volatile float *)dtdfifo_get_write_pointer( fifo);

  index = dtdfifo_get_write_index( fifo);
  buffer_size = dtdfifo_get_buffer_size( fifo);
  size = n_read * n_channels * sizeof( float);

  if ( index + size < buffer_size )
    {
      for ( n = 0; n < (int)(size/sizeof(float)); n++)
	{
	  *dst++ = ((float)(*buffer++)) / 32767.0f;
	}
    }
  else
    {
      for ( n = 0; n < (int)(buffer_size/sizeof(float)) - index; n++)
	{
	  *dst++ = ((float)(*buffer++)) / 32767.0f;
	}

      dst = (volatile float *)dtdfifo_get_buffer( fifo);

      for ( ; n < (int)(size/sizeof(float)); n++)
	{
	  *dst++ = ((float)(*buffer++)) / 32767.0f;
	}
    }

  if (n_read < n_frames)
    dtdfifo_set_state( fifo, FIFO_EOF);

  dtdfifo_incr_write_index( fifo, size);

  return n_read;
}

static void dtd_new( const char *line)
{
  int fifo_number, n_channels;
  char command[N];

  sscanf( line, "%s%d%d", command, &fifo_number, &n_channels);

  dtd_handle_table[fifo_number].n_channels = n_channels;
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

static char *search_file_in_path( const char *filename, const char *path, char *full_path)
{
  char pathelem[N];

  if (*filename == '/')
    {
      strcpy( full_path, filename);
      return full_path;
    }

  while ( (path = splitpath( path, pathelem, ':')) )
    {
      strcpy( full_path, pathelem);
      strcat( full_path, "/");
      strcat( full_path, filename);

      if (file_exists( full_path))
	  return full_path;
    }

  return 0;
}

static void dtd_open( const char *line)
{
  AFfilehandle file;
  dtdfifo_t *fifo;
  int fifo_number, file_channels, sampfmt, sampwidth, i;
  char command[N], filename[N], path[N];
  char full_path[N+N];;
  
  sscanf( line, "%s%d%s%s", command, &fifo_number, filename, path);

  fifo = dtd_handle_table[fifo_number].fifo;

  file = dtd_handle_table[fifo_number].file;

  if ( file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( file);
      dtd_handle_table[fifo_number].file = AF_NULL_FILEHANDLE;
    }

  dtdfifo_set_state( fifo, FIFO_INACTIVE);

  if ( !search_file_in_path( filename, path, full_path) )
    {
      fprintf( stderr, "[dtdserver] cannot open sound file %s\n", filename);
      return;
    }

  file = afOpenFile( filename, "r", 0);

  if (file == AF_NULL_FILEHANDLE)
    {
      fprintf( stderr, "[dtdserver] cannot open sound file %s\n", filename);
      return;
    }

  file_channels = afGetChannels( file, AF_DEFAULT_TRACK);

  if ( file_channels != dtd_handle_table[fifo_number].n_channels)
    {
      fprintf( stderr, "[dtdserver] invalid number of channels (%d)\n", file_channels);
      return;
    }

  afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

  if ((sampfmt != AF_SAMPFMT_TWOSCOMP) || (sampwidth != 16))
    {
      fprintf( stderr, "[dtdserver] invalid format\n");
      return;
    }

  dtd_handle_table[fifo_number].file = file;

  dtdfifo_init( fifo);

  for ( i = 0; i < BLOCK_FRAMES/PRELOAD_BLOCK_FRAMES - 1; i++)
    {
      dtd_read_block( file, fifo, read_block, PRELOAD_BLOCK_FRAMES, dtd_handle_table[fifo_number].n_channels);

      if ( i == 0)
	dtdfifo_set_state( fifo, FIFO_ACTIVE);
    }
}

static void dtd_close( const char *line)
{
  int fifo_number;
  char command[N];

  sscanf( line, "%s%d", command, &fifo_number);

  dtdfifo_set_state( dtd_handle_table[fifo_number].fifo, FIFO_INACTIVE);

  if ( dtd_handle_table[fifo_number].file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( dtd_handle_table[fifo_number].file);
      dtd_handle_table[fifo_number].file = AF_NULL_FILEHANDLE;
    }
}

/*
 * Commands are:
 * new <fifo_number> <number_of_channels>
 * open <fifo_number> <sound_file_name> <search_path>
 * close <fifo_number>
 */
static void dtd_process_command( const char *line)
{
  char command[N];

  sscanf( line, "%s", command);

  if ( !strcmp( "new", command))
    dtd_new( line);
  else if ( !strcmp( "open", command))
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

      if ( file != AF_NULL_FILEHANDLE  && dtdfifo_get_state( fifo) == FIFO_ACTIVE)
	{
	  dtd_read_block( file, fifo, read_block, BLOCK_FRAMES, dtd_handle_table[i].n_channels);
	}
    }
}

static void dtd_main_loop( void)
{
#ifdef DEBUG
  fprintf( stderr, "[dtdserver] DTD server running\n");
#endif

  while (1)
    {
      fd_set rfds;
      struct timeval tv;
      int retval;
      char line[N];
      
      FD_ZERO( &rfds);
      FD_SET( 0, &rfds);

      tv.tv_sec = DTD_SERVER_SELECT_TIMEOUT_SEC;
      tv.tv_usec = DTD_SERVER_SELECT_TIMEOUT_USEC;

      retval = select( 1, &rfds, NULL, NULL, &tv);

      if (retval)
	{
	  if ( fgets( line, N, stdin) == NULL)
	    break;

	  line[ strlen(line) - 1] = '\0';

	  dtd_process_command( line);
	}
      else if (retval < 0)
	{
	  fprintf( stderr, "[dtdserver] error in select (%s)\n", strerror( errno));
	  break;
	}
      else
	{
	  dtd_process_fifos();
	}
    }

#ifdef DEBUG
  fprintf( stderr, "[dtdserver] DTD server exiting\n");
#endif
}

int main( int argc, char **argv)
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

  dtd_init();

  dtd_main_loop();

  return 0;
}
