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

static void dtd_new( int fifo_number, const char *arg)
{
  int n_channels;

  sscanf( arg, "%d", &n_channels);
  dtd_handle_table[fifo_number].n_channels = n_channels;
}

static void dtd_open( int fifo_number, const char *arg)
{
  AFfilehandle file;
  dtdfifo_t *fifo;
  int file_channels, sampfmt, sampwidth, i;

  fifo = dtd_handle_table[fifo_number].fifo;

  dtdfifo_set_state( fifo, FIFO_INACTIVE);

  file = afOpenFile( arg, "r", 0);

  if (file == AF_NULL_FILEHANDLE)
    {
      fprintf( stderr, "[dtdserver] cannot open sound file %s\n", arg);
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

static void dtd_close( int fifo_number)
{
  dtdfifo_set_state( dtd_handle_table[fifo_number].fifo, FIFO_INACTIVE);

  if ( dtd_handle_table[fifo_number].file != AF_NULL_FILEHANDLE)
    {
      afCloseFile( dtd_handle_table[fifo_number].file);
      dtd_handle_table[fifo_number].file = AF_NULL_FILEHANDLE;
    }
}

/*
 * Commands are:
 * <fifo_number> new <number_of_channels>
 * <fifo_number> open <sound_file_name>
 * <fifo_number> close
 */
static void dtd_process_command( const char *line)
{
  int fifo_number;
  char command[N], arg[N];

  sscanf( line, "%d%s%s", &fifo_number, command, arg);

  if ( !strcmp( "new", command))
    dtd_new( fifo_number, arg);
  else if ( !strcmp( "open", command))
    dtd_open( fifo_number, arg);
  else if ( !strcmp( "close", command))
    dtd_close( fifo_number);
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
