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

#include <fts/fts.h>
#include <fts/project.h>
#include <ftsprivate/package.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/class.h>
#include <ftsprivate/bmaxhdr.h>
#include <ftsprivate/saver.h>
#include <ftsprivate/audio.h>
#include <ftsconfig.h>
#include <stdlib.h> 

#if HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif



/***********************************************************************
 *
 * The globals
 *
 */

static fts_project_t* fts_project = NULL;
static fts_class_t *fts_project_class = NULL;

/*  from package.c */
extern fts_package_t* fts_system_package;

void fts_package_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
void fts_package_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
void fts_package_require(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
void fts_package_template_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
void fts_package_data_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
void fts_package_abstraction_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
int fts_package_bmax_file_open(fts_package_t *this, fts_bmax_file_t* f, const char* filename);
int fts_package_bmax_file_close(fts_package_t *this, fts_bmax_file_t* f);
int fts_package_bmax_file_save(fts_package_t *this, fts_bmax_file_t* f);


/***********************************************************************
 *
 * Macros
 *
 */

extern fts_class_t *fts_array_class;

#define fts_array_new(_ac,_at) (fts_array_t *) fts_object_create(fts_array_class, _ac, _at); 
#define fts_array_delete(_array) fts_object_destroy((fts_object_t*)_array); 


/***********************************************************************
 *
 * The project object
 *
 */

struct _fts_project_t
{
  fts_package_t package;

  /* The sample rate, the fifosize, the init arguments for midi
     {in,out} and audio {in,out} are copied and stored in the project
     structure. They become effective when an 'apply' message is
     send. */

  int sample_rate;
  int fifo_size;

  int midi_changed;
  fts_array_t* midi;

  int midi_in_changed;
  fts_array_t* midi_in;

  int midi_out_changed;
  fts_array_t* midi_out;

  int audio_changed;
  fts_array_t* audio;
  fts_array_t* audio_in;
  fts_array_t* audio_out;
};


fts_project_t* 
fts_project_new(fts_symbol_t name)
{
  fts_atom_t a[2];
  
  fts_set_symbol(&a[0], fts_s_project);
  fts_set_symbol(&a[1], name);

  return (fts_project_t *) fts_eval_object_description(fts_get_root_patcher(), 2, a);
}

/* this is a copy of fts_package_load_from_file() adapted for projects */
fts_project_t* 
fts_project_open(const char* filename)
{
  char path[MAXPATHLEN];
  char dir[MAXPATHLEN];
  fts_object_t* obj;
  fts_project_t* project = NULL;

  /* this is a hack but not a big one: load the project in the context
     of the system package. */
  fts_package_push(fts_system_package);

  fts_make_absolute_path(NULL, filename, path, MAXPATHLEN);

  obj = fts_binary_file_load(path, (fts_object_t *) fts_get_root_patcher(), 0, 0, 0);

  if (!obj) {
    fts_log("[project]: Failed to load project file %s\n", path);
    project = fts_project_new(fts_s_project);
    project->package.state = fts_package_corrupt;

  } else if (fts_object_get_class(obj) != fts_project_class) {

    fts_log("[project]: Invalid project file %s\n", path);
    fts_object_delete_from_patcher(obj);
    project = fts_project_new(fts_s_project);
    project->package.state = fts_package_corrupt;

#if 0
    /* FIXME bootstrapping code to create the first project (ever!)  */
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("utils"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("system"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("data"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("guiobj"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("control"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("numeric"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("math"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("ispw"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("lists"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("midi"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("sequence"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("signal"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("ispwmath"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("qlist"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("explode"));
    fts_package_require_package((fts_package_t*) project, fts_new_symbol("io"));
#endif

  } else {
    project = (fts_project_t*) obj;
  }

  fts_dirname(path, dir, MAXPATHLEN);
  project->package.dir = fts_new_symbol_copy(dir);
  project->package.name = fts_s_project;
  project->package.filename = fts_new_symbol_copy(path);

  fts_package_pop(fts_system_package);

  return project;
}

void 
fts_project_destroy(fts_project_t* project)
{
  fts_object_delete_from_patcher((fts_object_t *) project);
}

static void 
fts_project_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_project_t* project = (fts_project_t *)o;

  fts_package_init(o, winlet, s, ac, at);

  project->sample_rate = 0;
  project->fifo_size = 0;

  project->midi_changed = 0;
  project->midi = NULL;
  project->midi_in_changed = 0;
  project->midi_in = NULL;
  project->midi_out_changed = 0;
  project->midi_out = NULL;

  project->audio_changed = 0;
  project->audio = NULL;
  project->audio_in = NULL;
  project->audio_out = NULL;
}

static void 
fts_project_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_project_t* project = (fts_project_t *)o;

  fts_package_delete(o, winlet, s, ac, at);

  if (project->midi != NULL) {
    fts_array_delete(project->midi);
  }
  if (project->midi_in != NULL) {
    fts_array_delete(project->midi_in);
  }
  if (project->midi_out != NULL) {
    fts_array_delete(project->midi_out);
  }
  if (project->audio != NULL) {
    fts_array_delete(project->audio);
  }
  if (project->audio_in != NULL) {
    fts_array_delete(project->audio_in);
  }
  if (project->audio_out != NULL) {
    fts_array_delete(project->audio_out);
  }
}

/* apply the settings of this project */
static void
fts_project_apply(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t *)o;
  fts_atom_t a[1];

  if (project->fifo_size > 0) {
    fts_set_int(a, project->fifo_size);
    fts_param_set(fts_s_fifo_size, a);
  }

  if (project->sample_rate > 0) {
    fts_set_int(a, project->sample_rate);
    fts_param_set(fts_s_sample_rate, a);  
  }

  /* if the user specified a default audio then so be it: that's what
     he will get. otherwise, check if the user specified an audio in
     and/or audio out. */
  if (project->audio_changed) {

    if ((project->audio != NULL) && (fts_array_get_size(project->audio) > 0)) {
      
      fts_audioport_set_default(fts_array_get_size(project->audio), fts_array_get_atoms(project->audio));
      
    } else {
      
      if ((project->audio_in != NULL) && (fts_array_get_size(project->audio_in) > 0)) {
	fts_audioport_set_default_in(fts_array_get_size(project->audio_in), fts_array_get_atoms(project->audio_in));
      } 
      if ((project->audio_out != NULL) && (fts_array_get_size(project->audio_out) > 0)) {
	fts_audioport_set_default_out(fts_array_get_size(project->audio_out), fts_array_get_atoms(project->audio_out));
      } 
    }

    project->audio_changed = 0;
  }

  /* check if the midi configuration changed and if so, reopen the
     existing midi ports with the new settings */
  if (project->midi_changed) {
    fts_midiport_reopen_default(fts_array_get_size(project->midi), fts_array_get_atoms(project->midi));
    project->midi_changed = 0;
  }

  if (project->midi_in_changed) {
    fts_midiport_reopen_default_in(fts_array_get_size(project->midi_in), fts_array_get_atoms(project->midi_in));
    project->midi_in_changed = 0;
  }

  if (project->midi_out_changed) {
    fts_midiport_reopen_default_out(fts_array_get_size(project->midi_out), fts_array_get_atoms(project->midi_out));
    project->midi_out_changed = 0;
  }
}

static void
fts_project_bang(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
}

static void
fts_project_sample_rate(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if ((ac > 0) && fts_is_number(at)) {
    project->sample_rate = fts_get_number_int(at);
  }
}

static void
fts_project_fifo_size(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if ((ac > 0) && fts_is_number(at)) {
    project->fifo_size = fts_get_number_int(at);
  }
}

static void
fts_project_midi(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->midi != NULL) {
    fts_array_clear(project->midi);
    fts_array_set(project->midi, ac, at);
  } else {
    project->midi = fts_array_new(ac, at);
  }

  project->midi_changed = 1;
}

static void
fts_project_midi_in(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->midi_in != NULL) {
    fts_array_clear(project->midi_in);
    fts_array_set(project->midi_in, ac, at);
  } else {
    project->midi_in = fts_array_new(ac, at);
  }

  project->midi_in_changed = 1;
}

static void
fts_project_midi_out(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->midi_out != NULL) {
    fts_array_clear(project->midi_out);
    fts_array_set(project->midi_out, ac, at);
  } else {
    project->midi_out = fts_array_new(ac, at);
  }

  project->midi_out_changed = 1;
}

static void
fts_project_audio(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->audio != NULL) {
    fts_array_clear(project->audio);
    fts_array_set(project->audio, ac, at);
  } else {
    project->audio = fts_array_new(ac, at);
  }

  project->audio_changed = 1;
}

static void
fts_project_audio_in(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->audio_in != NULL) {
    fts_array_clear(project->audio_in);
    fts_array_set(project->audio_in, ac, at);
  } else {
    project->audio_in = fts_array_new(ac, at);
  }

  project->audio_changed = 1;
}

static void
fts_project_audio_out(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_t* project = (fts_project_t*) o;

  if (project->audio_out != NULL) {
    fts_array_clear(project->audio_out);
    fts_array_set(project->audio_out, ac, at);
  } else {
    project->audio_out = fts_array_new(ac, at);
  }

  project->audio_changed = 1;
}


void
fts_project_get_default_midi(int* pac, fts_atom_t** pat)
{
  if (fts_project->midi != NULL) {
    *pac = fts_array_get_size(fts_project->midi);
    *pat = fts_array_get_atoms(fts_project->midi);
  } else {
    *pac = 0;
    *pat = NULL;
  }
}

void
fts_project_get_default_midi_in(int* pac, fts_atom_t** pat)
{
  if (fts_project->midi_in != NULL) {
    *pac = fts_array_get_size(fts_project->midi_in);
    *pat = fts_array_get_atoms(fts_project->midi_in);
  } else {
    *pac = 0;
    *pat = NULL;
  }
}

void
fts_project_get_default_midi_out(int* pac, fts_atom_t** pat)
{
  if (fts_project->midi_out != NULL) {
    *pac = fts_array_get_size(fts_project->midi_out);
    *pat = fts_array_get_atoms(fts_project->midi_out);
  } else {
    *pac = 0;
    *pat = NULL;
  }
}


static int
fts_project_bmax_file_save(fts_project_t *project, fts_bmax_file_t* f)
{
  /* call the package's save method */
  if (fts_package_bmax_file_save((fts_package_t*) project, f) < 0) 
    {
      post( "Failed to save the project\n");
      return -1;
    }

  /* save midi */
  if ((project->midi != NULL) && (fts_array_get_size(project->midi) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->midi), fts_array_get_atoms(project->midi));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_midi, fts_array_get_size(project->midi));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->midi));
  }

  /* save midi in */
  if ((project->midi_in != NULL) && (fts_array_get_size(project->midi_in) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->midi_in), fts_array_get_atoms(project->midi_in));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_midi_in, fts_array_get_size(project->midi_in));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->midi_in));
  }

  /* save midi out */
  if ((project->midi_out != NULL) && (fts_array_get_size(project->midi_in) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->midi_out), fts_array_get_atoms(project->midi_out));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_midi_out, fts_array_get_size(project->midi_out));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->midi_out));
  }

  /* save audio */
  if ((project->audio != NULL) && (fts_array_get_size(project->audio) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->audio), fts_array_get_atoms(project->audio));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_audio, fts_array_get_size(project->audio));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->audio));
  }

  /* save audio in */
  if ((project->audio_in != NULL) && (fts_array_get_size(project->audio_in) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->audio_in), fts_array_get_atoms(project->audio_in));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_audio_in, fts_array_get_size(project->audio_in));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->audio_in));
  }

  /* save audio out */
  if ((project->audio_out != NULL) && (fts_array_get_size(project->audio_out) > 0)) {
    fts_bmax_code_push_atoms(f, fts_array_get_size(project->audio_out), fts_array_get_atoms(project->audio_out));
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_audio_out, fts_array_get_size(project->audio_out));
    fts_bmax_code_pop_args(f, fts_array_get_size(project->audio_out));
  }
 
  /* save fifo size */
  if (project->fifo_size > 0) {
    fts_bmax_code_push_int(f, project->fifo_size);
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_fifo_size, 1);
    fts_bmax_code_pop_args(f, 1);    
  }
 
  /* save sample rate */
  if (project->sample_rate > 0) {
    fts_bmax_code_push_float(f, (float) project->sample_rate);
    fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_sample_rate, 1);
    fts_bmax_code_pop_args(f, 1);    
  }
  
  return 0;
}


static void 
fts_project_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_project_t *project = (fts_project_t *)o;
  fts_bmax_file_t f;
  const char *filename;


  if (project->package.filename == NULL) {
    post( "Project has no associated filename. Use \"Save As\".\n");    
    return;
  }
  
  filename = fts_symbol_name( project->package.filename);

  if (fts_package_bmax_file_open((fts_package_t*) project, &f, filename) < 0)
    {
      post( "Cannot open file %s\n", filename);
      return;
    }

  if (fts_project_bmax_file_save(project, &f) < 0) 
    {
      post( "Failed to save the project\n");      
    }

  fts_package_bmax_file_close((fts_package_t*) project, &f);
}


static void 
fts_project_saveas(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_project_t *project = (fts_project_t *)o;
  fts_bmax_file_t f;
  const char *filename;

  if (ac == 0) {
    post( "No filename specified\n");    
    return;
  }

  filename = fts_symbol_name(fts_get_symbol(at));

  if (project->package.filename == NULL) {
    project->package.filename = fts_get_symbol(at);
  }

  if (fts_package_bmax_file_open((fts_package_t*) project, &f, filename) < 0)
    {
      post( "Cannot open file %s\n", filename);
      return;
    }

  if (fts_project_bmax_file_save(project, &f) < 0) 
    {
      post( "Failed to save the project\n");      
    }

  fts_package_bmax_file_close((fts_package_t*) project, &f);
}


static fts_status_t
fts_project_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i, inlet;

  fts_class_init(cl, sizeof(fts_project_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_project_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_project_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, fts_project_bang);
  fts_method_define_varargs(cl, 0, fts_s_save, fts_project_save);
  fts_method_define_varargs(cl, 0, fts_s_saveas, fts_project_saveas);

  inlet = fts_SystemInlet;

  /* methods that are available on inlet 0 and system inlet */
  for (i = 0; i < 2; i++) {

    /* call the inherited methods */
    fts_method_define_varargs(cl, inlet, fts_s_require, fts_package_require);
    fts_method_define_varargs(cl, inlet, fts_s_template_path, fts_package_template_path);
    fts_method_define_varargs(cl, inlet, fts_s_abstraction_path, fts_package_abstraction_path);
    fts_method_define_varargs(cl, inlet, fts_s_data_path, fts_package_data_path);

    /* project specific methods */
    fts_method_define_varargs(cl, inlet, fts_s_sample_rate, fts_project_sample_rate);
    fts_method_define_varargs(cl, inlet, fts_s_fifo_size, fts_project_fifo_size);
    fts_method_define_varargs(cl, inlet, fts_s_midi, fts_project_midi);
    fts_method_define_varargs(cl, inlet, fts_s_midi_in, fts_project_midi_in);
    fts_method_define_varargs(cl, inlet, fts_s_midi_out, fts_project_midi_out);
    fts_method_define_varargs(cl, inlet, fts_s_audio, fts_project_audio);
    fts_method_define_varargs(cl, inlet, fts_s_audio_in, fts_project_audio_in);
    fts_method_define_varargs(cl, inlet, fts_s_audio_out, fts_project_audio_out);
    fts_method_define_varargs(cl, inlet, fts_s_apply, fts_project_apply);

    inlet = 0;
  }
 
  return fts_Success;
} 


/***********************************************************************
 *
 * The global interface to the current project
 *
 */

int 
fts_project_close(void)
{
  if (fts_project == NULL) {
    post("Error: can't close non-existing project");
    return -1;
  }
  
  /* the current package must be the project. it's an error
     otherwise. */
  if (fts_get_current_package() != (fts_package_t*) fts_project) {
    post("Error: can't close project when the project is not the current context");
    return -1;
  }

  /* pop the project of the package context stack */
  fts_package_pop((fts_package_t*) fts_project);
  
  fts_object_destroy( (fts_object_t*) fts_project);
  fts_project = NULL; 

  return 0;
}

void
fts_project_set(fts_project_t* p)
{
  if (fts_project != NULL) {
    fts_project_close();
  }

  fts_project = p;

  /* make the project the current package context */
  fts_package_push((fts_package_t*) fts_project);  

  fts_project_apply((fts_object_t*) fts_project, 0, NULL, 0, NULL);
}

fts_project_t* 
fts_project_get(void)
{
  return fts_project;
}

fts_symbol_t
fts_project_get_dir(void)
{
  if ((fts_project != NULL ) && (fts_package_get_dir((fts_package_t*) fts_project) != NULL)) {
    return fts_package_get_dir((fts_package_t*) fts_project);
  } else {
    char buf[1024];
    
    return fts_new_symbol_copy(getcwd(buf, 1024));
  }
}

fts_symbol_t
fts_project_get_data_file(fts_symbol_t filename)
{
  fts_symbol_t path;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  if (fts_project == NULL ) {
    return filename;
  }

  /* first try to find the file in the current project */
  path = fts_package_get_data_file((fts_package_t*) fts_project, filename);
  if (path != NULL) {
    return path;
  }

  /* if it's not in the project, try to find it in the required
     packages */
  fts_package_get_required_packages((fts_package_t*) fts_project, &pkg_iter);

  while (fts_iterator_has_more(&pkg_iter)) {
    
    fts_iterator_next(&pkg_iter, &pkg_name);
    pkg = fts_package_get(fts_get_symbol(&pkg_name));
    
    if (pkg == NULL) {
      continue;
    }

    path = fts_package_get_data_file(pkg, filename);
    
    if (path != NULL) {
      return path;
    }
  }

  return filename;
}



/***********************************************************************
 *
 * The interface to access the current project
 *
 * This is a cardboard interface to the current project
 *
 */

typedef struct _fts_current_project_t {
  fts_object_t object;
} fts_current_project_t;

static void 
fts_current_project_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void 
fts_current_project_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void
fts_current_project_bang(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
}

static void
fts_current_project_require(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_package_require((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_template_path(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_package_template_path((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_abstraction_path(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_package_abstraction_path((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_data_path(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_package_data_path((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_sample_rate(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_sample_rate((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_fifo_size(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_fifo_size((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_midi(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_midi((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_midi_in(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_midi_in((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_midi_out(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_midi_out((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_audio(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_audio((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_audio_in(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_audio_in((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_audio_out(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_audio_out((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_save(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_save((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_saveas(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_saveas((fts_object_t*) fts_project, winlet, s, ac, at);
}

static void
fts_current_project_apply(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_project_apply((fts_object_t*) fts_project, winlet, s, ac, at);
}

static fts_status_t
fts_current_project_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_current_project_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_current_project_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_current_project_delete);
  fts_method_define_varargs(cl, 0, fts_s_bang, fts_current_project_bang);
  fts_method_define_varargs(cl, 0, fts_s_require, fts_current_project_require);
  fts_method_define_varargs(cl, 0, fts_s_template_path, fts_current_project_template_path);
  fts_method_define_varargs(cl, 0, fts_s_abstraction_path, fts_current_project_abstraction_path);
  fts_method_define_varargs(cl, 0, fts_s_data_path, fts_current_project_data_path);
  fts_method_define_varargs(cl, 0, fts_s_sample_rate, fts_current_project_sample_rate);
  fts_method_define_varargs(cl, 0, fts_s_fifo_size, fts_current_project_fifo_size);
  fts_method_define_varargs(cl, 0, fts_s_midi, fts_current_project_midi);
  fts_method_define_varargs(cl, 0, fts_s_midi_in, fts_current_project_midi_in);
  fts_method_define_varargs(cl, 0, fts_s_midi_out, fts_current_project_midi_out);
  fts_method_define_varargs(cl, 0, fts_s_audio, fts_current_project_audio);
  fts_method_define_varargs(cl, 0, fts_s_audio_in, fts_current_project_audio_in);
  fts_method_define_varargs(cl, 0, fts_s_audio_out, fts_current_project_audio_out);
  fts_method_define_varargs(cl, 0, fts_s_save, fts_current_project_save);
  fts_method_define_varargs(cl, 0, fts_s_saveas, fts_current_project_saveas);
  fts_method_define_varargs(cl, 0, fts_s_apply, fts_current_project_apply);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */
void 
fts_kernel_project_init(void)
{
  fts_metaclass_install(fts_s_current_project, fts_current_project_instantiate, fts_always_equiv);
  fts_metaclass_install(fts_s_project, fts_project_instantiate, fts_always_equiv);
  fts_project_class = fts_class_get_by_name(fts_s_project);
}
