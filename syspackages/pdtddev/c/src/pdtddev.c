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


#include <pthread.h>
#include <audiofile.h>
#include <string.h>
#include "fts.h"


/* Implementation of asynchronius io for sound files,
   based on pthreads */

/**** FTS_SAMPLE_FIFO_T ****/

/* The fts_sample_fifo_t is a fifo of samples; to simplify, read and
   writes are performed with fixed (but potentially different) block
   size, that *must* be divisor of the fifo length.  It include
   pthread based synchronization, and a command word for each
   direction for out of band data exchanged from one side to the other
   (in any direction, provided synchronization is consistent).  The
   read/write operations are organized in a way they can be in place,
   without any need to copy data.

   Note the reader and the writer should be two different pthreads;
   mixing read and write operation on the same fifo in the same pthread
   will produce unpredictable operations; also, there is no provision
   for multiple reader/writers.

   Fifo can be used for multichannel signals, possibly using an interleaved
   format for the channells, or using some kind of convention of channel packeting.
   really the fifo do no assumption on the semantic of the content.

   Reader and writers can close their part of the communication; once both
   part are closed, we destroy the fifo; before, we call a destroy callback,
   that can be used to destroy other shared resources used for the communication.
   */

#ifdef HAVE_AF_VIRTUAL_PARAMETERS
typedef float fifo_sample_t;
#else
typedef short fifo_sample_t;
#endif

typedef struct fts_sample_fifo
{
  fifo_sample_t *buf;		/* the buffer actually holding the */
  int    size;			/* its total size */

  /* Reading */

  int    read_p;		/* the read pointer (index of the next sample to read) */
  int    read_block;		/* the size of the blocks being read, constant */
  int    read_pending;		/* != zero if a thread is blocked on reading */
  pthread_cond_t read_cond;	/* the read (fifo empty) semaphor */
  int    reader_eof;		/* the reader eof flag */

  /* Writing */

  int    write_p;		/* the write pointer (index of the next sample to write) */
  int    write_block;		/* the size of the blocks being read, constant */
  int    write_pending;		/* != zero if a thread is blocked on writing */
  pthread_cond_t write_cond;	/* the write (fifo full) semaphor */
  int    writer_eof;		/* the writer eof flag */

  /* Destroy call back*/

  void   (* destroy_callback)(void *);
  void   *destroy_callback_data;

  /* Mutex */

  pthread_mutex_t mutex;	/* a mutex to lock the whole structure */

} fts_sample_fifo_t;


static fts_heap_t *sample_fifo_heap;

/* Create a new empty fifo, of a given size */

static fts_sample_fifo_t *fts_sample_fifo_new(int size, int read_block, int write_block)
{
  fts_sample_fifo_t *sf= (fts_sample_fifo_t *) fts_heap_alloc(sample_fifo_heap);

  sf->buf = (fifo_sample_t *) fts_malloc(sizeof(fifo_sample_t) * size);
  sf->size = size;

  sf->read_p       = 0;
  sf->read_block   = read_block;
  sf->read_pending = 0;
  sf->reader_eof   = 0;
  pthread_cond_init(&(sf->read_cond), NULL);

  sf->write_p = 0;
  sf->write_block   = write_block;
  sf->write_pending = 0;
  sf->writer_eof    = 0;
  pthread_cond_init(&(sf->write_cond), NULL);

  sf->destroy_callback = 0;
  sf->destroy_callback_data = 0;

  pthread_mutex_init(&(sf->mutex), NULL);

  return sf;
}

static void fts_sample_fifo_set_destroy_callback(fts_sample_fifo_t *sf, void (* fun)(void *), void *data)
{
  sf->destroy_callback = fun;
  sf->destroy_callback_data = data;
}

/* Destroy must be called when the two parties already agreed to give up
   synchronization; calling this function with a thread blocked inside the fifo
   will cause impredictable effects */

static void fts_sample_fifo_destroy(fts_sample_fifo_t *sf)
{
  if (sf->destroy_callback)
    (* sf->destroy_callback)(sf->destroy_callback_data);

  pthread_cond_destroy(&(sf->read_cond));
  pthread_cond_destroy(&(sf->write_cond));
  pthread_mutex_destroy(&(sf->mutex));
  fts_free(sf->buf);
  fts_heap_free((char *) sf, sample_fifo_heap);
}

/* Ask for a pointer to a buffer ofsample to be read; size of the buffer
   is the declared read size at the sample fifo new time.
   Return the a pointer to the sample buffer thru a pointer argument,
   and the number of samples the called can actually write; if the value
   is less than the read block size, we are in writer eof situation,
   no more read should be tryied.

   the buffer is locked until the call to _got (see below) */

static int fts_sample_fifo_want_to_get(fts_sample_fifo_t *sf, fifo_sample_t **p)
{
  int ret;

  if (sf->writer_eof)
    {
      *p = (fifo_sample_t *)0;
      return 0;
    }

  pthread_mutex_lock(&(sf->mutex));

  if (((sf->write_p - sf->read_p + sf->size) % sf->size) < sf->read_block)
    {
      sf->read_pending = 1;
      pthread_cond_wait(&(sf->read_cond), &(sf->mutex));
      sf->read_pending = 0;

      if (((sf->write_p - sf->read_p + sf->size) % sf->size) < sf->read_block)
	ret = (sf->write_p - sf->read_p + sf->size) % sf->size;
      else
	ret = sf->read_block;
    }
  else
    ret = sf->read_block;

  pthread_mutex_unlock(&(sf->mutex));

  (*p) = sf->buf + sf->read_p;

  return ret;
}

/* Signal that the last get operation is completed, and the used buffer
   is empty; it may wake up a thread blocked in want_to_put if unlocking
   this buffer make the space available .

   Do nothing in case the writer is in eof.
 */

static void fts_sample_fifo_got(fts_sample_fifo_t *sf)
{
  if (sf->writer_eof)
    return;

  pthread_mutex_lock(&(sf->mutex));

  sf->read_p = (sf->read_p + sf->read_block) % sf->size;

  if (sf->write_pending && ((sf->read_p == sf->write_p) ||
			    ((sf->read_p - sf->write_p + sf->size) % sf->size  > sf->write_block)))
    {
      pthread_cond_signal(&(sf->write_cond));
    }

  pthread_mutex_unlock(&(sf->mutex));
}


/* Ask for a pointer to a buffer of 'samples' sample in which to write; samples must
   be a divisor of the fifo size; it can blocks the thread until
   the buffer is available */

static int fts_sample_fifo_want_to_put(fts_sample_fifo_t *sf, fifo_sample_t **p)
{
  int ret;

  if (sf->reader_eof)
    {
      *p = (fifo_sample_t *)0;
      return 0;
    }

  pthread_mutex_lock(&(sf->mutex));

  if ((sf->read_p != sf->write_p) && (sf->read_p - sf->write_p + sf->size) % sf->size  <= sf->write_block)
    {
      sf->write_pending = 1;
      pthread_cond_wait(&(sf->write_cond), &(sf->mutex));
      sf->write_pending = 0;

      if ((sf->read_p != sf->write_p) && (sf->read_p - sf->write_p + sf->size) % sf->size  <= sf->write_block)
	ret = (sf->read_p - sf->write_p + sf->size) % sf->size;
      else
	ret = sf->write_block;
    }
  else
    ret = sf->write_block;

  pthread_mutex_unlock(&(sf->mutex));

  (*p) = sf->buf + sf->write_p;

  return ret;
}


/* Signal that the last put operation is completed, and the used buffer
   is full; it may wake up a thread blocked in want_to_get if unlocking
   this buffer make the samples available .
 */

static void fts_sample_fifo_putted(fts_sample_fifo_t *sf)
{
  if (sf->reader_eof)
    return;

  pthread_mutex_lock(&(sf->mutex));

  sf->write_p = (sf->write_p + sf->write_block) % sf->size;

  if (sf->read_pending && ((sf->write_p - sf->read_p + sf->size) % sf->size >= sf->read_block))
    {
      pthread_cond_signal(&(sf->read_cond));
    }

  pthread_mutex_unlock(&(sf->mutex));
}

static int fts_sample_fifo_get_read_block(fts_sample_fifo_t *sf)
{
  return sf->read_block;
}

static int fts_sample_fifo_get_write_block(fts_sample_fifo_t *sf)
{
  return sf->write_block;
}

/* 
   Controlling the sample fifo
   */

/* Called by the reader, to tell
   the writer that nobody will read anymore from the fifo;
   if the writer is waiting on a want_to_put, it will be
   waken up with an error return value.
   After this call fifo will be closed, any want_to/done operation will return
   an error value.
   */

static void fts_sample_fifo_reader_eof(fts_sample_fifo_t *sf)
{
  pthread_mutex_lock(&(sf->mutex));

  sf->reader_eof = 1;

  if (sf->write_pending)
    pthread_cond_signal(&(sf->write_cond));

  if (sf->writer_eof)
    fts_sample_fifo_destroy(sf);

  pthread_mutex_unlock(&(sf->mutex));
}

/* Called by the writer, to tell
   the reader that nobody will write anymore from the fifo;
   if the reader is waiting on a want_to_get, it will be
   waken up with an error return value (telling how many samples are left
   on the fifo).
   After this call fifo will be closed, any want_to/done operation will return
   an error value.
   */


static void fts_sample_fifo_writer_eof(fts_sample_fifo_t *sf)
{
  pthread_mutex_lock(&(sf->mutex));

  sf->writer_eof = 1;

  if (sf->read_pending)
    pthread_cond_signal(&(sf->read_cond));

  if (sf->reader_eof)
    fts_sample_fifo_destroy(sf);

  pthread_mutex_unlock(&(sf->mutex));
}

static int fts_sample_fifo_is_reader_eof(fts_sample_fifo_t *sf)
{
  return sf->reader_eof;
}

static int fts_sample_fifo_is_writer_eof(fts_sample_fifo_t *sf)
{
  return sf->writer_eof;
}


static void fts_sample_fifo_init(void)
{
  sample_fifo_heap = fts_heap_new(sizeof(fts_sample_fifo_t));
}


/****  FTS_CMD_FIFO_T ****/

/* A fts_cmd_fifo_t is used to send commands to the master thread;
   the master thread will spawn workers threads, one for each file.
   The main reason for the existence of the master thread is to reduce
   the risk of delays at thread creation; test have shown that on the SGI
   a thread creation take around 1 mSec (or less) plus occasionally have 2 or 3
   millisecond of delay; in order to reduce the risk of dac slip at low
   latency, we create the threads in the master thread.
   */

#define CMD_FIFO_SIZE 64

typedef struct fts_cmd_fifo
{
  struct { void (*fun)(void *); void *v;} commands[CMD_FIFO_SIZE];

  /* Reading */

  int    read_p;		/* the read pointer (index of the next command to execute) */
  int    read_pending;		/* != zero if a thread is blocked on reading */
  pthread_cond_t read_cond;	/* the read (fifo empty) semaphor */

  /* Writing */

  int    write_p;		/* the write pointer (index of the first command to write) */
  int    write_pending;		/* != zero if a thread is blocked on writing */
  pthread_cond_t write_cond;	/* the write (fifo full) semaphor */

  /* Mutex */

  pthread_mutex_t mutex;	/* a mutex to lock the whole structure */

} fts_cmd_fifo_t;


static fts_heap_t *cmd_fifo_heap;

/* Create a new empty fifo, of a given size */

static fts_cmd_fifo_t *fts_cmd_fifo_new(void)
{
  fts_cmd_fifo_t *cf= (fts_cmd_fifo_t *) fts_heap_alloc(cmd_fifo_heap);

  cf->read_p        = 0;
  cf->read_pending  = 0;
  pthread_cond_init(&(cf->read_cond), NULL);

  cf->write_p       = 0;
  cf->write_pending = 0;
  pthread_cond_init(&(cf->write_cond), NULL);

  pthread_mutex_init(&(cf->mutex), NULL);

  return cf;
}


/* Destroy must be called when the two parties already agreed to give up
   synchronization; calling this function with a thread blocked inside the fifo
   will cause impredictable effects */

static void fts_cmd_fifo_destroy(fts_cmd_fifo_t *cf)
{
  pthread_cond_destroy(&(cf->read_cond));
  pthread_cond_destroy(&(cf->write_cond));
  pthread_mutex_destroy(&(cf->mutex));
  fts_heap_free((char *) cf, cmd_fifo_heap);
}

/* Get a command, and execute it, outside of the lock */
  
static void fts_cmd_fifo_exec_one(fts_cmd_fifo_t *cf)
{
  void (*fun)(void *);
  void *v;

  pthread_mutex_lock(&(cf->mutex));

  if (! ((cf->read_p > cf->write_p) || (cf->write_p - cf->read_p) >= 1))
    {
      cf->read_pending = 1;
      pthread_cond_wait(&(cf->read_cond), &(cf->mutex));
      cf->read_pending = 0;
    }

  fun = cf->commands[cf->read_p].fun;
  v = cf->commands[cf->read_p].v;

  cf->read_p = (cf->read_p + 1) % CMD_FIFO_SIZE;

  if (cf->write_pending && ((cf->write_p >= cf->read_p) || (cf->read_p - cf->write_p) > 1))
    {
      pthread_cond_signal(&(cf->write_cond));
    }

  pthread_mutex_unlock(&(cf->mutex));

  (* fun)(v);
}
  


/* Ask for a pointer to a buffer of 'samples' sample in which to write; samples must
   be a divisor of the fifo size; it can blocks the thread until
   the buffer is available; return a pointer to the buffer to be filled*/

static void fts_cmd_fifo_add_command(fts_cmd_fifo_t *cf,  void (*fun)(void *), void *v)
{
  pthread_mutex_lock(&(cf->mutex));

  if (! ((cf->write_p >= cf->read_p) || (cf->read_p - cf->write_p) > 1))
    {
      cf->write_pending = 1;
      pthread_cond_wait(&(cf->write_cond), &(cf->mutex));
      cf->write_pending = 0;
    }

  cf->commands[cf->write_p].fun = fun;
  cf->commands[cf->write_p].v = v;

  cf->write_p = (cf->write_p + 1) % CMD_FIFO_SIZE;

  if (cf->read_pending && ((cf->read_p > cf->write_p) || (cf->write_p - cf->read_p) >= 1))
    {
      pthread_cond_signal(&(cf->read_cond));
    }

  pthread_mutex_unlock(&(cf->mutex));
}

static void fts_cmd_fifo_init(void)
{
  cmd_fifo_heap = fts_heap_new(sizeof(fts_cmd_fifo_t));
}


/****  COMMAND THREAD (for executing commands) ****/

static fts_cmd_fifo_t *async_call_fifo;
static pthread_t async_call_thread;

static void *fts_async_sched(void *ignore)
{
  while (1)
    {
      fts_cmd_fifo_exec_one(async_call_fifo);
    }
}


static void fts_async_call(void (*fun)(void *), void *v)
{
  fts_cmd_fifo_add_command(async_call_fifo, fun, v);
}

static void fts_async_init(void)
{
  int rc;

  /* Build the command fifo */

  async_call_fifo = fts_cmd_fifo_new();

  /* Start the async command thread */

  rc = pthread_create(&async_call_thread, NULL, fts_async_sched, 0);

  if (rc)
    fprintf(stderr, "fts_async_init: return code from pthread_create() is %d\n", rc);
}


/** READSF **/


static fts_status_t pdtd_readsf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t pdtd_readsf_close(fts_dev_t *dev);

static void pdtd_readsf_get(fts_word_t *argv);

static fts_status_t pdtd_readsf_activate(fts_dev_t *dev);
static fts_status_t pdtd_readsf_deactivate(fts_dev_t *dev);
static int          pdtd_readsf_get_nchans(fts_dev_t *dev);

static void fts_readsf_forker(void *data);


static void pdtd_readsf_init(void)
{
  fts_dev_class_t *pdtd_readsf_class;

  /* dac file */

  pdtd_readsf_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("readsf"));

  /* Installation of all the device class functions */

  fts_dev_class_set_open_fun(pdtd_readsf_class, pdtd_readsf_open);
  fts_dev_class_set_close_fun(pdtd_readsf_class, pdtd_readsf_close);

  fts_dev_class_sig_set_get_fun(pdtd_readsf_class, pdtd_readsf_get);

  fts_dev_class_sig_set_activate_fun(pdtd_readsf_class, pdtd_readsf_activate);
  fts_dev_class_sig_set_deactivate_fun(pdtd_readsf_class, pdtd_readsf_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(pdtd_readsf_class, pdtd_readsf_get_nchans);
}


/* The device data do not contain the AF file  descriptor, beacause
   it is a private data of the reader thread. */

struct readsf_data
{
  /* The reader thread */

  pthread_t reader_thread;

  /* The sample fifo that connect the read thread to us */

  fts_sample_fifo_t *fifo;

  int file_block;
  int fifo_size;

  /* main thread (open) data */

  fts_symbol_t file_name;	/* the file name, filled by open */
  int nch;			/* number of channels, fill*/

  /* housekeeping */

  int active; /* Used directly by the readsf object */
};


/* Destroy callback */

void readsf_destroy_data(void *dev_data)
{
  fts_free(dev_data);
}

static fts_status_t
pdtd_readsf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct readsf_data *dev_data;

  if ((nargs < 1) || (! fts_is_symbol(&args[0])))
    return &fts_dev_open_error;

  /* make the structure */
  dev_data = (struct readsf_data *) fts_malloc(sizeof(struct readsf_data));
  fts_dev_set_device_data(dev, dev_data);

  /* get the file name */
  dev_data->file_name = fts_get_symbol(&args[0]);
  dev_data->active = 0;

  /* parse the other file parameters : channels 
     Note that while in theory the device should automatically get the number
     of channels of the file, since this device is used by the readsf and family
     objects, the read number of channels is fixed by the object, and not by the file;
     anyway, it seems that the SGI library can fix this, by automagically mixing
     the different tracks to get the needed number of channels.
     No test is done on the number of channels, we just accept everything
     the af library accept; will be used after the opening to set the 
     virtual channels.
     */

  dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);

  dev_data->file_block = fts_get_int_by_name(nargs, args, fts_new_symbol("fileblock"), 16 * 1024);
  dev_data->fifo_size  = fts_get_int_by_name(nargs, args, fts_new_symbol("fifosize"),  64 * 1024);

  /* Make a sample fifo */

  dev_data->fifo = fts_sample_fifo_new(dev_data->fifo_size, MAXVS * dev_data->nch, dev_data->file_block);
  fts_sample_fifo_set_destroy_callback(dev_data->fifo, readsf_destroy_data, dev_data);
  /*  Start the reader thread  using a async call*/

  fts_async_call(fts_readsf_forker, (void *)dev_data);

  return fts_Success;
}

static fts_status_t
pdtd_readsf_close(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = (struct readsf_data *) fts_dev_get_device_data(dev);

  fts_sample_fifo_reader_eof(dev_data->fifo);

  return fts_Success;
}


static fts_status_t pdtd_readsf_activate(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 1;

  return fts_Success;
}


static fts_status_t pdtd_readsf_deactivate(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 0;

  return fts_Success;
}

static int
pdtd_readsf_get_nchans(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  return dev_data->nch;
}


static void
pdtd_readsf_get(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  struct readsf_data *dev_data;
  int nchans;
  int n;
  int ch;
  int i,j;
  int doit = 0;
  fifo_sample_t *in;
  int ret;

  nchans = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  if (dev)
    {
      dev_data = fts_dev_get_device_data(dev);

      /* return all zeros if not active or if write only or in the eof case */

      if (dev_data->active)
	doit = 1;
    }

  if (doit)
    ret = fts_sample_fifo_want_to_get(dev_data->fifo, &in);
  else
    ret = 0;

  if (ret == nchans * MAXVS)
    {
      /* do the data transfer, transforming from interleaved to separate channels */

      for (ch = 0; ch < nchans; ch++)
	{
	  float *out;

	  out = (float *) fts_word_get_ptr(argv + 3 + ch);

	  for (i = ch, j = 0; j < n; i = i + nchans, j++)
	    {
#ifdef HAVE_AF_VIRTUAL_PARAMETERS
	      out[j] = in[i];
#else
	      out[j] = ((float) in[i] / 32768.0f);
#endif
	    }
	}
    }
  else
    {
      /* do a partial data transfer, fill what missing with zeros */
      
      for (ch = 0; ch < nchans; ch++)
	{
	  float *out;

	  out = (float *) fts_word_get_ptr(argv + 3 + ch);
	  
	  for (i = ch, j = 0; j < n; i = i + nchans, j++)
	    {
	      if (i < ret)
		{
#ifdef HAVE_AF_VIRTUAL_PARAMETERS
		  out[j] = in[i];
#else
		  out[j] = ((float) in[i] / 32768.0f);
#endif	  
		}
	      else
		out[j] = 0.0f;
	    }
	}
    }

  /* Unlock the buffer if needed */

  if (doit)
    fts_sample_fifo_got(dev_data->fifo);
}


/* READER THREAD: MUST started by an async call; will read the file
   file_block samples at a time; note that this value should be the same
   used to initialize the sample fifo.
*/

static void *fts_readsf_worker(void *data)
{
  int i;
  int ret;
  int eof = 0;
  AFfilehandle  file;		
  struct readsf_data *dev_data = (struct readsf_data *)data;
  fts_sample_fifo_t *fifo = dev_data->fifo;
  int frames_for_block = (dev_data->file_block / dev_data->nch);

  /* Actually Open the audio file  */

  file = afOpenFile(fts_symbol_name(dev_data->file_name), "r", 0);

  if (file == AF_NULL_FILEHANDLE)
    {
      eof = 1;
      fts_sample_fifo_writer_eof(fifo);
    }
  else
    {
      /* Note that on Linux the number of channels must match ! */

#ifdef HAVE_AF_VIRTUAL_PARAMETERS
      /* Set the number of virtual channels */

      afSetVirtualChannels(file, AF_DEFAULT_TRACK, dev_data->nch);

      /* Set the virtual format of the file */

      afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
#else

      /* If we dont have the virtual format translation, we do a check
	 on a the actual number of channels we got, and the bit size and format,
	 and give do an eof if
	 they do not match; we currently have no way to signal the error
	 from this thread.
      */

      if (afGetChannels(file, AF_DEFAULT_TRACK) != dev_data->nch)
	{
	  eof = 1;
	  fts_sample_fifo_writer_eof(fifo);
	}
      
      {
	int sampfmt;
	int sampwidth;

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

	if ((sampfmt != AF_SAMPFMT_TWOSCOMP) || (sampwidth != 16))
	  fts_sample_fifo_writer_eof(fifo);
      }
#endif
    }

  /*  LOOP:  read the file -> Write to the sample fifo */

  while (! eof)
    {
      int ret;
      fifo_sample_t *p;

      ret = fts_sample_fifo_want_to_put(fifo, &p);

      /**Check ret; if it is less than the block we are in 
	reader_eof situation; stop the loop and close the file.
	We do not need to write partial blocks, because on the other side
	there is nobody that read */

      if (ret != dev_data->file_block)
	eof = 1;
      else
	{
	  ret = afReadFrames(file, AF_DEFAULT_TRACK, p, frames_for_block);

	  if (ret != frames_for_block)
	    {
	      /* Fill the remained of the buffer with zeros */

	      for (i = ret * dev_data->nch; i < dev_data->file_block; i++)
		p[i] = 0.0f;

	      /* Set the eof flag */

	      eof = 1;
	    }
	  else
	    fts_sample_fifo_putted(fifo);
	}
    }

  /* Close the file, free all the structures and exit the thread */

  fts_sample_fifo_writer_eof(fifo);

  if (file != AF_NULL_FILEHANDLE)
    afCloseFile(file);
  
  return NULL;
}

/* The function called async to create a reader thread */

static void fts_readsf_forker(void *data)
{
  int rc;
  struct readsf_data *dev_data = (struct readsf_data *)data;

  rc = pthread_create(&(dev_data->reader_thread), NULL, fts_readsf_worker, data);

  if (rc)
    fprintf(stderr, "fts_readsf_forker_init: return code from pthread_create() is %d\n", rc);
}


/** WRITESF **/


static fts_status_t pdtd_writesf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t pdtd_writesf_close(fts_dev_t *dev);

static void pdtd_writesf_put(fts_word_t *argv);

static fts_status_t pdtd_writesf_activate(fts_dev_t *dev);
static fts_status_t pdtd_writesf_deactivate(fts_dev_t *dev);
static int          pdtd_writesf_get_nchans(fts_dev_t *dev);

static void fts_writesf_forker(void *data);

static void pdtd_writesf_init(void)
{
  fts_dev_class_t *pdtd_writesf_class;

  /* dac file */

  pdtd_writesf_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("writesf"));

  /* Installation of all the device class functions */

  fts_dev_class_set_open_fun(pdtd_writesf_class, pdtd_writesf_open);
  fts_dev_class_set_close_fun(pdtd_writesf_class, pdtd_writesf_close);

  fts_dev_class_sig_set_put_fun(pdtd_writesf_class, pdtd_writesf_put);

  fts_dev_class_sig_set_activate_fun(pdtd_writesf_class, pdtd_writesf_activate);
  fts_dev_class_sig_set_deactivate_fun(pdtd_writesf_class, pdtd_writesf_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(pdtd_writesf_class, pdtd_writesf_get_nchans);
}


/* The device data do not contain the file  descriptor, beacause
   it is a private data of the writer thread. */

struct writesf_data
{
  /* The reader thread */

  pthread_t writer_thread;

  /* The sample fifo that connect the read thread to us */

  fts_sample_fifo_t *fifo;

  int file_block;
  int fifo_size;

  /* main thread (open) data */

  fts_symbol_t file_name;	/* the file name, filled by open */
  fts_atom_t *format_descr;	/* the pointer to the format descriptor */
  int nch;			/* number of channels, fill*/
  float sr;			/* Sampling rate to use for the file */

  /* housekeeping */

  int active; /* Used directly by the writesf object */
};


void writesf_destroy_data(void *dev_data)
{
  fts_free(dev_data);
}


static fts_status_t
pdtd_writesf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t format_name;
  struct writesf_data *dev_data;

  if ((nargs < 1) || (! fts_is_symbol(&args[0])))
    return &fts_dev_open_error;

  /* make the structure */

  dev_data = (struct writesf_data *) fts_malloc(sizeof(struct writesf_data));
  fts_dev_set_device_data(dev, dev_data);

  /* get the file name */

  dev_data->file_name = fts_get_symbol(&args[0]);
  dev_data->active = 0;

  /* parse the other file parameters : channels and format.
     Note that while in theory the device should automatically get the number
     of channels of the file, since this device is used by the writesf and family
     objects, the read number of channels is fixed by the object, and not by the file;
     anyway, it seems that the SGI library can fix this, by automagically mixing
     the different tracks to get the needed number of channels.
     No test is done on the number of channels, we just accept everything
     the af library accept; will be used after the opening to set the 
     virtual channels.
     */

  dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  dev_data->sr  = fts_get_float_by_name(nargs, args, fts_s_sampling_rate, 44100.0f); /* mandatory! */

  format_name = fts_get_symbol_by_name(nargs, args, fts_new_symbol("format"), fts_s_void);

  if (format_name == fts_s_void)
    {
      /* Try with the filename extension */

      char *extension = strrchr(fts_symbol_name(dev_data->file_name), '.');
      
      if (extension)
	format_name = fts_new_symbol(extension + 1);
      else
	format_name = fts_soundfile_format_get_default();
    }

  dev_data->format_descr = fts_soundfile_format_get_descriptor(format_name);
  dev_data->file_block = fts_get_int_by_name(nargs, args, fts_new_symbol("fileblock"), 16 * 1024);
  dev_data->fifo_size  = fts_get_int_by_name(nargs, args, fts_new_symbol("fifosize"),  64 * 1024);

  /* Make a sample fifo */

  dev_data->fifo = fts_sample_fifo_new(dev_data->fifo_size, dev_data->file_block, MAXVS * dev_data->nch);
  fts_sample_fifo_set_destroy_callback(dev_data->fifo, writesf_destroy_data, dev_data);

  /*  Start the reader thread  using a async call*/

  fts_async_call(fts_writesf_forker, (void *)dev_data);

  return fts_Success;
}

static fts_status_t
pdtd_writesf_close(fts_dev_t *dev)
{
  struct writesf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  /* Just tell the worker thread to close and destroy everything */

  fts_sample_fifo_writer_eof(dev_data->fifo);

  return fts_Success;
}


static fts_status_t pdtd_writesf_activate(fts_dev_t *dev)
{
  struct writesf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 1;

  return fts_Success;
}


static fts_status_t pdtd_writesf_deactivate(fts_dev_t *dev)
{
  struct writesf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 0;

  return fts_Success;
}

static int
pdtd_writesf_get_nchans(fts_dev_t *dev)
{
  struct writesf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  return dev_data->nch;
}


static void
pdtd_writesf_put(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  struct writesf_data *dev_data;
  int nchans;
  int n;
  int ch;
  int i,j;

  nchans = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  if (dev)
    {
      dev_data = fts_dev_get_device_data(dev);

      /* return all zeros if not active or if write only or in the eof case */

      if (dev_data->active)
	{
	  fifo_sample_t *out;
	  int ret;

	  ret = fts_sample_fifo_want_to_put(dev_data->fifo, &out);
      
	  /* check ret; if ret is different from the block size,
	     the worker thread made a call to reader_eof; this means
	     that an i/o error occurred; we just stop reading/writing.
	     */

	  if (ret == MAXVS * nchans)
	    {
	      /* do the data transfer, transforming from interleaved to separate channels */

	      for (ch = 0; ch < nchans; ch++)
		{
		  float *in;

		  in = (float *) fts_word_get_ptr(argv + 3 + ch);

#ifdef HAVE_AF_VIRTUAL_PARAMETERS
		  for (i = ch, j = 0; j < n; i = i + nchans, j++)
		    out[i] = in[j];
#else
		  for (i = ch, j = 0; j < n; i = i + nchans, j++)
		    out[i] = (short) (in[j] * 32768.0f);
#endif
		}

	      /* Unlock the buffer */

	      fts_sample_fifo_putted(dev_data->fifo);
	    }

	  return;
	}
    }
}


/* WRITER THREAD: MUST started by an async call; will read the file
   file_block samples at a time; note that this value should be the same
   used to initialize the sample fifo.
*/

static void *fts_writesf_worker(void *data)
{
  int i;
  int ret;
  int eof = 0;
  AFfilehandle  file;		
  struct writesf_data *dev_data = (struct writesf_data *)data;
  fts_sample_fifo_t *fifo = dev_data->fifo;
  int frames_for_block = (dev_data->file_block / dev_data->nch);
  AFfilesetup setup;

  /* Actually Open the audio file  */

  setup = afNewFileSetup();

  afInitFileFormat(setup, fts_get_int(dev_data->format_descr));
  afInitRate(setup, AF_DEFAULT_TRACK, dev_data->sr);
  afInitChannels(setup, AF_DEFAULT_TRACK, dev_data->nch);

  file = afOpenFile(fts_symbol_name(dev_data->file_name), "w", setup);

  afFreeFileSetup(setup);

  if (file == AF_NULL_FILEHANDLE)
    {
      eof = 1;
      fts_sample_fifo_reader_eof(fifo);
    }
  else
    {
#ifdef HAVE_AF_VIRTUAL_PARAMETERS
      /* Set the virtual format of the file */

      afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
#endif
    }

  /*  LOOP: on the out of band status --> read the file -> Write to the sample fifo */

  while (! eof)
    {
      int ret;
      fifo_sample_t *p;

      ret = fts_sample_fifo_want_to_get(fifo, &p);

      afWriteFrames(file, AF_DEFAULT_TRACK, p, ret / dev_data->nch);

      if (ret != dev_data->file_block)
	eof = 1;

      fts_sample_fifo_got(fifo);
    }

  /* Close the file, free all the structures and exit the thread */

  fts_sample_fifo_reader_eof(fifo);

  if (file != AF_NULL_FILEHANDLE)
    afCloseFile(file);

  return NULL;
}

/* The function called async to create a reader thread */

static void fts_writesf_forker(void *data)
{
  int rc;
  struct writesf_data *dev_data = (struct writesf_data *)data;

  rc = pthread_create(&(dev_data->writer_thread), NULL, fts_writesf_worker, data);

  if (rc)
    fprintf(stderr, "fts_writesf_forker_init: return code from pthread_create() is %d\n", rc);
}


/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void pdtddev_init(void);

fts_module_t pdtddev_module = {"pdtddev", "Posix Direct-To-Disk devices", pdtddev_init, 0, 0, 0};

static void pdtddev_init( void)
{
  afSetErrorHandler(NULL);	/* avoid stupid error printing in the af library */

  fts_sample_fifo_init();
  fts_cmd_fifo_init();
  fts_async_init();

  pdtd_readsf_init();
  pdtd_writesf_init();
}







