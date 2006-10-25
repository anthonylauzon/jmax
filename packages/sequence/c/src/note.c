/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

fts_class_t *scoob_class = 0;
enumeration_t *scoob_type_enumeration = NULL;

void 
scoob_copy(scoob_t *org, scoob_t *copy)
{
  copy->type = org->type;
  copy->pitch = org->pitch;
  copy->interval = org->interval;
  copy->duration = org->duration;
  
  propobj_copy((propobj_t *)org, (propobj_t *)copy);
}

static void
scoob_copy_function(const fts_object_t *from, fts_object_t *to)
{
  scoob_copy((scoob_t *)from, (scoob_t *)to);
}

static int
scoob_equals(const scoob_t *a, const scoob_t *b)
{
  if(a->type == b->type && a->pitch == b->pitch && a->interval == b->pitch && a->duration == b->duration)
    return propobj_equals((propobj_t *)a, (propobj_t *)b);
  
  return 0;
}

static void 
scoob_array_function(fts_object_t *o, fts_array_t *array)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_array_append_symbol(array, self->type);
  fts_array_append_float(array, self->pitch);
  fts_array_append_float(array, self->interval);
  fts_array_append_float(array, self->duration);
  propobj_append_properties((propobj_t *)self, array);
}

static void 
scoob_description_function(fts_object_t *o, fts_array_t *array)
{  
  fts_array_append_symbol(array, seqsym_scoob);
  scoob_array_function(o, array);
}

static fts_method_status_t
_scoob_set_type(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  fts_symbol_t type = fts_get_symbol(at);
  
  if(enumeration_get_index(scoob_type_enumeration, type) >= 0)
    self->type = type;
  
  return fts_ok;
}

static fts_method_status_t
_scoob_get_type(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_set_symbol(ret, self->type);
  
  return fts_ok;
}

static fts_method_status_t
_scoob_set_pitch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;  
  double pitch = fts_get_number_float(at);
  
  if(pitch < 0.0)
    pitch = 0.0;
  
  self->pitch = pitch;
  
  return fts_ok;
}

static fts_method_status_t
_scoob_get_pitch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_set_float(ret, self->pitch);
  
  return fts_ok;
}

static fts_method_status_t
_scoob_set_interval(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  double interval = fts_get_number_float(at);
  
  if(interval < 0.0)
    interval = 0.0;
  
  self->interval = interval;
  
  return fts_ok;
}

static fts_method_status_t
_scoob_get_interval(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_set_float(ret, self->interval);
  
  return fts_ok;
}       

static fts_method_status_t
_scoob_set_duration(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  double duration = fts_get_number_float(at);
  
  if(duration < 0.0)
    duration = 0.0;
  
  self->duration = duration;
  
  return fts_ok;
}

static fts_method_status_t
_scoob_get_duration(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_set_float(ret, self->duration);
  
  return fts_ok;
}


static fts_method_status_t
scoob_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int i;
  
  switch(ac)
  {
    default:
      for(i=4; i<ac-1; i+=2)
      {  
        if(fts_is_symbol(at + i))
          propobj_set_property_by_name((propobj_t *)o, fts_get_symbol(at + i), at + i + 1);
      }
      
    case 4:
      if(fts_is_number(at + 3))
        _scoob_set_duration(o, NULL, 1, at + 3, fts_nix);
      
    case 3:
      if(fts_is_number(at + 2))
        _scoob_set_interval(o, NULL, 1, at + 2, fts_nix);
      
    case 2:
      if(fts_is_number(at + 1))
        _scoob_set_pitch(o, NULL, 1, at + 1, fts_nix);
      
    case 1:
      if (fts_is_symbol(at))
        _scoob_set_type(o, NULL, 1, at, fts_nix);
      else
        fts_object_error(o, "can't create scoob: need type as first argument");
      
    case 0:
      break;
  }
  
  return fts_ok;
}


static fts_method_status_t
scoob_set_from_scoob(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  scoob_copy((scoob_t *)fts_get_object(at), self);
  
  return fts_ok;
}

static fts_method_status_t
_scoob_stretch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  double stretch = 1.0;
  double duration = self->duration;
  
  switch(ac)
  {
    default:
    case 2:
      if(fts_is_number(at + 1))
        duration = fts_get_number_float(at + 1);        
    case 1:
      if(fts_is_number(at))
        stretch = fts_get_number_float(at);
    case 0:
      break;
  }
  
  if(stretch > 0.0)
    self->duration = (self->duration - duration) + duration * stretch;
  
  return fts_ok;
}  

/*
 * MIDI properties 
 */
void
scoob_set_velocity(scoob_t *self, double velocity)
{
  fts_atom_t a;
  
  fts_set_float(&a, velocity);
  scoob_property_set_by_index(self, scoob_propidx_velocity, &a);
}

double
scoob_get_velocity(scoob_t *self)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  scoob_property_get_by_index(self, scoob_propidx_velocity, &a);
  
  if(fts_is_int(&a))
    return (float)fts_get_int(&a);
  else if(fts_is_float(&a))
    return fts_get_float(&a);
  else
    return (double)-1.0;
}

void
scoob_set_channel(scoob_t *self, int channel)
{
  fts_atom_t a;
  
  fts_set_int(&a, channel);
  scoob_property_set_by_index(self, scoob_propidx_channel, &a);
}

int
scoob_get_channel(scoob_t *self)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  scoob_property_get_by_index(self, scoob_propidx_channel, &a);
  
  if(fts_is_int(&a))
    return fts_get_int(&a);
  else
    return -1;
}

static fts_method_status_t
scoob_get_property_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_array_t *array = fts_get_pointer(at);
  int n_types = enumeration_get_size(scoob_type_enumeration);
  int i;
  
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, seqsym_enum);
  fts_array_append_int(array, n_types);
  
  for(i=0; i<n_types; i++)
    fts_array_append_symbol(array, enumeration_get_name(scoob_type_enumeration, i));      
  
  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_symbol(array, fts_s_float);
  
  fts_array_append_symbol(array, seqsym_interval);
  fts_array_append_symbol(array, fts_s_float);
  
  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_symbol(array, fts_s_float);
  
  propobj_class_append_properties(scoob_class, array);
  
  return fts_ok;
}

static fts_method_status_t
scoob_append_properties(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, self->type);
  
  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_float(array, self->pitch);
  
  fts_array_append_symbol(array, seqsym_interval);
  fts_array_append_float(array, self->interval);
  
  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_float(array, self->duration);
  
  propobj_append_properties((propobj_t *)self, array);
  
  return fts_ok;
}




/******************************************************************************
 *
 *  scoob class
 *
 */

static fts_method_status_t
scoob_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scoob_t *self = (scoob_t *)o;
  
  propobj_init(o);
  
  self->type = seqsym_note;
  self->pitch = 60;
  self->interval = 0;
  self->duration = 100;
  
  scoob_set(o, NULL, ac, at, fts_nix);
  
  return fts_ok;
}

static fts_method_status_t
scoob_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_delete(o);
  
  return fts_ok;
}

static void
scoob_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scoob_t), scoob_init, scoob_delete);
  
  /* types and properties */
  propobj_class_init(cl);
  
  propobj_class_add_float_property(cl, seqsym_velocity, NULL); /* scoob_propidx_velocity = 0 */
  propobj_class_add_int_property(cl, seqsym_channel, NULL); /* scoob_propidx_channel = 1 */
  propobj_class_add_int_property(cl, seqsym_cue, NULL);
  propobj_class_add_float_property(cl, seqsym_offset, NULL);

  /* preliminary (1), until suivi can be initialised before track loading,
     preliminary (2), until section and cue are markers */
  propobj_class_add_int_property(cl, seqsym_section, NULL);
  propobj_class_add_symbol_property(cl, fts_new_symbol("text"), NULL);

  fts_class_message_symbol(cl, fts_s_remove, propobj_remove_property);
  
  fts_class_set_copy_function(cl, scoob_copy_function);
  fts_class_set_array_function(cl, scoob_array_function);
  fts_class_set_description_function(cl, scoob_description_function);
  
  fts_class_message_varargs(cl, seqsym_get_property_list, scoob_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, scoob_append_properties);
  
  fts_class_message_symbol(cl, seqsym_type, _scoob_set_type);
  fts_class_message_void(cl, seqsym_type, _scoob_get_type);
  
  fts_class_message_number(cl, seqsym_pitch, _scoob_set_pitch);
  fts_class_message_void(cl, seqsym_pitch, _scoob_get_pitch);
  
  fts_class_message_number(cl, seqsym_interval, _scoob_set_interval);
  fts_class_message_void(cl, seqsym_interval, _scoob_get_interval);
  
  fts_class_message_number(cl, seqsym_duration, _scoob_set_duration);
  fts_class_message_void(cl, seqsym_duration, _scoob_get_duration);
  
  fts_class_message_varargs(cl, fts_s_set, scoob_set);
  fts_class_message(cl, fts_s_set, cl, scoob_set_from_scoob);
  
  fts_class_message_varargs(cl, seqsym_stretch, _scoob_stretch);
  
  fts_class_doc(cl, seqsym_scoob, "[<'note'|'interval'|'rest'|'trill'|'unvoiced': type> [<num: pitch> [<num: interval> [<num: duration>]]]]", "score object");
  fts_class_doc(cl, fts_s_set, "[<'note'|'interval'|'rest'|'trill'|'unvoiced': type> [<num: pitch> [<num: interval> [<num: duration>]]]]", "set sccob");
  fts_class_doc(cl, fts_s_set, "<scoob: other>", "set from scoob instance");
  fts_class_doc(cl, seqsym_type, "[<'note'|'interval'|'rest'|'trill'|'unvoiced': type>]", "get/set score object type");
  fts_class_doc(cl, seqsym_pitch, "[<num: pitch>]", "get/set pitch as (float) MIDI note number");
  fts_class_doc(cl, seqsym_interval, "[<num: interval>]", "get/set interval in (float) MIDI note numbers");
  fts_class_doc(cl, seqsym_duration, "[<num: duration>]", "get/set duration in msecs");
  fts_class_doc(cl, fts_new_symbol("<property name>"), "[<any: value>]", "get/set additional property");
  fts_class_doc(cl, fts_s_remove, "<sym: property name>", "remove value of given additional property");
}

FTS_MODULE_INIT(scoob)
{
  scoob_type_enumeration = enumeration_new(fts_new_symbol("scoob_type"));
  
  enumeration_add_name(scoob_type_enumeration, seqsym_note);
  enumeration_add_name(scoob_type_enumeration, seqsym_interval);
  enumeration_add_name(scoob_type_enumeration, seqsym_rest);
  enumeration_add_name(scoob_type_enumeration, seqsym_trill);
  enumeration_add_name(scoob_type_enumeration, seqsym_unvoiced);
    
  scoob_class = fts_class_install(seqsym_scoob, scoob_instantiate);
  fts_class_alias(scoob_class, seqsym_note);
}

/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
