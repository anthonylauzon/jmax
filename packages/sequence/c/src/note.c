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

fts_class_t *scoob_class = 0;

typedef struct
{
  fts_symbol_t name;
  fts_symbol_t type;
} scoob_property_t;

static int scoob_n_properties = 0;
static scoob_property_t scoob_properties[512];
static fts_hashtable_t scoob_property_indices;

static fts_symbol_t scoob_type_names[n_scoob_types];
static fts_hashtable_t scoob_type_indices;

/***********************************************************************
 *
 *  scoob types
 *
 */
enum scoob_type_enum
scoob_get_type_by_name(fts_symbol_t name)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  fts_hashtable_get(&scoob_type_indices, &k, &a);

  return fts_get_int(&a);
}

enum midi_type
scoob_get_type_from_atom(const fts_atom_t *at)
{
  fts_atom_t k;

  if(fts_is_int(at))
    return fts_get_int(at);
  else if(fts_is_symbol(at))
  {
    fts_atom_t a;

    fts_hashtable_get(&scoob_type_indices, &k, &a);

    return fts_get_int(&a);
  }
  else
    return scoob_none;
}

/***********************************************************************
 *
 *  scoob properties
 *
 */
static void
scoob_property_get_by_index(scoob_t *this, int index, fts_atom_t *p)
{
  if(index < fts_array_get_size(&this->properties))
    fts_atom_assign(p, fts_array_get_element(&this->properties, index));
}

static void
scoob_property_get(scoob_t *this, fts_symbol_t name, fts_atom_t *p)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&scoob_property_indices, &k, &a))
    scoob_property_get_by_index(this, fts_get_int(&a), p);
}

static void
scoob_property_set_by_index(scoob_t *this, int index, const fts_atom_t *value)
{
  fts_array_set_element(&this->properties, index, value);
}

int
scoob_property_set(scoob_t *this, fts_symbol_t name, const fts_atom_t *value)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&scoob_property_indices, &k, &a))
  {
    scoob_property_set_by_index(this, fts_get_int(&a), value);
    return 1;
  }

  return 0;
}

static void
scoob_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  if(ac > 0)
    scoob_property_set(this, s, at);
  else
    scoob_property_get(this, s, fts_get_return_value());
}

void
scoob_declare_property(fts_symbol_t name, fts_symbol_t type)
{
  fts_atom_t k, a;

  if(!fts_class_get_method_varargs(scoob_class, name))
  {
    fts_set_symbol(&k, name);
    fts_set_int(&a, scoob_n_properties);

    fts_hashtable_put(&scoob_property_indices, &k, &a);

    scoob_properties[scoob_n_properties].name = name;
    scoob_properties[scoob_n_properties].type = type;
    
    fts_class_message_varargs(scoob_class, name, scoob_property);

    scoob_n_properties++;
  }
}

void
scoob_set_velocity(scoob_t *this, int velocity)
{
  fts_atom_t a;

  fts_set_int(&a, velocity);
  scoob_property_set_by_index(this, 0, &a);
}

int
scoob_get_velocity(scoob_t *this)
{
  fts_atom_t a;

  fts_set_void(&a);
  scoob_property_get_by_index(this, 0, &a);

  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

void
scoob_set_channel(scoob_t *this, int channel)
{
  fts_atom_t a;

  fts_set_int(&a, channel);
  scoob_property_set_by_index(this, 1, &a);
}

int
scoob_get_channel(scoob_t *this)
{
  fts_atom_t a;

  fts_set_void(&a);
  scoob_property_get_by_index(this, 1, &a);

  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

static void
scoob_get_property_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_array_t *array = fts_get_pointer(at);
  int i;

  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, fts_s_symbol);

  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_symbol(array, fts_s_float);

  fts_array_append_symbol(array, seqsym_interval);
  fts_array_append_symbol(array, fts_s_float);

  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_symbol(array, fts_s_float);

  for(i=0; i<scoob_n_properties; i++)
  {
    fts_array_append_symbol(array, scoob_properties[i].name);
    fts_array_append_symbol(array, scoob_properties[i].type);
  }
}

static void
scoob_append_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  int size = fts_array_get_size(&this->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&this->properties);
  int i;

  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, scoob_type_names[this->type]);

  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_float(array, this->pitch);

  if(this->interval != 0.0)
  {
    fts_array_append_symbol(array, seqsym_interval);
    fts_array_append_float(array, this->interval);
  }

  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_float(array, this->duration);

  for(i=0; i<size; i++)
  {
    if(!fts_is_void(atoms + i))
    {
      fts_array_append_symbol(array, scoob_properties[i].name);
      fts_array_append(array, 1, atoms + i);
    }
  }
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
_scoob_set_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  this->type = scoob_get_type_from_atom(at);
}

static void
_scoob_get_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  fts_return_int(this->type);
}

static void
_scoob_set_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
  int pitch = fts_get_number_float(at);

  if(pitch < 0.0)
    pitch = 0.0;

  this->pitch = pitch;
}

static void
_scoob_get_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  fts_return_float(this->pitch);
}

static void
_scoob_set_interval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
  int interval = fts_get_number_float(at);

  if(interval < 0.0)
    interval = 0.0;

  this->interval = interval;
}

static void
_scoob_get_interval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  fts_return_float(this->interval);
}	

static void
_scoob_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  double duration = fts_get_number_float(at);

  if(duration < 0.0)
    duration = 0.0;

  this->duration = duration;
}

static void
_scoob_get_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  fts_return_float(this->duration);
}

static void
scoob_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  if(ac > 2)
  {
    _scoob_set_type(o, 0, 0, 1, at);

    if(fts_is_number(at + 1))
      _scoob_set_pitch(o, 0, 0, 1, at + 1);

    if(fts_is_number(at + 2))
      _scoob_set_interval(o, 0, 0, 1, at + 2);

    if(ac > 3 && fts_is_number(at + 3))
      _scoob_set_duration(o, 0, 0, 1, at + 3);
  }
  else if(ac > 0)
  {
    this->type = scoob_note;
    
    /* compatibility with former two argument note */
    if(fts_is_number(at))
      _scoob_set_pitch(o, 0, 0, 1, at);

    if(ac > 1 && fts_is_number(at + 1))
      _scoob_set_duration(o, 0, 0, 1, at + 1);
  }  
}

static void 
scoob_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);

  switch(this->type)
  {
    case scoob_note:
      fts_tuple_append_symbol(tuple, seqsym_note);
      fts_tuple_append_float(tuple, this->pitch);
      fts_tuple_append_float(tuple, this->duration);
      break;
    case scoob_interval:
      fts_tuple_append_symbol(tuple, seqsym_interval);
      fts_tuple_append_float(tuple, this->pitch);
      fts_tuple_append_float(tuple, this->interval);
      fts_tuple_append_float(tuple, this->duration);
      break;
    case scoob_rest:
      fts_tuple_append_symbol(tuple, seqsym_rest);
      fts_tuple_append_float(tuple, this->duration);
      break;
    case scoob_trill:
      fts_tuple_append_symbol(tuple, seqsym_trill);
      fts_tuple_append_float(tuple, this->pitch);
      fts_tuple_append_float(tuple, this->interval);
      fts_tuple_append_float(tuple, this->duration);
      break;
    default:
      break;
  }

  fts_return_object((fts_object_t *)tuple);
}

static void 
scoob_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int i;
  
  switch(this->type)
  {
    case scoob_note:
      fts_spost(stream, "(:scoob note %g %g", this->pitch, this->duration);
      break;
    case scoob_interval:
      fts_spost(stream, "(:scoob interval %g %g %g", this->pitch, this->interval, this->duration);
      break;
    case scoob_rest:
      fts_spost(stream, "(:scoob rest %g", this->duration);
      break;
    case scoob_trill:
      fts_spost(stream, "(:scoob trill %g %g", this->pitch, this->interval, this->duration);
      break;
    default:
      break;
  }  

  for(i=0; i<fts_array_get_size(&this->properties); i++)
  {
    fts_atom_t *a = fts_array_get_element(&this->properties, i);
    
    if(!fts_is_void(a))
    {
      fts_spost(stream, ", %s: ", scoob_properties[i].name);
      fts_spost_atoms(stream, 1, a);
    }
  }

  fts_spost(stream, ")", this->pitch, this->duration);
}

static void
scoob_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;
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
      fts_dumper_send(dumper, scoob_properties[i].name, 1, a);
  }
}

static void
scoob_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *this = (scoob_t *)o;

  this->type = scoob_note;
  this->pitch = scoob_DEF_PITCH;
  this->interval = 0;
  this->duration = scoob_DEF_DURATION;

  scoob_set(o, 0, 0, ac, at);
  fts_array_init(&this->properties, 0, 0);
}

static void
scoob_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scoob_t), scoob_init, 0);

  fts_class_message_varargs(cl, fts_s_dump_state, scoob_dump_state);

  fts_class_message_varargs(cl, fts_s_get_tuple, scoob_get_tuple);
  fts_class_message_varargs(cl, fts_s_set, scoob_set);

  fts_class_message_varargs(cl, fts_s_post, scoob_post);

  fts_class_message_varargs(cl, seqsym_get_property_list, scoob_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, scoob_append_properties);

  fts_class_message_number(cl, seqsym_type, _scoob_set_type);
  fts_class_message_number(cl, seqsym_pitch, _scoob_set_pitch);
  fts_class_message_number(cl, seqsym_interval, _scoob_set_interval);
  fts_class_message_number(cl, seqsym_duration, _scoob_set_duration);

  fts_class_message_void(cl, seqsym_type, _scoob_get_type);
  fts_class_message_void(cl, seqsym_pitch, _scoob_get_pitch);
  fts_class_message_void(cl, seqsym_interval, _scoob_get_interval);
  fts_class_message_void(cl, seqsym_duration, _scoob_get_duration);

  fts_hashtable_init(&scoob_property_indices, FTS_HASHTABLE_SMALL);
  
  scoob_declare_property(seqsym_velocity, fts_s_int); /* property 0 */
  scoob_declare_property(seqsym_channel, fts_s_int); /* property 1 */
}

void
scoob_config(void)
{
  int i;
  
  scoob_class = fts_class_install(seqsym_scoob, scoob_instantiate);
  fts_class_alias(scoob_class, seqsym_note);

  scoob_type_names[scoob_none] = seqsym_undefined;
  scoob_type_names[scoob_note] = seqsym_note;
  scoob_type_names[scoob_interval] = seqsym_interval;
  scoob_type_names[scoob_rest] = seqsym_rest;
  scoob_type_names[scoob_trill] = seqsym_trill;

  fts_hashtable_init(&scoob_type_indices, FTS_HASHTABLE_SMALL);
  for(i=0; i<n_scoob_types; i++)
  {
    fts_atom_t k, a;

    fts_set_symbol(&k, scoob_type_names[i]);
    fts_set_int(&a, i);
    fts_hashtable_put(&scoob_type_indices, &k, &a);
  }

}
