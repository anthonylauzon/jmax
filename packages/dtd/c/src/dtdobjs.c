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
 * this file's authors: francois dechelle.
 */


#include <math.h>
#include <string.h>

#include <fts/fts.h>
#include "dtd.h"
#include "task_manager.h"
#include "thread.h"


/* ********************************************************************** */
/*                                                                        */
/* globals                                                                */
/*                                                                        */
/* ********************************************************************** */

#define READ_BUFFER_SIZE 22050
#define WRITE_BUFFER_SIZE 22050

static fts_symbol_t s_writesf;
static fts_symbol_t s_readsf;
static fts_symbol_t s_open;
static fts_symbol_t s_close;
static fts_symbol_t s_play;
static fts_symbol_t s_record;
static fts_symbol_t s_pause;
static fts_symbol_t s_do_open;
static fts_symbol_t s_do_close;
static fts_symbol_t s_do_read;
static fts_symbol_t s_do_write;

fts_task_manager_t* manager = NULL;


/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* readsf~                                                                */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

typedef enum { 
  readsf_closed, 
  readsf_opened, 
  readsf_playing, 
  readsf_paused
} readsf_state_t;


typedef struct {
  fts_object_t _o;
  readsf_state_t state;
  fts_symbol_t filename;
  fts_audiofile_t* audiofile;
  int n_channels;
  float** buf[2];         /* the local buffer sets */
  int available[2];       /* the number of avilable samples in the buffer sets */
  int bufsize;            /* size of the dsp buffers */
  int curbuf;             /* which of the two buffer sets is used in the dsp */
  int offset;             /* the current offset in the buffer */
  unsigned int frames;    /* the number of frames that have been played */
  fts_mutex_t* mutex;
} readsf_t;

static fts_status_t readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
static void readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_do_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_do_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_do_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_change_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void readsf_dsp( fts_word_t *argv);
static void readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);


/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* writesf~                                                                */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

typedef enum { 
  writesf_closed, 
  writesf_opened, 
  writesf_recording, 
  writesf_paused
} writesf_state_t;


typedef struct {
  fts_object_t _o;
  writesf_state_t state;
  fts_symbol_t filename;
  fts_audiofile_t* audiofile;
  int n_channels;
  fts_symbol_t format;
  float** buf[2];         /* the local buffer sets */
  int available[2];       /* the number of avilable samples in the buffer sets */
  int bufsize;            /* size of the dsp buffers */
  int curbuf;             /* which of the two buffer sets is used in the dsp */
  int offset;             /* the current offset in the buffer */
  unsigned int frames;    /* the number of frames that have been written */
  fts_mutex_t* mutex;
} writesf_t;

static fts_status_t writesf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
static void writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_do_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_do_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_do_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_change_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void writesf_dsp( fts_word_t *argv);
static void writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);



/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* Module initialization                                                  */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

void dtdobjs_config( void)
{
  manager = fts_task_manager_new(8);

  s_readsf = fts_new_symbol( "readsf~");
  s_writesf = fts_new_symbol( "writesf~");

  fts_metaclass_install(s_readsf, readsf_instantiate, fts_first_arg_equiv);
  fts_metaclass_install(s_writesf, writesf_instantiate, fts_first_arg_equiv);

  s_open = fts_new_symbol( "open");
  s_close = fts_new_symbol( "close");
  s_play = fts_new_symbol( "play");
  s_record = fts_new_symbol( "record");
  s_pause = fts_new_symbol( "pause");
  s_do_open = fts_new_symbol( "do_open");
  s_do_close = fts_new_symbol( "do_close");
  s_do_read = fts_new_symbol( "do_read");
  s_do_write = fts_new_symbol( "do_write");
}



/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* readsf~ class                                                          */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

static fts_status_t 
readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels, i;

  n_channels = fts_get_int_arg(ac, at, 1, 1);

  if (n_channels < 1) {
    n_channels = 1;
  }

  fts_class_init(cl, sizeof(readsf_t), 1, n_channels + 1, 0);

  /* constructor/deconstructor */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, readsf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, readsf_delete);

  /* methods to change the state */
  fts_method_define_varargs(cl, 0, s_open, readsf_change_state);
  fts_method_define_varargs( cl, 0, s_play, readsf_change_state);
  fts_method_define_varargs( cl, 0, s_pause, readsf_change_state);
  fts_method_define_varargs( cl, 0, s_close, readsf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_stop, readsf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_start, readsf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_bang, readsf_change_state);

  /* callbacks for background processing */
  fts_method_define_varargs( cl, 0, s_do_open, readsf_do_open);
  fts_method_define_varargs( cl, 0, s_do_read, readsf_do_read);
  fts_method_define_varargs( cl, 0, s_do_close, readsf_do_close);

  /* dsp interface */
  for (i = 0; i < n_channels; i++)
    fts_dsp_declare_outlet(cl, i);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, readsf_put);
  fts_dsp_declare_function( s_readsf, readsf_dsp);

  return fts_Success;
}


/* ********************************************************************** */
/*                                                                        */
/* readsf~ constructor/deconstructor                                      */
/*                                                                        */
/* ********************************************************************** */

static void 
readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n_channels;
  int i, n;

  ac--;
  at++;

  this->filename = 0;
  this->audiofile = NULL;
  this->state = readsf_closed;
  this->buf[0] = NULL;
  this->buf[1] = NULL;
  this->bufsize = READ_BUFFER_SIZE;
  this->curbuf = 0; 
  this->offset = 0;
  this->available[0] = 0;
  this->available[1] = 0;
  this->mutex = fts_mutex_new();

  n_channels = fts_get_int_arg(ac, at, 0, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  if ((ac == 2) && fts_is_symbol(at + 1)) {
    this->filename = fts_get_symbol( at + 1);
  }

  for (i = 0; i < 2; i++) {
    this->buf[i] = (float**) fts_malloc(this->n_channels * sizeof(float*));
    for (n = 0; n < n_channels; n++) {
      this->buf[i][n] = (float*) fts_malloc(this->bufsize * sizeof(float));
    }
  }

  fts_dsp_add_object(o);
}

static void 
readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i, n;
  readsf_t *this = (readsf_t *)o;

  readsf_do_close(o, 0, NULL, 0, NULL);
  
  for (i = 0; i < 2; i++) {
    if (this->buf[i] != NULL) {
      for (n = 0; n < this->n_channels; n++) {
	if (this->buf[i][n] != NULL) {
	  fts_free(this->buf[i][n]);
	}
      }
      fts_free(this->buf[i]);
    }
  }

  fts_mutex_delete(this->mutex);

  fts_dsp_remove_object(o);
}


/* ********************************************************************** */
/*                                                                        */
/* readsf~ methods for background processing                              */
/*                                                                        */
/* ********************************************************************** */

static void 
readsf_do_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int i;
  int next_state;

  fts_mutex_lock(this->mutex);

  if (this->audiofile) {
    fts_audiofile_delete(this->audiofile);
    this->audiofile = NULL;
  }

  next_state = (ac > 1)? fts_get_int(at) : readsf_opened;

  if ((ac > 1) && fts_is_symbol(at + 1)) {
    this->filename = fts_get_symbol(at + 1);
  }

  if (this->filename) {
    
    this->audiofile = fts_audiofile_open_read(this->filename);
    
    if (!fts_audiofile_valid(this->audiofile)) {
      post( "readsf~: error: failed to open the audio file\n");
      fts_object_set_error(o, "failed to open the audio file");
      goto error_recovery;
    }

    this->curbuf = 0; 
    this->offset = 0;
    this->frames = 0;

    /* pre-fill both buffer */
    for (i = 0; i < 2; i++) {

      this->available[i] = fts_audiofile_read(this->audiofile, this->buf[i], this->n_channels, this->bufsize);

      if (this->available[i] < 0) {
	goto error_recovery;
      }
    }

    /* switch to the new state */
    this->state = next_state;

  } else {
    this->state = readsf_closed; 
    post( "readsf~: error: no file name specified\n");
  }

  fts_mutex_unlock(this->mutex);
  return;

 error_recovery:

  fts_audiofile_delete(this->audiofile);
  this->audiofile = NULL;
  this->state = readsf_closed;
  
  fts_mutex_unlock(this->mutex);
  return;
}

static void 
readsf_do_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  fts_mutex_lock(this->mutex);

  if (this->audiofile) {
    fts_audiofile_delete(this->audiofile);
    this->audiofile = NULL;
  }

  this->state = readsf_closed;

  fts_mutex_unlock(this->mutex);
}

static void 
readsf_do_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int nextbuf;

  fts_mutex_lock(this->mutex);

  if (this->state == readsf_closed) {
    return;
  }

  nextbuf = 1 - this->curbuf;
  
  this->available[nextbuf] = fts_audiofile_read(this->audiofile, this->buf[nextbuf], 
						this->n_channels, this->bufsize);
  
  if (this->available[nextbuf] < 0) {
    /* FIXME: close, do nothing ??? */
  } else if (this->available[nextbuf] < this->bufsize) {
    /* FIXME: close, loop, reposition at beginning, do nothing ??? */
  }

  fts_mutex_unlock(this->mutex);
}


/* ********************************************************************** */
/*                                                                        */
/* readsf~ control state methods                                          */
/*                                                                        */
/* ********************************************************************** */

static void 
readsf_change_state(fts_object_t *o, int winlet, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_atom_t a[2];

  switch( this->state) {
  case readsf_closed:
    if (message == s_open)
      {
	fts_set_int(a, readsf_opened);
	if ((ac == 1) && fts_is_symbol(at)) {
	  a[1] = at[0];	  
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 2, a);
	} else {
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 1, a);
	}
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
      }
    else if ((message == s_play) || (message == fts_s_start) || (message == fts_s_bang))
      {
	fts_set_int(a, readsf_playing);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    else if (message == s_pause)
      {
	fts_set_int(a, readsf_paused);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    break;

  case readsf_opened:
    if (message == s_open)
      {
	fts_set_int(a, readsf_opened);
	if ((ac == 1) && fts_is_symbol(at)) {
	  a[1] = at[0];	  
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 2, a);
	}
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_play) || (message == fts_s_start) || (message == fts_s_bang))
      {
	this->state = readsf_playing;
      }
    else if (message == s_pause)
      {
	this->state = readsf_paused;
      }
    break;

  case readsf_playing:
    if (message == s_open)
      {
	/* switch to the paused state to avoid that the buffers are
           read while they are being refilled */
	this->state = readsf_paused;
	fts_set_int(a, readsf_playing);
	if ((ac == 1) && fts_is_symbol(at)) {
	  a[1] = at[0];	  
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 2, a);
	} else {
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 1, a);
	}
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_play) || (message == fts_s_start) || (message == fts_s_bang))
      {
      }
    else if (message == s_pause)
      {
	this->state = readsf_paused;
      }
    break;

  case readsf_paused:
    if (message == s_open)
      {
	fts_set_int(a, readsf_opened);
	if ((ac == 1) && fts_is_symbol(at)) {
	  a[1] = at[0];	  
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 2, a);
	} else {
	  fts_task_manager_add_task(manager, o, 0, s_do_open, 1, a);
	}
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_play) || (message == fts_s_start) || (message == fts_s_bang))
      {
	this->state = readsf_playing;
      }
    else if (message == s_pause)
      {
      }
    break;

  default:
    break;
  }
}

/* ********************************************************************** */
/*                                                                        */
/* readsf~ dsp methods                                                    */
/*                                                                        */
/* ********************************************************************** */

static void 
readsf_dsp( fts_word_t *argv)
{
  readsf_t *this;
  int n, n_channels;
  fts_word_t *outputs;
  float *out;
  float* buf;
  int curbuf;
  int n1, n2, i, count, ch;

  this = (readsf_t *)fts_word_get_ptr( argv + 0);
  n = fts_word_get_int( argv + 1);
  outputs = argv + 2;
  n_channels = this->n_channels;
  curbuf = this->curbuf;

  if (this->state != readsf_playing) {

    /* clean the buffers */
    for (ch = 0; ch < n_channels; ch++) {
      out = (float*) fts_word_get_ptr(outputs + ch);
      for (count = 0; count < n; count++) {
	out[count] = 0.0f;
      }
    }
    return;
  } 

  if (n + this->offset <= this->bufsize) {
    n1 = n;   /* n1 is the number of samples we read from the current buffer */
    n2 = 0;   /* n2 is the number of samples we read from the next buffer */
  } else {
    n1 = this->bufsize - this->offset;
    n2 = n - n1;
  }

  /* read the sample we have in the current buffer */
  for (ch = 0; ch < n_channels; ch++) {

    out = (float*) fts_word_get_ptr(outputs + ch);
    buf = this->buf[curbuf][ch];

    for (i = this->offset, count = 0; count < n1; i++, count++) {
      out[count] = buf[i];
    }
  }

  this->offset += n1;

  /* if the end of the current buffer is reached, swap the buffers and request
     a refill */
  if (this->offset >= this->bufsize) {
    this->available[curbuf] = 0;
    this->curbuf = 1 - this->curbuf;
    curbuf = this->curbuf;
    this->offset = 0;
    fts_task_manager_add_task(manager, (fts_object_t*) this, 0, s_do_read, 0, NULL);
  }

  /* if we swapped buffers, check if we need to read any samples from
     the new buffer */
  if (n2 > 0) {
    for (ch = 0; ch < n_channels; ch++) {
      
      out = (float*) fts_word_get_ptr(outputs + ch);
      buf = this->buf[curbuf][ch];
      
      for (i = this->offset, count = n1; count < n; i++, count++) {
	out[count] = buf[i];
      }
    }

    this->offset += n2;
  }

  this->frames += n;

  if (this->frames > fts_audiofile_get_num_frames(this->audiofile)) { 
    /* we're done. set the state to pause to avoid that the dsp is run
       once more before the object switches to the closed state. */
    this->state = readsf_paused;
    fts_outlet_bang((fts_object_t*) this, n_channels);
    fts_task_manager_add_task(manager, (fts_object_t*) this, 0, s_do_close, 0, NULL);
  }
}

static void 
readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[50];
  int i;

  fts_set_ptr( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_output_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_output_name( dsp, i));

  fts_dsp_add_function( s_readsf, 2 + this->n_channels, argv);
}



/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* writesf~ class                                                          */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

static fts_status_t 
writesf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels, i;

  n_channels = fts_get_int_arg(ac, at, 1, 1);

  if (n_channels < 1) {
    n_channels = 1;
  }

  fts_class_init(cl, sizeof(writesf_t), n_channels, 0, 0);

  /* constructor/deconstructor */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, writesf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, writesf_delete);

  /* methods to change the state */
  fts_method_define_varargs(cl, 0, s_open, writesf_change_state);
  fts_method_define_varargs( cl, 0, s_record, writesf_change_state);
  fts_method_define_varargs( cl, 0, s_pause, writesf_change_state);
  fts_method_define_varargs( cl, 0, s_close, writesf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_stop, writesf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_start, writesf_change_state);
  fts_method_define_varargs( cl, 0, fts_s_bang, writesf_change_state);

  /* callbacks for background processing */
  fts_method_define_varargs( cl, 0, s_do_open, writesf_do_open);
  fts_method_define_varargs( cl, 0, s_do_write, writesf_do_write);
  fts_method_define_varargs( cl, 0, s_do_close, writesf_do_close);

  /* dsp interface */
  for (i = 0; i < n_channels; i++)
    fts_dsp_declare_inlet(cl, i);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, writesf_put);
  fts_dsp_declare_function( s_writesf, writesf_dsp);

  return fts_Success;
}


/* ********************************************************************** */
/*                                                                        */
/* writesf~ constructor/deconstructor                                      */
/*                                                                        */
/* ********************************************************************** */

static void 
writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int n_channels;
  int i, n;

  ac--;
  at++;

  this->filename = 0;
  this->format = NULL;
  this->audiofile = NULL;
  this->state = writesf_closed;
  this->buf[0] = NULL;
  this->buf[1] = NULL;
  this->bufsize = WRITE_BUFFER_SIZE;
  this->curbuf = 0; 
  this->offset = 0;
  this->available[0] = 0;
  this->available[1] = 0;
  this->mutex = fts_mutex_new();

  n_channels = fts_get_int_arg(ac, at, 0, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  if ((ac >= 2) && fts_is_symbol( at + 1)) {
    this->filename = fts_get_symbol( at + 1);
  }

  if ((ac >= 3) && fts_is_symbol( at + 2)) {
    this->format = fts_get_symbol( at + 2);
  } else {
    this->format = fts_s_16bits;
  }

  for (i = 0; i < 2; i++) {
    this->buf[i] = (float**) fts_malloc(this->n_channels * sizeof(float*));
    
    fts_log("buf[%i]=0x%p\n", i, this->buf[i]);

    for (n = 0; n < n_channels; n++) {
      this->buf[i][n] = (float*) fts_malloc(this->bufsize * sizeof(float));

      fts_log("buf[%i][%i]=0x%p\n", i, n, this->buf[i][n]);
    }
  }

  fts_dsp_add_object(o);
}

static void 
writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i, n;
  writesf_t *this = (writesf_t *)o;

  writesf_do_close(o, 0, NULL, 0, NULL);
  
  for (i = 0; i < 2; i++) {
    if (this->buf[i] != NULL) {
      for (n = 0; n < this->n_channels; n++) {
	if (this->buf[i][n] != NULL) {
	  fts_free(this->buf[i][n]);
	}
      }
      fts_free(this->buf[i]);
    }
  }

  fts_mutex_delete(this->mutex);

  fts_dsp_remove_object(o);
}


/* ********************************************************************** */
/*                                                                        */
/* writesf~ methods for background processing                              */
/*                                                                        */
/* ********************************************************************** */

static void 
writesf_do_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  fts_mutex_lock(this->mutex);

  if (this->audiofile) {
    fts_audiofile_delete(this->audiofile);
    this->audiofile = NULL;
  }

  if (this->filename) {
    
    this->audiofile = fts_audiofile_open_write(this->filename, 
					       (int) fts_dsp_get_sample_rate(),       /* FIXME */
					       this->n_channels, this->format);
    
    if ((this->audiofile == NULL) 
	|| (fts_object_get_error((fts_object_t*) this->audiofile) != NULL)) {
      post( "writesf~: error: failed to open the audio file\n");
      fts_object_set_error(o, "failed to open the audio file");
      goto error_recovery;
    }

    this->curbuf = 0; 
    this->offset = 0;
    this->frames = 0;

    /* switch to the new state */
    if (ac > 0) {
      switch (fts_get_int(at)) {
      case writesf_opened:
	this->state = writesf_opened;
	break;
	
      case writesf_recording:
	this->state = writesf_recording;
	break;
	
      case writesf_paused:
	this->state = writesf_paused;
	break;
	
      case writesf_closed:
	this->state = writesf_opened;      /* FIXME */
	break;
      }
    }

  } else {
    this->state = writesf_closed; 
    post( "writesf~: error: no file name specified\n");
  }

  fts_mutex_unlock(this->mutex);
  return;

 error_recovery:

  fts_audiofile_delete(this->audiofile);
  this->audiofile = NULL;
  this->state = writesf_closed;
  
  fts_mutex_unlock(this->mutex);
  return;
}

static void 
writesf_do_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  fts_mutex_lock(this->mutex);

  if (this->audiofile) {
    fts_audiofile_delete(this->audiofile);
    this->audiofile = NULL;
  }

  this->state = writesf_closed;

  fts_mutex_unlock(this->mutex);
}

static void 
writesf_do_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int prevbuf, count;

  fts_mutex_lock(this->mutex);

  if (this->state == writesf_closed) {
    return;
  }

  prevbuf = 1 - this->curbuf;
  
  count = fts_audiofile_write(this->audiofile, this->buf[prevbuf], 
			      this->n_channels, this->available[prevbuf]);
  
  if (count < 0) {
    fts_audiofile_delete(this->audiofile);
    this->audiofile = NULL;
    this->state = writesf_closed;

  } else if (count < this->available[prevbuf]) {
    /* FIXME: close or ignore ??? */
  }

  fts_mutex_unlock(this->mutex);
}


/* ********************************************************************** */
/*                                                                        */
/* writesf~ control state methods                                          */
/*                                                                        */
/* ********************************************************************** */

static void 
writesf_change_state(fts_object_t *o, int winlet, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_atom_t a[1];

  switch( this->state) {
  case writesf_closed:
    if (message == s_open)
      {
	fts_set_int(a, writesf_opened);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
      }
    else if ((message == s_record) || (message == fts_s_start) || (message == fts_s_bang))
      {
	fts_set_int(a, writesf_recording);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    else if (message == s_pause)
      {
	fts_set_int(a, writesf_paused);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    break;

  case writesf_opened:
    if (message == s_open)
      {
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_record) || (message == fts_s_start) || (message == fts_s_bang))
      {
	this->state = writesf_recording;
      }
    else if (message == s_pause)
      {
	this->state = writesf_paused;
      }
    break;

  case writesf_recording:
    if (message == s_open)
      {
	/* switch to the paused state to avoid that the buffers are
           written do disk while the file is opened */
	this->state = writesf_paused;
	fts_set_int(a, writesf_recording);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_record) || (message == fts_s_start) || (message == fts_s_bang))
      {
      }
    else if (message == s_pause)
      {
	this->state = writesf_paused;
      }
    break;

  case writesf_paused:
    if (message == s_open)
      {
	fts_set_int(a, writesf_opened);
	fts_task_manager_add_task(manager, o, 0, s_do_open, 1, &a[0]);
      }
    else if ((message == s_close) || (message == fts_s_stop))
      {
	fts_task_manager_add_task(manager, o, 0, s_do_close, 0, NULL);
      }
    else if ((message == s_record) || (message == fts_s_start) || (message == fts_s_bang))
      {
	this->state = writesf_recording;
      }
    else if (message == s_pause)
      {
      }
    break;

  default:
    break;
  }
}

/* ********************************************************************** */
/*                                                                        */
/* writesf~ dsp methods                                                    */
/*                                                                        */
/* ********************************************************************** */

static void 
writesf_dsp( fts_word_t *argv)
{
  writesf_t *this;
  int n, n_channels;
  fts_word_t *outputs;
  float *out;
  float* buf;
  int curbuf;
  int n1, n2, i, count, ch;

  this = (writesf_t *)fts_word_get_ptr( argv + 0);
  n = fts_word_get_int( argv + 1);
  outputs = argv + 2;
  n_channels = this->n_channels;
  curbuf = this->curbuf;

  if (this->state != writesf_recording) {
    return;
  } 

  if (n + this->offset <= this->bufsize) {
    n1 = n;   /* n1 is the number of samples we write to the current buffer */
    n2 = 0;   /* n2 is the number of samples we write to the next buffer */
  } else {
    n1 = this->bufsize - this->offset;
    n2 = n - n1;
  }

  /* write the samples to the current buffer */
  for (ch = 0; ch < n_channels; ch++) {

    out = (float*) fts_word_get_ptr(outputs + ch);
    buf = this->buf[curbuf][ch];

    for (i = this->offset, count = 0; count < n1; i++, count++) {
      buf[i] = out[count];
    }
  }

  this->offset += n1;

  /* if the end of the current buffer is reached, swap the buffers and request
     a write-to-disk */
  if (this->offset >= this->bufsize) {
    this->available[curbuf] = this->offset;
    this->curbuf = 1 - this->curbuf;
    curbuf = this->curbuf;
    this->offset = 0;
    this->available[curbuf] = 0;
    fts_task_manager_add_task(manager, (fts_object_t*) this, 0, s_do_write, 0, NULL);
  }

  /* if we swapped buffers, check if we need to write any samples to
     the new buffer */
  if (n2 > 0) {
    for (ch = 0; ch < n_channels; ch++) {
      
      out = (float*) fts_word_get_ptr(outputs + ch);
      buf = this->buf[curbuf][ch];
      
      for (i = this->offset, count = n1; count < n; i++, count++) {
	buf[i] = out[count];
      }
    }

    this->offset += n2;
  }

  this->frames += n;
}

static void 
writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[50];
  int i;

  fts_set_ptr( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_input_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_input_name( dsp, i));

  fts_dsp_add_function( s_writesf, 2 + this->n_channels, argv);
}
