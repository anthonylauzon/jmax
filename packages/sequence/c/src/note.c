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

static void
_scoob_set_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_symbol_t type = fts_get_symbol(at);
  
  if(enumeration_get_index(scoob_type_enumeration, type) >= 0)
    self->type = type;
}

static void
_scoob_get_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_return_symbol(self->type);
}

static void
_scoob_set_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;  
  double pitch = fts_get_number_float(at);
  
  if(pitch < 0.0)
    pitch = 0.0;
  
  self->pitch = pitch;
}

static void
_scoob_get_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_return_float(self->pitch);
}

static void
_scoob_set_interval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  double interval = fts_get_number_float(at);
  
  if(interval < 0.0)
    interval = 0.0;
  
  self->interval = interval;
}

static void
_scoob_get_interval(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_return_float(self->interval);
}	

static void
_scoob_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  double duration = fts_get_number_float(at);
  
  if(duration < 0.0)
    duration = 0.0;
  
  self->duration = duration;
}

static void
_scoob_get_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  fts_return_float(self->duration);
}

static void
scoob_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
  {
    default:
      if(fts_is_number(at + 3))
        _scoob_set_duration(o, 0, 0, 1, at + 3);
      
    case 3:
      if(fts_is_number(at + 2))
        _scoob_set_interval(o, 0, 0, 1, at + 2);
      
    case 2:
      if(fts_is_number(at + 1))
        _scoob_set_pitch(o, 0, 0, 1, at + 1);
      
    case 1:
      if(fts_is_symbol(at))
        _scoob_set_type(o, 0, 0, 1, at);
      
    case 0:
      break;
  }
}

static void
scoob_set_from_scoob(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  scoob_copy((scoob_t *)fts_get_object(at), self);
}

/*
 * MIDI properties 
 */
void
scoob_set_velocity(scoob_t *self, int velocity)
{
  fts_atom_t a;
  
  fts_set_int(&a, velocity);
  scoob_property_set_by_index(self, 0, &a);
}

int
scoob_get_velocity(scoob_t *self)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  scoob_property_get_by_index(self, 0, &a);
  
  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

void
scoob_set_channel(scoob_t *self, int channel)
{
  fts_atom_t a;
  
  fts_set_int(&a, channel);
  scoob_property_set_by_index(self, 1, &a);
}

int
scoob_get_channel(scoob_t *self)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  scoob_property_get_by_index(self, 1, &a);
  
  if(!fts_is_void(&a))
    return fts_get_int(&a);
  else
    return 0;
}

static void 
scoob_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  
  fts_tuple_append_symbol(tuple, self->type);
  fts_tuple_append_float(tuple, self->pitch);
  fts_tuple_append_float(tuple, self->interval);
  fts_tuple_append_float(tuple, self->duration);
  
  fts_return_object((fts_object_t *)tuple);
}

static void 
scoob_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  
  switch(enumeration_get_index(scoob_type_enumeration, self->type))
  {
    case scoob_note:
      if(self->interval > 0)
        fts_spost(stream, "<scoob note %g (±%g) %g", self->pitch, self->interval, self->duration);
      else
        fts_spost(stream, "<scoob note %g %g", self->pitch, self->duration);
      break;
    case scoob_interval:
      fts_spost(stream, "<scoob interval %g %s%g %g", self->pitch, (self->interval >= 0)? "+": "", self->interval, self->duration);
      break;
    case scoob_rest:
      fts_spost(stream, "<scoob rest %g", self->duration);
      break;
    case scoob_trill:
      fts_spost(stream, "<scoob trill %g %g %g", self->pitch, self->interval, self->duration);
      break;
    default:
      break;
  }  
  
  propobj_post_properties((propobj_t *)self, stream);
  
  fts_spost(stream, ">", self->pitch, self->duration);
}

static void 
scoob_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_post(o, 0, NULL, 0, NULL);
  fts_post("\n");
}  

static void
scoob_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new(dumper, fts_s_set);
  
  fts_message_append_symbol(mess, self->type);
  fts_message_append_float(mess, self->pitch);
  fts_message_append_float(mess, self->interval);
  fts_message_append_float(mess, self->duration);
  fts_dumper_message_send(dumper, mess);
  
  propobj_dump_properties(o, 0, NULL, ac, at);  
}

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
scoob_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  scoob_copy((scoob_t *)fts_get_object(from), (scoob_t *)fts_get_object(to));
}

static int
scoob_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  scoob_t *o = (scoob_t *)fts_get_object(a);
  scoob_t *p = (scoob_t *)fts_get_object(b);
  
  if(o->type == p->type && o->pitch == p->pitch && o->interval == p->pitch && o->duration == p->duration)
    return propobj_equals(a, b);
  
  return 0;
}

static void
scoob_get_property_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
}

static void
scoob_append_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
}

/**************************************************************************************
 *
 *  scoob class
 *
 */
static void
scoob_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  propobj_init(o);
  
  self->type = seqsym_note;
  self->pitch = 60;
  self->interval = 0;
  self->duration = 100;
  
  scoob_set(o, 0, 0, ac, at);
}

static void
scoob_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  propobj_delete(o);
}

static void
scoob_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scoob_t), scoob_init, scoob_delete);
  
  /* types and properties */
  propobj_class_init(cl);
  
  propobj_class_add_int_property(cl, seqsym_velocity);
  propobj_class_add_int_property(cl, seqsym_channel);
  propobj_class_add_int_property(cl, seqsym_cue);
  
  fts_class_set_copy_function(cl, propobj_copy_function);
  fts_class_set_equals_function(cl, propobj_equals);
  
  fts_class_message_varargs(cl, seqsym_get_property_list, scoob_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, scoob_append_properties);
  
  fts_class_message_varargs(cl, fts_s_get_tuple, scoob_get_tuple);
  fts_class_message_varargs(cl, fts_s_dump_state, scoob_dump_state);
  fts_class_message_varargs(cl, fts_s_post, scoob_post);
  fts_class_message_varargs(cl, fts_s_print, scoob_print);
  
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
  
  fts_class_set_copy_function(cl, scoob_copy_function);
  fts_class_set_equals_function(cl, scoob_equals);
  
  fts_class_doc(cl, seqsym_scoob, "[<'note'|'interval'|'rest'|'trill': type> [<num: pitch> [<num: interval> [<num: duration>]]]]", "score object");
  fts_class_doc(cl, fts_s_set, "[<'note'|'interval'|'rest'|'trill': type> [<num: pitch> [<num: interval> [<num: duration>]]]]", "set sccob");
  fts_class_doc(cl, fts_s_set, "<scoob: other>", "set from scoob instance");
  fts_class_doc(cl, seqsym_type, "[<'note'|'interval'|'rest'|'trill': type>]", "get/set score object type");
  fts_class_doc(cl, seqsym_pitch, "[<num: pitch>]", "get/set pitch as (float) MIDI note number");
  fts_class_doc(cl, seqsym_interval, "[<num: interval>]", "get/set interval in (float) MIDI note numbers");
  fts_class_doc(cl, seqsym_duration, "[<num: duration>]", "get/set duration in msecs");
  fts_class_doc(cl, fts_new_symbol("<property name>"), "[<any: value>]", "get/set additional property");
  fts_class_doc(cl, fts_s_remove, "<sym: property name>", "remove value of given additional property");
  fts_class_doc(cl, fts_s_print, NULL, "print");
}

/**************************************************************************************
 *
 *  scomark, score marker (tempo change, bar, etc.)
 *
 */
fts_class_t *scomark_class = 0;
enumeration_t *scomark_type_enumeration = NULL;

static void
_scomark_set_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  fts_symbol_t type = fts_get_symbol(at);
  
  if(enumeration_get_index(scomark_type_enumeration, type) >= 0)
    self->type = type;
}

static void
_scomark_get_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  fts_return_symbol(self->type);
}

static void
_scomark_set_tempo(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;  
  double tempo = fts_get_number_float(at);
  
  if(tempo < 0.0)
    tempo = 0.0;
  
  self->tempo = tempo;
}

static void
_scomark_get_tempo(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  fts_return_float(self->tempo);
}

static void
scomark_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
  {
    default:
      if(fts_is_number(at + 1))
        _scomark_set_tempo(o, 0, 0, 1, at + 1);
      
    case 1:
      if(fts_is_symbol(at))
        _scomark_set_type(o, 0, 0, 1, at);
      
    case 0:
      break;
  }
}

static void
scomark_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new(dumper, fts_s_set);
  
  fts_message_append_symbol(mess, self->type);
  fts_message_append_float(mess, self->tempo);
  fts_dumper_message_send(dumper, mess);
  
  propobj_dump_properties(o, 0, NULL, ac, at);  
}

static void
scomark_get_property_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_array_t *array = fts_get_pointer(at);
  int n_types = enumeration_get_size(scomark_type_enumeration);
  int i;
  
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, seqsym_enum);
  fts_array_append_int(array, n_types);
  
  for(i=0; i<n_types; i++)
    fts_array_append_symbol(array, enumeration_get_name(scomark_type_enumeration, i));      
  
  fts_array_append_symbol(array, seqsym_tempo);
  fts_array_append_symbol(array, fts_s_float);
  
  propobj_class_append_properties(scomark_class, array);
}

static void
scomark_append_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, self->type);
  
  fts_array_append_symbol(array, seqsym_tempo);
  fts_array_append_float(array, self->tempo);
  
  propobj_append_properties((propobj_t *)self, array);
}

static void
scomark_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  propobj_init(o);
  
  self->type = seqsym_tempo;
  self->tempo = 0.0;
  
  scomark_set(o, 0, 0, ac, at);
}

static void
scomark_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  propobj_delete(o);
}

static void
scomark_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scomark_t), scomark_init, scomark_delete);
  
  /* types and properties */
  propobj_class_init(cl);
  
  propobj_class_add_int_property(cl, seqsym_section);
  
  fts_class_message_varargs(cl, seqsym_get_property_list, scomark_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, scomark_append_properties);
  
  fts_class_message_varargs(cl, fts_s_dump_state, scomark_dump_state);
  
  fts_class_message_symbol(cl, seqsym_type, _scomark_set_type);
  fts_class_message_void(cl, seqsym_type, _scomark_get_type);
  
  fts_class_message_number(cl, seqsym_pitch, _scomark_set_tempo);
  fts_class_message_void(cl, seqsym_pitch, _scomark_get_tempo);
  
  fts_class_message_varargs(cl, fts_s_set, scomark_set);  
}

void
scoob_config(void)
{
  scoob_type_enumeration = enumeration_new(fts_new_symbol("scoob_type"));
  
  enumeration_add_name(scoob_type_enumeration, seqsym_note);
  enumeration_add_name(scoob_type_enumeration, seqsym_interval);
  enumeration_add_name(scoob_type_enumeration, seqsym_rest);
  enumeration_add_name(scoob_type_enumeration, seqsym_trill);
  
  scoob_class = fts_class_install(seqsym_scoob, scoob_instantiate);
  fts_class_alias(scoob_class, seqsym_note);
}

/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
