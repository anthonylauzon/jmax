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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


/******************************************************************************/
/*                                                                            */
/*                              Shared memory Devices                         */
/*                                                                            */
/******************************************************************************/

#include <ulocks.h>
#include <assert.h>

#if 1
#include <stdio.h>
#endif

#include <fts/fts.h>

/**** SHM_FIFO_T ****/

typedef struct shm_fifo
{
  usptr_t *handle;		/* the shared arena handle */

  float *buf;			/* the buffer actually holding the */
  int size;			/* its total size */

  /* Reading */

  int    read_p;		/* the read pointer (index of the next sample to read) */
  int    read_pending;		/* the size a thread blocked on reading wants to read, 0 if nobody blocked */
  usema_t *read_semaphore;	/* the read (fifo empty) semaphor */
  int    read_status;		/* the out of band data written by the reader thread */

  /* Writing */

  int    write_p;		/* the write pointer (index of the next sample to write) */
  int    write_pending;		/* the size a thread blocked on writing wants to write, 0 if nobody blocked */
  usema_t *write_semaphore;	/* the write (fifo full) semaphor */
  int    write_status;		/* the out of band data written by the writer thread */

} shm_fifo_t;

void shm_fifo_free(shm_fifo_t *sf);

#if 0
static void shm_fifo_describe(const char *msg, shm_fifo_t *fifo)
{
  fprintf(stderr, "%s: FIFO %lx\n", msg, (unsigned int) fifo);
  fprintf(stderr, "\tbuf %lx\n", (unsigned int) fifo->buf);
  fprintf(stderr, "\tsize %d\n", fifo->size);
  fprintf(stderr, "\tread_p %d\n", fifo->read_p);
  fprintf(stderr, "\tread_block %d\n", fifo->read_block);
  fprintf(stderr, "\tread_pending %d\n", fifo->read_pending);
  fprintf(stderr, "\tread_status %d\n", fifo->read_status);
  fprintf(stderr, "\twrite_p %d\n", fifo->write_p);
  fprintf(stderr, "\twrite_block %d\n", fifo->write_block);
  fprintf(stderr, "\twrite_pending %d\n", fifo->write_pending);
  fprintf(stderr, "\twrite_status %d\n", fifo->write_status);
}
#endif

/* Create a new empty fifo, of a given size */

static shm_fifo_t *shm_fifo_allocate( usptr_t *handle, int size)
{
  shm_fifo_t *sf;

  sf = (shm_fifo_t *) usmalloc( sizeof( shm_fifo_t), handle);
  if ( !sf)
    {
#if 1
      fprintf( stderr, "usmalloc() failed [1]\n");
#endif
      return NULL;
    }

  sf->buf = (float *) usmalloc( sizeof(float) * size, handle);
  if ( !sf->buf)
    {
#if 1
      fprintf( stderr, "usmalloc() failed [2]\n");
#endif
      return NULL;
    }

  sf->size = size;

  sf->read_p       = 0;
  sf->read_pending = 0;
  sf->read_status  = 0;
  sf->read_semaphore = usnewsema( handle, 0);
  if ( sf->read_semaphore == NULL)
    {
#if 1
      fprintf( stderr, "usnewsema() failed [1]\n");
#endif
      return NULL;
    }

  sf->write_p = 0;
  sf->write_pending = 0;
  sf->write_status  = 0;
  sf->write_semaphore = usnewsema( handle, 0);
  if (sf->write_semaphore == NULL)
    {
#if 1
      fprintf( stderr, "usnewsema() failed [2]\n");
#endif
      return NULL;
    }

  return sf;
}


shm_fifo_t *shm_fifo_new( const char *name, int size)
{
  usptr_t *handle;
  shm_fifo_t *fifo;

  handle = usinit( name);

  if ( handle == NULL)
    {
#if 1
      fprintf( stderr, "usinit() failed\n");
#endif
      return NULL;
    }

  fifo = (shm_fifo_t *)usgetinfo( handle);

  while ( fifo == 0)
    {
      fifo = shm_fifo_allocate( handle, size);
      if (fifo == 0)
	return NULL;

      if ( uscasinfo( handle, 0, fifo) != 0)
	break;

      shm_fifo_free( fifo);
      sleep( 1);

      fifo = (shm_fifo_t *)usgetinfo( handle);
    }

  return fifo;
}

/* Destroy must be called when the two parties already agreed to give up
   synchronization; calling this function with a thread blocked inside the fifo
   will cause impredictable effects */

void shm_fifo_free(shm_fifo_t *sf)
{
  usptr_t *handle;

  handle = sf->handle;

  usfreesema( sf->read_semaphore, handle);
  usfreesema( sf->write_semaphore, handle);

  usfree( sf->buf, handle);
  usfree( sf, handle);

  usdetach( handle);
}

/* Ask for a pointer to a buffer ofsample to be read; size of the buffer
   is the declared read size at the sample fifo new time.
   return a pointer to the sample buffer; 
   the buffer is locked until the call to _got (see below) */

float *shm_fifo_want_to_get(shm_fifo_t *sf, int size)
{
  if ((sf->write_p - sf->read_p + sf->size) % sf->size < size)
    {
      sf->read_pending = size;
      uspsema( sf->read_semaphore);
      sf->read_pending = 0;
    }

  return sf->buf + sf->read_p;
}

/* Signal that the last get operation is completed, and the used buffer
   is empty; it may wake up a thread blocked in want_to_put if unlocking
   this buffer make the space available .
 */

void shm_fifo_got(shm_fifo_t *sf, int size)
{
  sf->read_p = (sf->read_p + size) % sf->size;

  if (sf->write_pending && ((sf->read_p == sf->write_p) ||
			    ((sf->read_p - sf->write_p + sf->size) % sf->size  > sf->write_pending)))
    {
      usvsema( sf->write_semaphore);
    }
}


/* Ask for a pointer to a buffer of 'samples' sample in which to write; samples must
   be a divisor of the fifo size; it can blocks the thread until
   the buffer is available; return a pointer to the buffer to be filled*/

float *shm_fifo_want_to_put(shm_fifo_t *sf, int size)
{
  if ((sf->read_p != sf->write_p) && (sf->read_p - sf->write_p + sf->size) % sf->size  <= size)
    {
      sf->write_pending = size;
      uspsema( sf->write_semaphore);
      sf->write_pending = 0;
    }

  return sf->buf + sf->write_p;
}


/* Signal that the last put operation is completed, and the used buffer
   is full; it may wake up a thread blocked in want_to_get if unlocking
   this buffer make the samples available .
 */

void shm_fifo_putted(shm_fifo_t *sf, int size)
{
  sf->write_p = (sf->write_p + size) % sf->size;

  if (sf->read_pending && ((sf->write_p - sf->read_p + sf->size) % sf->size >= sf->read_pending))
    {
      usvsema( sf->read_semaphore);
    }
}

/* Put the out of band command; note that in a given application,
   either the reader or the writer should write the cmd, but not both,
   and the other should read it; also, the sample fifo give no provision
   of extra synchronization, if the command reader is currently blocked,
   it will read the command when waken up by the normal read/write operations */

void shm_fifo_set_read_status(shm_fifo_t *sf, int v)
{
  sf->read_status = v;
}

int shm_fifo_get_read_status(shm_fifo_t *sf)
{
  return sf->read_status;
}

void shm_fifo_set_write_status(shm_fifo_t *sf, int v)
{
  sf->write_status = v;
}

int shm_fifo_get_write_status(shm_fifo_t *sf)
{
  return sf->write_status;
}

/* ********************************************************************** */
/*                                                                        */
/* The device itself                                                      */
/*                                                                        */
/* ********************************************************************** */

static void shmout_init(void);
static void shmin_init(void);

void shmdev_init( void)
{
  shmout_init();
  shmin_init();
}

/* Common to out and in */
typedef struct 
{
  int n_channels;
  shm_fifo_t *fifo;
} shm_data_t;

#define SHM_CLOSE	1
#define SHM_ACTIVE	2

/* Forward declarations of OUT/IN dev and class static functions */

static int          shm_get_nchans( fts_dev_t *dev);

static fts_status_t shmout_open( fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t shmout_close( fts_dev_t *dev);
static void         shmout_put( fts_word_t *args);

static fts_status_t shmin_open( fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t shmin_close( fts_dev_t *dev);
static void         shmin_get( fts_word_t *args);

/* Common OUT/IN get_nchans */
static int shm_get_nchans( fts_dev_t *dev)
{
  shm_data_t *dev_data = (shm_data_t *) fts_dev_get_device_data(dev);

  return dev_data->n_channels;
}

/* OUT device installation */
static void shmout_init(void)
{
  fts_dev_class_t *class;

  class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("shmOut"));

  fts_dev_class_set_open_fun( class, shmout_open);
  fts_dev_class_set_close_fun( class, shmout_close);

  fts_dev_class_sig_set_put_fun( class, shmout_put);

  fts_dev_class_sig_set_get_nchans_fun( class, shm_get_nchans);
}

/* OUT device functions */
static fts_status_t shmout_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  shm_data_t *dev_data;
  fts_symbol_t name;
  shm_fifo_t *fifo;
  int size;

  dev_data = (shm_data_t *)fts_malloc( sizeof( shm_data_t));
  fts_dev_set_device_data( dev, dev_data);

  name = fts_get_symbol_by_name( nargs, args, fts_new_symbol( "name"), fts_new_symbol( "/tmp/shmdev"));

  dev_data->n_channels = fts_get_int_by_name( nargs, args, fts_new_symbol("channels"), 2);

  size = fts_param_get_int( fts_s_fifo_size, 256);

  fifo = shm_fifo_new( fts_symbol_name( name), size * dev_data->n_channels);
  if (fifo == NULL)
    return &fts_dev_open_error;

  dev_data->fifo = fifo;

  shm_fifo_set_write_status( fifo, SHM_ACTIVE);

  return fts_Success;
}

static fts_status_t shmout_close( fts_dev_t *dev)
{
  shm_data_t *dev_data = (shm_data_t *) fts_dev_get_device_data(dev);

  /* Signal EOF to the fifo */
  shm_fifo_set_write_status( dev_data->fifo, SHM_CLOSE);

  shm_fifo_free( dev_data->fifo);

  fts_free( dev_data);

  return fts_Success;
}

/* OUT put (Arguments: fts_dev_t *dev, int n, float *buf1 ... *bufn) */
static void shmout_put(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  int n = fts_word_get_int(argv + 2);
  shm_data_t *dev_data;
  int i, j, n_channels, ch;

  dev_data = fts_dev_get_device_data(dev);
  n_channels = fts_word_get_int(argv + 1);

  if ( shm_fifo_get_read_status( dev_data->fifo) == SHM_ACTIVE)
    {
      float *buf;

      buf = shm_fifo_want_to_put( dev_data->fifo, n * n_channels );

      for ( ch = 0; ch < n_channels; ch++)
	{
	  float *in = (float *) fts_word_get_ptr(argv + 3 + ch);
	  
	  i = ch;
	  for ( j = 0; j < n; j++)
	    {
	      buf[i] = in[j];
	      i += n_channels;
	    }
	}

      shm_fifo_putted( dev_data->fifo, n * n_channels);
    }
}

/* IN device installation */
static void shmin_init(void)
{
  fts_dev_class_t *class;

  class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("shmIn"));

  fts_dev_class_set_open_fun( class, shmin_open);
  fts_dev_class_set_close_fun( class, shmin_close);
  fts_dev_class_sig_set_get_fun( class, shmin_get);
  fts_dev_class_sig_set_get_nchans_fun( class, shm_get_nchans);
}

/* IN device functions */
static fts_status_t shmin_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  shm_data_t *dev_data;
  fts_symbol_t name;
  shm_fifo_t *fifo;
  int size;

  dev_data = (shm_data_t *)fts_malloc( sizeof( shm_data_t));
  fts_dev_set_device_data( dev, dev_data);

  name = fts_get_symbol_by_name( nargs, args, fts_new_symbol( "name"), fts_new_symbol( "/tmp/shmdev"));

  dev_data->n_channels = fts_get_int_by_name( nargs, args, fts_new_symbol("channels"), 2);

  size = fts_param_get_int( fts_s_fifo_size, 256);
  
  fifo = shm_fifo_new( fts_symbol_name( name), size * dev_data->n_channels);
  if (fifo == NULL)
    return &fts_dev_open_error;

  dev_data->fifo = fifo;

  shm_fifo_set_read_status( fifo, SHM_ACTIVE);

  return fts_Success;
}

static fts_status_t shmin_close( fts_dev_t *dev)
{
  shm_data_t *dev_data = (shm_data_t *) fts_dev_get_device_data(dev);

  /* Signal EOF to the fifo */
  shm_fifo_set_read_status( dev_data->fifo, SHM_CLOSE);

  shm_fifo_free( dev_data->fifo);

  fts_free( dev_data);

  return fts_Success;
}

/* IN get (Arguments: fts_dev_t *dev, int n, float *buf1 ... *bufn) */
static void shmin_get(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  int n = fts_word_get_int(argv + 2);
  shm_data_t *dev_data;
  int i, j, n_channels, ch;

  dev_data = fts_dev_get_device_data(dev);
  n_channels = fts_word_get_int(argv + 1);

  if ( shm_fifo_get_write_status( dev_data->fifo) == SHM_ACTIVE)
    {
      float *buf;

      buf = shm_fifo_want_to_get( dev_data->fifo, n * n_channels );

      for ( ch = 0; ch < n_channels; ch++)
	{
	  float *out = (float *) fts_word_get_ptr(argv + 3 + ch);
	  
	  i = ch;
	  for ( j = 0; j < n; j++)
	    {
	      out[j] = buf[i];
	      i += n_channels;
	    }
	}

      shm_fifo_got( dev_data->fifo, n * n_channels);
    }
  else
    {
      /* Fill with zeros */
      for ( ch = 0; ch < n_channels; ch++)
	{
	  float *out = (float *) fts_word_get_ptr(argv + 3 + ch);

	  for ( j = 0; j < n; j++)
	    out[j] = 0.0;
	}
    }
}
