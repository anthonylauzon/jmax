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

enum scoob_type_enum
scoob_get_type_from_atom(const fts_atom_t *at)
{
  if(fts_is_int(at))
    {
      int type = fts_get_int(at);

      if(type > 0 && type < n_scoob_types)
	return type;
    }
  else if(fts_is_symbol(at))
  {
    fts_atom_t a;

    if(fts_hashtable_get(&scoob_type_indices, at, &a))
      return fts_get_int(&a);
  }

  return scoob_none;
}

/***********************************************************************
 *
 *  scoob properties
 *
 */

/**
* Method to get or set property.
 */
static void
scoob_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  
  if(ac > 0)
    scoob_property_set(self, s, at);
  else
    scoob_property_get(self, s, fts_get_return_value());
}

int
scoob_declare_property(fts_symbol_t name, fts_symbol_t type)
{
  fts_atom_t k, a;
  
  fts_class_instantiate(scoob_class);
  if(!fts_class_get_method_varargs(scoob_class, name))
  {
    fts_set_symbol(&k, name);
    fts_set_int(&a, scoob_n_properties);
    
    fts_hashtable_put(&scoob_property_indices, &k, &a);
    
    scoob_properties[scoob_n_properties].name = name;
    scoob_properties[scoob_n_properties].type = type;
    
    fts_class_message_varargs(scoob_class, name, scoob_property);
    
    return scoob_n_properties++;
  }
  else
    return -1;
  
}

int
scoob_property_get_index(fts_symbol_t name)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&scoob_property_indices, &k, &a))
    return fts_get_int(&a);
  else
    return -1;
}

void
scoob_property_get_by_index(scoob_t *self, int index, fts_atom_t *p)
{
  if(index < fts_array_get_size(&self->properties))
    /* copy atom to output argument */
    *p = *fts_array_get_element(&self->properties, index);
}

void
scoob_property_get(scoob_t *self, fts_symbol_t name, fts_atom_t *p)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&scoob_property_indices, &k, &a))
    scoob_property_get_by_index(self, fts_get_int(&a), p);
}

void
scoob_property_set_by_index(scoob_t *self, int index, const fts_atom_t *value)
{
  fts_array_set_element(&self->properties, index, value);
}

int
scoob_property_set(scoob_t *self, fts_symbol_t name, const fts_atom_t *value)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&scoob_property_indices, &k, &a))
  {
    scoob_property_set_by_index(self, fts_get_int(&a), value);
    return 1;
  }

  return 0;
}

static void 
scoob_copy(scoob_t *org, scoob_t *copy)
{
  int n_props = fts_array_get_size(&org->properties);
  int i;
  
  copy->type = org->type;
  copy->pitch = org->pitch;
  copy->interval = org->interval;
  copy->duration = org->duration;
  
  fts_array_set_size(&copy->properties, n_props);
  
  for(i=0; i<n_props; i++)
  {
    fts_atom_t *a = fts_array_get_element(&org->properties, i);
    fts_array_set_element(&copy->properties, i, a);
  }
}

static void
scoob_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  scoob_copy((scoob_t *)fts_get_object(from), (scoob_t *)fts_get_object(to));
}

/*
 * some standard properties 
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
scoob_get_property_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_array_t *array = fts_get_pointer(at);
  int i;

  fts_array_append_symbol(array, seqsym_type);
  /*fts_array_append_symbol(array, fts_s_symbol);*/
  fts_array_append_symbol(array, seqsym_enum);
  fts_array_append_int(array, n_scoob_types-1);
  for(i=0; i<n_scoob_types-1; i++)
    fts_array_append_symbol(array, scoob_type_names[i+1]);

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
  scoob_t *self = (scoob_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  int size = fts_array_get_size(&self->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&self->properties);
  int i;

  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, scoob_type_names[self->type]);

  fts_array_append_symbol(array, seqsym_pitch);
  fts_array_append_float(array, self->pitch);

	fts_array_append_symbol(array, seqsym_interval);
	fts_array_append_float(array, self->interval);
  
  fts_array_append_symbol(array, seqsym_duration);
  fts_array_append_float(array, self->duration);

  for(i=0; i<size; i++)
  {
    if(!fts_is_void(atoms + i))
    {
      fts_array_append_symbol(array, scoob_properties[i].name);

      if (fts_is_object(atoms + i)  && !fts_object_has_id(fts_get_object(atoms + i)))
      { /* object has no client-id: upload only string representation 
        todo: prevent string from being edited in table editor */
				fts_symbol_t objdescription = fts_get_class_name(atoms + i);
				fts_array_append_symbol(array, objdescription);
      }
      else
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
  scoob_t *self = (scoob_t *)o;
  enum scoob_type_enum type = scoob_get_type_from_atom(at);

  if(type > scoob_none)
    self->type = type;
}

static void
_scoob_get_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;

  fts_return_symbol(scoob_type_names[self->type]);
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
scoob_remove_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_symbol_t name = fts_get_symbol(at);

  scoob_property_set(self, name, fts_null);
}

static void
scoob_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;

  if(ac > 1 && fts_is_symbol(at))
  {
    switch(ac)
      {
      default:
	if(ac > 3 && fts_is_number(at + 3))
	  _scoob_set_duration(o, 0, 0, 1, at + 3);

      case 3:
	if(fts_is_number(at + 2))
	  _scoob_set_interval(o, 0, 0, 1, at + 2);

      case 2:
	if(fts_is_number(at + 1))
	  _scoob_set_pitch(o, 0, 0, 1, at + 1);

      case 1:
	_scoob_set_type(o, 0, 0, 1, at);

      case 0:
	break;
      }
  }
  else
  {
    /* old note format compatibility */
    self->type = scoob_note;
    
    switch(ac)
    {
      default:
	if(fts_is_number(at + 3))
	  scoob_set_channel(self, fts_get_number_int(at + 3));
        
      case 3:
	if(fts_is_number(at + 2))
	  scoob_set_velocity(self, fts_get_number_int(at + 2));
        
      case 2:
	if(fts_is_number(at + 1))
	  _scoob_set_duration(o, 0, 0, 1, at + 1);
        
      case 1:
	if(fts_is_number(at))
	  _scoob_set_pitch(o, 0, 0, 1, at);
        
      case 0:
	break;
    }
  }  
}

static void
scoob_set_from_scoob(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;

  scoob_copy((scoob_t *)fts_get_object(at), self);
}

static void 
scoob_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);

  switch(self->type)
  {
    case scoob_note:
      fts_tuple_append_symbol(tuple, seqsym_note);
      fts_tuple_append_float(tuple, self->pitch);
      fts_tuple_append_float(tuple, self->duration);
      break;
    case scoob_interval:
      fts_tuple_append_symbol(tuple, seqsym_interval);
      fts_tuple_append_float(tuple, self->pitch);
      fts_tuple_append_float(tuple, self->interval);
      fts_tuple_append_float(tuple, self->duration);
      break;
    case scoob_rest:
      fts_tuple_append_symbol(tuple, seqsym_rest);
      fts_tuple_append_float(tuple, self->duration);
      break;
    case scoob_trill:
      fts_tuple_append_symbol(tuple, seqsym_trill);
      fts_tuple_append_float(tuple, self->pitch);
      fts_tuple_append_float(tuple, self->interval);
      fts_tuple_append_float(tuple, self->duration);
      break;
    default:
      break;
  }

  fts_return_object((fts_object_t *)tuple);
}

static void 
scoob_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int i;
  
  switch(self->type)
  {
    case scoob_note:
      fts_spost(stream, "<scoob note %g %g", self->pitch, self->duration);
      break;
    case scoob_interval:
      fts_spost(stream, "<scoob interval %g %g %g", self->pitch, self->interval, self->duration);
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

  for(i=0; i<fts_array_get_size(&self->properties); i++)
  {
    fts_atom_t *a = fts_array_get_element(&self->properties, i);
    
    if(!fts_is_void(a))
    {
      fts_spost(stream, ", %s: ", fts_symbol_name(scoob_properties[i].name));
      fts_spost_atoms(stream, 1, a);
    }
  }

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
  fts_message_t *mess;
  int i;

  /* send set message with type, pitch, interval, and duration */
  mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_message_append_symbol(mess, scoob_type_names[self->type]);
  fts_message_append_float(mess, self->pitch);
  fts_message_append_float(mess, self->interval);
  fts_message_append_float(mess, self->duration);
  fts_dumper_message_send(dumper, mess);

  /* send a message for each of the dynamic properties */
  for(i=0; i<fts_array_get_size(&self->properties); i++)
  {
    fts_atom_t *a = fts_array_get_element(&self->properties, i);

    if(!fts_is_void(a))
      fts_dumper_send(dumper, scoob_properties[i].name, 1, a);
  }
}

static int
scoob_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  scoob_t *o = (scoob_t *)fts_get_object(a);
  scoob_t *p = (scoob_t *)fts_get_object(b);

  if(o->type == p->type &&
     o->pitch == p->pitch &&
     o->interval == p->pitch &&
     o->duration == p->duration)
  {
    int o_n_prop = fts_array_get_size(&o->properties);
    int p_n_prop = fts_array_get_size(&p->properties);
    int i;

    /* send a message for each of the dynamic properties */
    for(i=0; i<o_n_prop; i++)
    {
      fts_atom_t *o_prop = fts_array_get_element(&o->properties, i);

      if(i < p_n_prop)
      {
        if(!fts_atom_equals(o_prop, fts_array_get_element(&p->properties, i)))
          return 0;
      }
      else if(!fts_is_void(o_prop))
        return 0;
    }

    for(; i<p_n_prop; i++)
      if(!fts_is_void(fts_array_get_element(&p->properties, i)))
        return 0;

    return 1;
  }

  return 0;
}

static void
scoob_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;

  self->type = scoob_note;
  self->pitch = scoob_DEF_PITCH;
  self->interval = 0;
  self->duration = scoob_DEF_DURATION;

  fts_array_init(&self->properties, 0, 0);
  scoob_set(o, 0, 0, ac, at);
}

static void
scoob_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scoob_t *self = (scoob_t *)o;

  fts_array_destroy(&self->properties);
}

static void
scoob_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scoob_t), scoob_init, scoob_delete);

  fts_class_message_varargs(cl, fts_s_dump_state, scoob_dump_state);

  fts_class_message_varargs(cl, fts_s_get_tuple, scoob_get_tuple);
  fts_class_message_varargs(cl, fts_s_set, scoob_set);
  fts_class_message(cl, fts_s_set, cl, scoob_set_from_scoob);

  fts_class_message_varargs(cl, fts_s_post, scoob_post);
  fts_class_message_varargs(cl, fts_s_print, scoob_print);

  fts_class_message_varargs(cl, seqsym_get_property_list, scoob_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, scoob_append_properties);

  fts_class_set_equals_function(cl, scoob_equals);
  
  fts_class_message_number(cl, seqsym_type, _scoob_set_type);
  fts_class_message_symbol(cl, seqsym_type, _scoob_set_type);
  fts_class_message_number(cl, seqsym_pitch, _scoob_set_pitch);
  fts_class_message_number(cl, seqsym_interval, _scoob_set_interval);
  fts_class_message_number(cl, seqsym_duration, _scoob_set_duration);

  fts_class_message_void(cl, seqsym_type, _scoob_get_type);
  fts_class_message_void(cl, seqsym_pitch, _scoob_get_pitch);
  fts_class_message_void(cl, seqsym_interval, _scoob_get_interval);
  fts_class_message_void(cl, seqsym_duration, _scoob_get_duration);

  fts_class_message_symbol(cl, fts_s_remove, scoob_remove_property);

  fts_class_set_copy_function(cl, scoob_copy_function);

  /* builtin properties */
  scoob_declare_property(seqsym_velocity, fts_s_int); /* property 0 */
  scoob_declare_property(seqsym_channel, fts_s_int); /* property 1 */
  scoob_declare_property(seqsym_cue, fts_s_int); /* property 2 */

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
  fts_hashtable_init(&scoob_property_indices, FTS_HASHTABLE_SMALL);
  
  for(i=0; i<n_scoob_types; i++)
  {
    fts_atom_t k, a;

    fts_set_symbol(&k, scoob_type_names[i]);
    fts_set_int(&a, i);
    fts_hashtable_put(&scoob_type_indices, &k, &a);
  }
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
