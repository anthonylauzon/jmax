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
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/audioconfig.h> /* require bmaxfile.h */
#include <ftsprivate/midi.h> /* require bmaxfile.h */

#include <ftsprivate/client.h> 
#include <ftsprivate/loader.h> 
#include <ftsprivate/config.h> /* require audioconfig.h and midi.h */

/****************************************************
 *
 *  AUDIO/MIDI configuration class
 *
 */
static fts_symbol_t config_s_name;

fts_class_t* config_type = NULL;

static fts_config_t *config;

static void
config_clear(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* self = (fts_config_t*)o;
  /* send message clear to audio config and mid config */
  fts_send_message((fts_object_t*)self->audio_config, fts_s_clear, ac, at);
  fts_send_message((fts_object_t*)self->midi_config, fts_s_clear, ac, at);
}

static void
config_restore_labels(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* self = (fts_config_t*)o;
  /* send message restore_labels to audio config and midi config */
  fts_send_message((fts_object_t*)self->audio_config, fts_s_clear, ac, at);
  fts_send_message((fts_object_t*)self->midi_config, fts_s_clear, ac, at);

}

void
fts_config_open(fts_symbol_t file_name)
{
  fts_object_t* obj = NULL;
  
  fts_bmax_file_load(file_name, (fts_object_t*)fts_get_root_patcher(), 0, 0, &obj);
  
  if (obj != NULL && fts_object_get_class(obj) == config_type)
  {
    ((fts_config_t*)obj)->file_name = file_name;
    
    /* replace current config by loaded config */
    fts_config_set((fts_config_t*)obj);
    fts_log("[config]: Opening configuration %s\n", file_name);
    post("Open configuration: %s\n", file_name);
  }
  else
  {
    fts_log("[config]: Cannot read AUDIO/MIDI configuration from file %s\n", file_name);
  }
}

/* static void */
/* config_load(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at) */
/* { */
/*   fts_symbol_t file_name = fts_get_symbol(at); */
/*   fts_symbol_t project_dir = fts_project_get_dir(); */
/*   fts_object_t* obj = NULL; */
/*   char path[MAXPATHLEN]; */

/*   fts_make_absolute_path(project_dir, file_name, path, MAXPATHLEN); */
/*   fts_config_open(path); */
/* } */

static void
config_save(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* this = (fts_config_t*)o;
  fts_symbol_t file_name = fts_get_symbol(at);
  fts_symbol_t project_dir = fts_project_get_dir();
  char path[MAXPATHLEN];
  fts_bmax_file_t f;
    
  fts_make_absolute_path( project_dir, file_name, path, MAXPATHLEN);

  post("CONFIG SAVE \n");
  if (fts_bmax_file_open( &f, path, 0, 0, 0) >= 0)
  {
    /* write config object */
    fts_bmax_code_new_object( &f, o, -1);

    /* save audio config */
    fts_audioconfig_dump( this->audio_config, &f);
    
    /* save midi config */
    fts_midiconfig_dump( this->midi_config, &f);
    
    fts_bmax_code_return( &f);
    fts_bmax_file_close( &f);    
    
    fts_config_set_dirty( this, 0);
  }
  else
    fts_log( "config save: cannot open file %s\n", file_name);
}

static void
config_midi_message(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* this = (fts_config_t*)o;
  fts_symbol_t selector = fts_get_symbol( at);
  fts_send_message( ((fts_object_t *)this->midi_config), selector, ac - 1, at + 1);
}

static void
config_audio_message(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* this = (fts_config_t*)o;
  fts_symbol_t selector = fts_get_symbol( at);
  fts_send_message( ((fts_object_t *)this->audio_config), selector, ac - 1, at + 1);
}

/* set config as dirty or as saved.
 * A "setDirty" message is sent to the client after is_dirty flag changed
 */
void fts_config_set_dirty(fts_config_t *this, int is_dirty)
{
  if(( this)&&(this->dirty != is_dirty))
    {
      this->dirty = is_dirty;
      
      if ( fts_object_has_id( (fts_object_t *)this))
	{
	  fts_atom_t a[1];
	  
	  fts_set_int(&a[0], is_dirty);
	  fts_client_send_message((fts_object_t *)this, fts_s_set_dirty, 1, a);
	}
    }
}

static void
config_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* self = (fts_config_t*)o;
    
  /* send message print to audio config and mid config */
  fts_send_message((fts_object_t*)self->audio_config, fts_s_print, ac, at);
  fts_send_message((fts_object_t*)self->midi_config, fts_s_print, ac, at);
}

static void
config_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_config_t *this = (fts_config_t *)o;

  if (this->editor_opened == 1)
    {
      this->editor_opened = 0;
      fts_client_send_message(o, fts_s_closeEditor, 0, 0);
    }
}

static void
config_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_config_t *this = (fts_config_t *)o;
  fts_atom_t a;

  if ( !fts_object_has_id( ((fts_object_t *)this->audio_config)))
    fts_client_register_object( ((fts_object_t *)this->audio_config), fts_get_client_id( o));

  fts_set_int( &a, fts_get_object_id( ((fts_object_t *)this->audio_config)));
  fts_client_send_message( o, fts_s_audio_config, 1, &a);
      
  fts_send_message( ((fts_object_t *)this->audio_config), fts_s_upload, 0, 0);

  if ( !fts_object_has_id( ((fts_object_t *)this->midi_config)))
    fts_client_register_object( ((fts_object_t *)this->midi_config), fts_get_client_id( o));

  fts_set_int( &a, fts_get_object_id( ((fts_object_t *)this->midi_config)));
  fts_client_send_message( o, fts_s_midi_config, 1, &a);
  fts_send_message( ((fts_object_t *)this->midi_config), fts_s_upload, 0, 0);

  if( this->file_name != NULL)
    { 
      fts_set_symbol( &a, this->file_name);
      fts_client_send_message( o, fts_s_name, 1, &a);
    }

  this->uploaded = 1;
}

static void
config_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_config_t *this = (fts_config_t *)o;

  if (!this->uploaded)
    {
      config_upload( o, 0, 0, 0, 0);
    }

  this->editor_opened = 1;
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
config_close(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_object_release( o);
}

static void
config_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* self = (fts_config_t*)o;
  fts_atom_t a;
  fts_audioconfig_t* audio_config;
  fts_midiconfig_t* midi_config;

  self->file_name = NULL;
  self->dirty = 0;

  /* create midi config object */
  midi_config = (fts_midiconfig_t*)fts_object_create( fts_midiconfig_class, 0, 0);
  fts_object_refer((fts_object_t*)midi_config);
  self->midi_config = midi_config;
  /* @@@@@ default midi is set in fts_load_config @@@@@ 
     fts_midiconfig_set_defaults(self->midi_config); 
  */

  /* create audio config object */
  audio_config = (fts_audioconfig_t*)fts_object_create( fts_audioconfig_class, 0, 0);
  fts_object_refer((fts_object_t*)audio_config);
  self->audio_config = audio_config;
  /* @@@@@ defautlt audio is now set in fts_load_config @@@@@ 
     fts_audioconfig_set_defaults(self->audio_config); 
  */
  /* modify object description */
  fts_set_symbol(&a, config_s_name);
  fts_object_set_description(o, 1, &a);
}


static void
config_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_config_t* self = (fts_config_t*)o;
  
  fts_object_release((fts_object_t*)self->audio_config);
  fts_object_release((fts_object_t*)self->midi_config);
}

static void
config_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_config_t), config_init, config_delete);

/*   fts_class_message_varargs(cl, fts_s_load, config_load); */
  fts_class_message_varargs(cl, fts_s_save, config_save);
  fts_class_message_varargs(cl, fts_s_midi_config, config_midi_message);
  fts_class_message_varargs(cl, fts_s_audio_config, config_audio_message);
  fts_class_message_varargs(cl, fts_s_upload, config_upload);

  fts_class_message_varargs(cl, fts_s_openEditor, config_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, config_close_editor);

  fts_class_message_varargs(cl, fts_s_close, config_close);

  fts_class_message_varargs(cl, fts_s_print, config_print);
}

fts_object_t*
fts_audioconfig_get(void)
{
  if (NULL != config)
  {
    return (fts_object_t*)config->audio_config;
  }
  else
  {
    return NULL;
  }
}

fts_object_t* 
fts_midiconfig_get(void)
{
  if (NULL != config)
  {
    return (fts_object_t*)config->midi_config;
  }
  else
  {
    return NULL;
  }
}

void
fts_config_set( fts_config_t *new_config)
{
  fts_config_t* old_config = (fts_config_t*)fts_config_get();
  if( new_config != NULL)
    {
      fts_object_refer((fts_object_t *)new_config);
 
      if( (old_config != NULL) && fts_object_has_id( (fts_object_t *)old_config))
	{
	  fts_atom_t a;
      
	  if( ! fts_object_has_id( (fts_object_t *)new_config))
	    fts_client_register_object(  (fts_object_t *)new_config, fts_get_client_id( (fts_object_t *)old_config));
	  
	  fts_set_int(&a, fts_get_object_id( (fts_object_t *)new_config));
	  fts_client_send_message(  (fts_object_t *) object_get_client( (fts_object_t *)new_config), fts_s_config, 1, &a);
      
	  fts_send_message( (fts_object_t *)new_config, fts_s_upload, 0, 0);
    }
  }
  
  if( old_config != NULL)
    fts_object_release((fts_object_t *)old_config);
    
  config = new_config;
}

fts_object_t* 
fts_config_get(void)
{
  if (NULL != config)
    {
      return (fts_object_t*)config;
    }
  else
    {
      return NULL;
    }
}

void fts_config_config(void)
{
  fts_atom_t a;

  config_s_name = fts_new_symbol("__config");

  /* Configuration class */
  config_type = fts_class_install(config_s_name, config_instantiate);

  /* @@@@@ Default config must be created in fts_load_config @@@@@@ */
#if 0
  /* create config object */
  fts_config_set( (fts_config_t*)fts_object_create(config_type, 0, 0));

  /* define global config variable */
  fts_set_object(&a, config);
  fts_name_set_value(fts_get_root_patcher(), config_s_name, &a);
#endif 
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
