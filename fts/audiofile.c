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

#include "ftsconfig.h"
#include <fts/fts.h>

#define fts_audiofile_loader_open_write(_f)      (*fts_audiofile_loader->open_write)(_f)
#define fts_audiofile_loader_open_read(_f)       (*fts_audiofile_loader->open_read)(_f)

static fts_status_t fts_audiofile_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
static void fts_audiofile_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void fts_audiofile_destroy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/* FIXME: TEST_READ */
/*  #define TEST_READ 1 */
/*  #define NUM_CHAN  16 */
#if TEST_READ
static void fts_audiofile_put_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void fts_audiofile_ftl_read(fts_word_t *argv);
#elif TEST_WRITE
static void fts_audiofile_put_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void fts_audiofile_ftl_write(fts_word_t *argv);
#endif

/*******************************************************
 *
 * Global variables
 *
 */

fts_audiofile_loader_t* fts_audiofile_loader = NULL;
static fts_class_t *fts_audiofile_class = NULL;

/*******************************************************
 *
 * Module configuration
 *
 */

void 
fts_kernel_audiofile_init(void)
{
  fts_s_audiofile = fts_new_symbol("audiofile");
  fts_metaclass_install(fts_s_audiofile, fts_audiofile_instantiate, fts_always_equiv);
  fts_audiofile_class = fts_class_get_by_name(fts_s_audiofile);
}

/*******************************************************
 *
 * Audio file loader
 *
 */

int 
fts_audiofile_set_loader(char* name, fts_audiofile_loader_t* loader)
{
  fts_audiofile_loader = loader;
  fts_log("[audiofile] Setting audio file loader to %s\n", name);
  return 0;
}

/*******************************************************
 *
 * Audio file class
 *
 */

static fts_status_t
fts_audiofile_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

#ifdef TEST_READ
  fts_class_init(cl, sizeof(fts_audiofile_t), 0, NUM_CHAN, 0);
#elif TEST_WRITE
  fts_class_init(cl, sizeof(fts_audiofile_t), NUM_CHAN, 0, 0);
#else
  fts_class_init(cl, sizeof(fts_audiofile_t), 0, 0, 0);
#endif

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_audiofile_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_audiofile_destroy);

#ifdef TEST_READ
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, fts_audiofile_put_read);
  fts_dsp_declare_function(fts_s_audiofile, fts_audiofile_ftl_read);
  for (i = 0; i < NUM_CHAN; i++) {
    fts_dsp_declare_outlet(cl, i);
  }
#elif TEST_WRITE
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, fts_audiofile_put_write);
  fts_dsp_declare_function(fts_s_audiofile, fts_audiofile_ftl_write);
  for (i = 0; i < NUM_CHAN; i++) {
    fts_dsp_declare_inlet(cl, i);
  }
#endif

  return fts_Success;
}


/*******************************************************
 *
 * Audio file constructor/deconstructor
 *
 */

static void 
fts_audiofile_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audiofile_t* aufile = (fts_audiofile_t *)o;

  ac--;
  at++;

  if (fts_audiofile_loader == NULL) {
    fts_object_set_error(o, "No audio file loader has been installed");
    return;
  }

#if defined(TEST_READ) || defined(TEST_WRITE)
  /* FIXME: if the audiofile object is an error object (e.g. because
     file not found), it isn't destroyed when the init params are
     changed (e.g. filename has been edited). instead it is
     initialized twice and added to the dsp twice!!! */
/*    fts_dsp_add_object(o); */
#endif

  aufile->filename = NULL;
  aufile->sample_rate = 0;
  aufile->channels = 0;
  aufile->format = 0;
  aufile->handle = NULL;

  if (ac == 1) {

    /* open a new audio file for reading */

    if (!fts_is_symbol(at)) { 
      fts_object_set_error(o, "First argument should be a filename");
      return;
    }

    aufile->filename = fts_get_symbol(at);
    
    if (fts_audiofile_loader_open_read(aufile) != 0) {
      fts_object_set_error(o, fts_audiofile_error(aufile)); 
      return;
    }

#if defined(TEST_READ)
    fts_dsp_add_object(o);
#endif
    
  } else if (ac == 4) {

    /* open a new audio file for writing */

    if (!fts_is_symbol(at)) { 
      fts_object_set_error(o, "First argument should be a filename");
      return;
    }
    if (!fts_is_int(at + 1)) { 
      fts_object_set_error(o, "Second argument should be the sample rate as integer");
      return;
    }
    if (!fts_is_int(at + 2)) { 
      fts_object_set_error(o, "Third argument should be the number of channels as integer");
      return;
    }
    if (!fts_is_symbol(at + 3)) { 
      fts_object_set_error(o, "Fourth argument should be the sample format");
      return;
    }

    aufile->filename = fts_get_symbol(at); 
    aufile->sample_rate = fts_get_int(at + 1);
    aufile->channels = fts_get_int(at + 2);
    aufile->format = fts_get_symbol(at + 3);

    if (fts_audiofile_loader_open_write(aufile) != 0) {
      fts_object_set_error(o, fts_audiofile_error(aufile));      
      return;
    }

#if defined(TEST_WRITE)
    fts_dsp_add_object(o);
#endif

  } else {
    fts_object_set_error(o, "Wrong number of arguments");
  }
}

static void 
fts_audiofile_destroy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audiofile_t* aufile = (fts_audiofile_t *)o;
  fts_audiofile_close(aufile);

#if defined(TEST_READ) || defined(TEST_WRITE)
  fts_dsp_remove_object(o);
#endif
}

/*******************************************************
 *
 * Audio file methods
 *
 */
fts_audiofile_t* 
fts_audiofile_open_write(char* filename, int sample_rate, int channels, char* sample_format)
{
  fts_atom_t a[4];

  fts_set_symbol(&a[0], fts_new_symbol(filename));
  fts_set_int(&a[1], sample_rate);
  fts_set_int(&a[2], channels);
  fts_set_symbol(&a[3], fts_new_symbol(sample_format));

  return (fts_audiofile_t *) fts_object_create(fts_audiofile_class, 4, a);
}

fts_audiofile_t* 
fts_audiofile_open_read(char* filename)
{
  fts_atom_t a[1];
  fts_set_symbol(&a[0], fts_new_symbol(filename));
  return (fts_audiofile_t *) fts_object_create(fts_audiofile_class, 1, a);
}

void 
fts_audiofile_delete(fts_audiofile_t* aufile)
{
  fts_object_destroy( (fts_object_t*) aufile);
}

#if TEST_READ

void
fts_audiofile_put_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t a[2 + NUM_CHAN];
  int i;

  fts_set_ptr(a + 0, o);
  fts_set_int(a + 1, fts_dsp_get_output_size(dsp, 0));

  for (i = 0; i < NUM_CHAN; i++) {
    fts_set_symbol(a + 2 + i, fts_dsp_get_output_name(dsp, i));
  }
  
  fts_dsp_add_function(fts_s_audiofile, 2 + NUM_CHAN, a);
}

void
fts_audiofile_put_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t a[2 + NUM_CHAN];
  int i;

  fts_set_ptr(a + 0, o);
  fts_set_int(a + 1, fts_dsp_get_input_size(dsp, 0));

  for (i = 0; i < NUM_CHAN; i++) {
    fts_set_symbol(a + 2 + i, fts_dsp_get_input_name(dsp, i));
  }
  
  fts_dsp_add_function(fts_s_audiofile, 2 + NUM_CHAN, a);
}

#endif


#if TEST_WRITE

static void
fts_audiofile_ftl_read(fts_word_t *argv)
{
  fts_audiofile_t *this = (fts_audiofile_t *)fts_word_get_ptr(argv + 0);
  int n = fts_word_get_int(argv + 1);
  float* buf[NUM_CHAN];
  int i;

  for (i = 0; i < NUM_CHAN; i++) {
    buf[i] = (float *)fts_word_get_ptr(argv + 2 + i);
  }

  fts_audiofile_read(this, &buf[0], NUM_CHAN, n); 
}

static void
fts_audiofile_ftl_write(fts_word_t *argv)
{
  fts_audiofile_t *this = (fts_audiofile_t *)fts_word_get_ptr(argv + 0);
  int n = fts_word_get_int(argv + 1);
  float* buf[NUM_CHAN];
  int i;

  for (i = 0; i < NUM_CHAN; i++) {
    buf[i] = (float *)fts_word_get_ptr(argv + 2 + i);
  }

  fts_audiofile_write(this, &buf[0], NUM_CHAN, n); 
}

#endif
