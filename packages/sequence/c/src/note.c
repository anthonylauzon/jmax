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
#include <fts/packages/sequence/sequence.h>
#include <fts/packages/sequence/track.h>
#include <fts/packages/sequence/note.h>
#include <fts/packages/sequence/seqsym.h>

fts_class_t *note_type = 0;

typedef struct
{
  fts_symbol_t name;
  fts_symbol_t type;
} note_property_t;

static int note_n_properties = 0;
static note_property_t note_properties[512];
static fts_hashtable_t note_property_indices;

static void
note_property_get_by_index(note_t *this, int index, fts_atom_t *p)
{
  if(index < fts_array_get_size(&this->properties))
    fts_atom_assign(p, fts_array_get_element(&this->properties, index));
}

static void
note_property_get(note_t *this, fts_symbol_t name, fts_atom_t *p)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&note_property_indices, &k, &a))
    note_property_get_by_index(this, fts_get_int(&a), p);
}

static void
note_property_set_by_index(note_t *this, int index, const fts_atom_t *value)
{
  fts_array_set_element(&this->properties, index, value);
}

int
note_property_set(note_t *this, fts_symbol_t name, const fts_atom_t *value)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&note_property_indices, &k, &a))
  {
    note_property_set_by_index(this, fts_get_int(&a), value);
    return 1;
  }

  return 0;
}

static void
note_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  if(ac > 0)
    note_property_set(this, s, at);
  else
    note_property_get(this, s, fts_get_return_value());
}

void
note_declare_property(fts_symbol_t name, fts_symbol_t type)
{
  fts_atom_t k, a;

  if(!fts_class_get_method_varargs(note_type, name))
  {
    fts_set_symbol(&k, name);
    fts_set_int(&a, note_n_properties);

    fts_hashtable_put(&note_property_indices, &k, &a);

    note_properties[note_n_properties].name = name;
    note_properties[note_n_properties].type = type;
    
    fts_class_message_varargs(note_type, name, note_property);

    note_n_properties++;
  }
}

void
note_set_velocity(note_t *this, int velocity)
{
  fts_atom_t a;

  fts_set_int(&a, velocity);
  note_property_set_by_index(this, 0, &a);
}

int
note_get_velocity(note_t *this)
{
  fts_atom_t a;

  fts_set_void(&a);
  note_property_get_by_index(this, 0, &a);

  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

void
note_set_channel(note_t *this, int channel)
{
  fts_atom_t a;

  fts_set_int(&a, channel);
  note_property_set_by_index(this, 1, &a);
}

int
note_get_channel(note_t *this)
{
  fts_atom_t a;

  fts_set_void(&a);
  note_property_get_by_index(this, 1, &a);

  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
note_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  if(ac > 0)
  {
    if(fts_is_number(at))
    {
      int pitch = fts_get_number_int(at);

      if(pitch < 0)
        pitch = 0;

      this->pitch = pitch;
    }
  }
  else
    fts_return_int(this->pitch);
}
  
static void
note_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  if(ac > 0)
  {
    if(fts_is_number(at))
    {
      double duration = fts_get_number_float(at);

      if(duration < 0.0)
        duration = 0.0;

      this->duration = duration;
    }
  }
  else
    fts_return_float(this->duration);  
}

static void
note_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
    {
    default:
    case 2:
      note_duration(o, 0, 0, 1, at + 1);
    case 1:
      note_pitch(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static void 
note_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  
  fts_tuple_append_int(tuple, this->pitch);
  fts_tuple_append_float(tuple, this->duration);
  
  fts_return_object((fts_object_t *)tuple);
}

static void 
note_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int i;

  fts_spost(stream, "(:note %d %f", this->pitch, this->duration);

  for(i=0; i<fts_array_get_size(&this->properties); i++)
  {
    fts_atom_t *a = fts_array_get_element(&this->properties, i);
    
    if(!fts_is_void(a))
    {
      fts_spost(stream, ", %s: ", note_properties[i].name);
      fts_spost_atoms(stream, 1, a);
    }
  }

  fts_spost(stream, ")", this->pitch, this->duration);
}

static void
note_get_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_array_t *array = fts_get_pointer(at);
  int i;

  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_symbol(array, fts_s_int);

  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_symbol(array, fts_s_float);

  for(i=0; i<note_n_properties; i++)
  {
    fts_array_append_symbol(array, note_properties[i].name);
    fts_array_append_symbol(array, note_properties[i].type);
  }
}

static void
note_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess;
  int i;

  /* send set message with pitch and duration */
  mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_message_append_int(mess, this->pitch);
  fts_message_append_float(mess, this->duration);
  fts_dumper_message_send(dumper, mess);

  /* send a message for each of the properties */
  for(i=0; i<fts_array_get_size(&this->properties); i++)
  {
    fts_atom_t *a = fts_array_get_element(&this->properties, i);

    if(!fts_is_void(a))
      fts_dumper_send(dumper, note_properties[i].name, 1, a);
  }
}

static void
note_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  this->pitch = NOTE_DEF_PITCH;
  this->duration = NOTE_DEF_DURATION;

  note_set(o, 0, 0, ac, at);
  fts_array_init(&this->properties, 0, 0);
}

static void
note_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(note_t), note_init, 0);

  fts_class_message_varargs(cl, fts_new_symbol("get_properties"), note_get_properties);
  fts_class_message_varargs(cl, fts_s_dump_state, note_dump_state);

  fts_class_message_varargs(cl, fts_s_get_tuple, note_get_tuple);
  fts_class_message_varargs(cl, fts_s_set, note_set);

  fts_class_message_varargs(cl, fts_s_post, note_post);

  fts_class_message_varargs(cl, seqsym_pitch, note_pitch);
  fts_class_message_varargs(cl, seqsym_duration, note_duration);

  fts_hashtable_init(&note_property_indices, FTS_HASHTABLE_SMALL);
  
  note_declare_property(seqsym_velocity, fts_s_int); /* property 0 */
  note_declare_property(seqsym_channel, fts_s_int); /* property 1 */
}

void
note_config(void)
{
  note_type = fts_class_install(seqsym_note, note_instantiate);
}
