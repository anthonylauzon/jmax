/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */


#include <pthread.h>
#include <dmedia/audiofile.h>
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

   */


typedef struct fts_sample_fifo
{
  float *buf;			/* the buffer actually holding the */
  int    size;			/* its total size */

  /* Reading */

  int    read_p;		/* the read pointer (index of the next sample to read) */
  int    read_block;		/* the size of the blocks being read, constant */
  int    read_pending;		/* != zero if a thread is blocked on reading */
  pthread_cond_t read_cond;	/* the read (fifo empty) semaphor */
  int    read_status;		/* the out of band data written by the writer thread */

  /* Writing */

  int    write_p;		/* the write pointer (index of the next sample to write) */
  int    write_block;		/* the size of the blocks being read, constant */
  int    write_pending;		/* != zero if a thread is blocked on writing */
  pthread_cond_t write_cond;	/* the write (fifo full) semaphor */
  int    write_status;		/* the out of band data written by the writer thread */

  /* Mutex */

  pthread_mutex_t mutex;	/* a mutex to lock the whole structure */

} fts_sample_fifo_t;


static void fts_sample_fifo_describe(const char *msg, fts_sample_fifo_t *fifo)
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

static fts_heap_t *sample_fifo_heap;

/* Create a new empty fifo, of a given size */

fts_sample_fifo_t *fts_sample_fifo_new(int size, int read_block, int write_block)
{
  fts_sample_fifo_t *sf= (fts_sample_fifo_t *) fts_heap_alloc(sample_fifo_heap);

  sf->buf = (float *) fts_malloc(sizeof(float) * size);
  sf->size = size;

  sf->read_p       = 0;
  sf->read_block   = read_block;
  sf->read_pending = 0;
  sf->read_status  = 0;
  pthread_cond_init(&(sf->read_cond), NULL);

  sf->write_p = 0;
  sf->write_block   = write_block;
  sf->write_pending = 0;
  sf->write_status  = 0;
  pthread_cond_init(&(sf->write_cond), NULL);

  pthread_mutex_init(&(sf->mutex), NULL);

  return sf;
}


/* Destroy must be called when the two parties already agreed to give up
   synchronization; calling this function with a thread blocked inside the fifo
   will cause impredictable effects */

void fts_sample_fifo_destroy(fts_sample_fifo_t *sf)
{
  pthread_cond_destroy(&(sf->read_cond));
  pthread_cond_destroy(&(sf->write_cond));
  pthread_mutex_destroy(&(sf->mutex));
  fts_free(sf->buf);
  fts_heap_free((char *) sf, sample_fifo_heap);
}

/* Ask for a pointer to a buffer ofsample to be read; size of the buffer
   is the declared read size at the sample fifo new time.
   return a pointer to the sample buffer; 
   the buffer is locked until the call to _got (see below) */

float *fts_sample_fifo_want_to_get(fts_sample_fifo_t *sf)
{
  pthread_mutex_lock(&(sf->mutex));

  if ((sf->write_p - sf->read_p + sf->size) % sf->size < sf->read_block)
    {
      sf->read_pending = 1;
      pthread_cond_wait(&(sf->read_cond), &(sf->mutex));
      sf->read_pending = 0;
    }

  pthread_mutex_unlock(&(sf->mutex));

  return sf->buf + sf->read_p;
}

/* Signal that the last get operation is completed, and the used buffer
   is empty; it may wake up a thread blocked in want_to_put if unlocking
   this buffer make the space available .
 */

void fts_sample_fifo_got(fts_sample_fifo_t *sf)
{
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
   the buffer is available; return a pointer to the buffer to be filled*/

float *fts_sample_fifo_want_to_put(fts_sample_fifo_t *sf)
{
  pthread_mutex_lock(&(sf->mutex));

  if ((sf->read_p != sf->write_p) && (sf->read_p - sf->write_p + sf->size) % sf->size  <= sf->write_block)
    {
      sf->write_pending = 1;
      pthread_cond_wait(&(sf->write_cond), &(sf->mutex));
      sf->write_pending = 0;
    }

  pthread_mutex_unlock(&(sf->mutex));

  return sf->buf + sf->write_p;
}


/* Signal that the last put operation is completed, and the used buffer
   is full; it may wake up a thread blocked in want_to_get if unlocking
   this buffer make the samples available .
 */

void fts_sample_fifo_putted(fts_sample_fifo_t *sf)
{
  pthread_mutex_lock(&(sf->mutex));

  sf->write_p = (sf->write_p + sf->write_block) % sf->size;

  if (sf->read_pending && ((sf->write_p - sf->read_p + sf->size) % sf->size >= sf->read_block))
    {
      pthread_cond_signal(&(sf->read_cond));
    }

  pthread_mutex_unlock(&(sf->mutex));
}

int fts_sample_fifo_get_read_block(fts_sample_fifo_t *sf)
{
  return sf->read_block;
}

int fts_sample_fifo_get_write_block(fts_sample_fifo_t *sf)
{
  return sf->write_block;
}


/* Put the out of band command; note that in a given application,
   either the reader or the writer should write the cmd, but not both,
   and the other should read it; also, the sample fifo give no provision
   of extra synchronization, if the command reader is currently blocked,
   it will read the command when waken up by the normal read/write operations */

void fts_sample_fifo_set_read_status(fts_sample_fifo_t *sf, int v)
{
  sf->read_status = v;
}

int fts_sample_fifo_get_read_status(fts_sample_fifo_t *sf)
{
  return sf->read_status;
}

void fts_sample_fifo_set_write_status(fts_sample_fifo_t *sf, int v)
{
  sf->write_status = v;
}

int fts_sample_fifo_get_write_status(fts_sample_fifo_t *sf)
{
  return sf->write_status;
}


void fts_sample_fifo_init()
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

fts_cmd_fifo_t *fts_cmd_fifo_new()
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

void fts_cmd_fifo_destroy(fts_cmd_fifo_t *cf)
{
  pthread_cond_destroy(&(cf->read_cond));
  pthread_cond_destroy(&(cf->write_cond));
  pthread_mutex_destroy(&(cf->mutex));
  fts_heap_free((char *) cf, cmd_fifo_heap);
}

/* Get a command, and execute it, outside of the lock */
  
void fts_cmd_fifo_exec_one(fts_cmd_fifo_t *cf)
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

void fts_cmd_fifo_add_command(fts_cmd_fifo_t *cf,  void (*fun)(void *), void *v)
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

void fts_cmd_fifo_init()
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

void fts_async_init()
{
  int rc;

  /* Build the command fifo */

  async_call_fifo = fts_cmd_fifo_new();

  /* Start the async command thread */

  rc = pthread_create(&async_call_thread, NULL, fts_async_sched, 0);

  if (rc)
    fprintf(stderr, "fts_async_init: return code from pthread_create() is %d\n", rc);
}


/* Simplification number one: there two device class,
   a read and a write one, called readsf and writesf.
   readsf will be implemented first, writesf later.
   */


static fts_status_t sgi_readsf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_readsf_close(fts_dev_t *dev);

static void sgi_readsf_get(fts_word_t *argv);

static fts_status_t sgi_readsf_activate(fts_dev_t *dev);
static fts_status_t sgi_readsf_deactivate(fts_dev_t *dev);
static int          sgi_readsf_get_nchans(fts_dev_t *dev);

static void fts_readsf_forker(void *data);

#define SFDEV_CLOSE 1
#define SFDEV_EOF   2

static void sgi_readsf_init(void)
{
  fts_dev_class_t *sgi_readsf_class;

  /* dac file */

  sgi_readsf_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("readsf"));

  /* Installation of all the device class functions */

  fts_dev_class_set_open_fun(sgi_readsf_class, sgi_readsf_open);
  fts_dev_class_set_close_fun(sgi_readsf_class, sgi_readsf_close);

  fts_dev_class_sig_set_get_fun(sgi_readsf_class, sgi_readsf_get);

  fts_dev_class_sig_set_activate_fun(sgi_readsf_class, sgi_readsf_activate);
  fts_dev_class_sig_set_deactivate_fun(sgi_readsf_class, sgi_readsf_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(sgi_readsf_class, sgi_readsf_get_nchans);
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
  int eof;
};


static void readsf_data_describe(const char *msg, struct readsf_data *data)
{
  fprintf(stderr, "%s: DATA %lx\n", msg, (unsigned int) data);
  fprintf(stderr, "\tfifo %lx\n", (unsigned int) data->fifo);
  fprintf(stderr, "\tfile_block %d\n", data->file_block);
  fprintf(stderr, "\tfifo_size_p %d\n", data->fifo_size);
  fprintf(stderr, "\tfile_name %s\n", fts_symbol_name(data->file_name));
  fprintf(stderr, "\tnch %d\n", data->nch);
  fprintf(stderr, "\tactive %d\n", data->active);
  fprintf(stderr, "\teof %d\n", data->eof);
}


static fts_status_t
sgi_readsf_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct readsf_data *dev_data;

  if ((nargs < 1) || (! fts_is_symbol(&args[0])))
    return &fts_dev_open_error;

  /* make the structure */

  dev_data = (struct readsf_data *) fts_malloc(sizeof(struct readsf_data));
  fts_dev_set_device_data(dev, dev_data);

  /* get the file name */

  dev_data->file_name = fts_get_symbol(&args[0]);
  dev_data->eof = 0;
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

  /*  Start the reader thread  using a async call*/

  fts_async_call(fts_readsf_forker, (void *)dev_data);

  return fts_Success;
}

static fts_status_t
sgi_readsf_close(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  /* Just tell the worker thread to close and destroy everything */

  fts_sample_fifo_set_read_status(dev_data->fifo, SFDEV_CLOSE);

  return fts_Success;
}


static fts_status_t sgi_readsf_activate(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 1;

  return fts_Success;
}


static fts_status_t sgi_readsf_deactivate(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 0;

  return fts_Success;
}

static int
sgi_readsf_get_nchans(fts_dev_t *dev)
{
  struct readsf_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  return dev_data->nch;
}

/* If the file is finished, we probabily just crash ... @@@ MUST HANDLE EOF */
/* Probabily, we should get optimized version for mono and stereo files (??) */

static void
sgi_readsf_get(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  struct readsf_data *dev_data;
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

      if (dev_data->active && (! dev_data->eof))
	{
	  float *in;
	  int ret;

	  in = fts_sample_fifo_want_to_get(dev_data->fifo);
      
	  /* do the data transfer, transforming from interleaved to separate channels */

	  for (ch = 0; ch < nchans; ch++)
	    {
	      float *out;

	      out = (float *) fts_word_get_ptr(argv + 3 + ch);

	      for (i = ch, j = 0; j < n; i = i + nchans, j++)
		out[j] = in[i];
	    }

	  /* Unlock the buffer */

	  fts_sample_fifo_got(dev_data->fifo);

	  return;
	}
    }

  /* If there is a null device, or if the device is not active,
     just put zeros */

  for (ch = 0; ch < nchans; ch++)
    {
      float *out;

      out = (float *) fts_word_get_ptr(argv + 3 + ch);	  

      for(i = 0; i < n; i++)
	out[i] = 0.0f;
    }
}


/* READER THREAD: MUST started by an async call; will read the file
   file_block samples at a time; note that this value should be the same
   used to initialize the sample fifo.
*/

/* @@@@@ should add check on the results !!!, and put the result on status in case of problems ??? */

/* @@@ Must handle the eof case: read a partial block, then fill with
   zeros what left, and signal eof to the others, waiting for a close signal ?
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
      fts_sample_fifo_set_write_status(fifo, SFDEV_EOF);
    }
  else
    {
      /* Set the number of virtual channels */

      afSetVirtualChannels(file, AF_DEFAULT_TRACK, dev_data->nch);

      /* Set the virtual format of the file */

      afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
    }

  /*  LOOP: on the out of band status --> read the file -> Write to the sample fifo */

  while (fts_sample_fifo_get_read_status(fifo) != SFDEV_CLOSE)
    {
      float *p;

      p = fts_sample_fifo_want_to_put(fifo);

      if (eof)
	{
	  /* Just fill with zeros */

	  for (i = 0; i < dev_data->file_block; i++)
	    p[i] = 0.0f;
	}
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
	      fts_sample_fifo_set_write_status(fifo, SFDEV_EOF);
	    }
	}

      fts_sample_fifo_putted(fifo);
    }

  /* Close the file, free all the structures and exit the thread */

  afCloseFile(file);
  
  fts_sample_fifo_destroy(fifo);
  fts_free(dev_data);

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


/* File Init */

static void test_init(void);	/* @@@ */

void sfdev_init(void)
{
  fts_sample_fifo_init();
  fts_cmd_fifo_init();
  fts_async_init();
  sgi_readsf_init();
}







