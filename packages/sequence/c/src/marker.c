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

static fts_symbol_t sym_meter_2_4 = NULL;
static fts_symbol_t sym_meter_3_4 = NULL;
static fts_symbol_t sym_meter_4_4 = NULL;

/**************************************************************************************
*
*  marker track functions
*
*/

static void
marker_track_tempo_changed(track_t * marker_track, scomark_t *scomark, double old_tempo, double new_tempo)
{
  if(new_tempo != 0.0 && old_tempo != 0.0)
  {
    track_t *track = (track_t *)fts_object_get_context((fts_object_t *)marker_track);
    event_t *first_mark_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
    event_t *mark_evt = first_mark_evt;
    double begin = event_get_time(mark_evt);
    double end = 2.0 * track_get_total_duration(track);
    double t = -1.0;
    event_t *first = NULL;
    event_t *after = NULL;
    double stretch = old_tempo/new_tempo;
        
    mark_evt = event_get_next(mark_evt);
    while(mark_evt != NULL && t < 0.0)
    {
      scomark_t *marker = (scomark_t *)fts_get_object( event_get_value(mark_evt));
      scomark_get_tempo(marker, &t);
      mark_evt = event_get_next(mark_evt);
    }
            
    if(mark_evt != NULL)
      end = event_get_time(mark_evt);
        
    /* stretch scoob track */
    track_segment_get(track, begin, end, &first, &after);
    if(first != NULL)
    {
      track_segment_stretch(track, first, after, begin, end, stretch);
      track_move_events_at_client(track, first, NULL);
    }
    
    /* stretch marker track */
    track_segment_stretch(marker_track, first_mark_evt, mark_evt, begin, end, stretch);  
    track_move_events_at_client(marker_track, first_mark_evt, NULL);
  }
}

/**************************************************************************************
 *
 *  scomark, score marker (tempo change, bar, etc.)
 *
 */
fts_class_t *scomark_class = 0;
enumeration_t *scomark_type_enumeration = NULL;

void 
scomark_spost(fts_object_t *o, fts_bytestream_t *stream)
{
  scomark_t *self = (scomark_t *)o;
  
  fts_spost(stream, "%s ", fts_symbol_name(self->type));
  
  propobj_post_properties((propobj_t *)self, stream);
}

int
scomark_meter_symbol_get_quotient(fts_symbol_t sym, int *meter_num, int *meter_den)
{
  const char *str = fts_symbol_name(sym);
  int slash = 0;
  int b = 0;
  int bt = 0;
  int i = 0;
  
  *meter_num = 0;
  *meter_den = 0;

  while(str[i] != '\0' && (str[i] < '0' || str[i] > '9'))
    i++;
  
  while(str[i] != '\0' && str[i] >= '0' && str[i] <= '9')
  {
    b *= 10;
    b += (int)(str[i] - '0');
    i++;
  }
  
  while(str[i] != '\0' && (str[i] < '0' || str[i] > '9'))
  {
    if(str[i] == '/')
      slash = 1;
    
    i++;
  }
  
  if(b == 0 || slash == 0)
    return 0;
  
  while(str[i] != '\0' && str[i] >= '0' && str[i] <= '9')
  {
    bt *= 10;
    bt += (int)(str[i] - '0');
    i++;
  }
  
  if(bt == 0)
    return 0;
  
  *meter_num = b;
  *meter_den = bt;
  
  return 1;
}

fts_symbol_t
scomark_meter_quotient_get_symbol(int meter_num, int meter_den)
{
  if(meter_num > 0 && meter_den > 0)
  {
    char str[64];

    if(meter_den == 4)
    {
      if(meter_num == 2)
        return sym_meter_2_4;
      else if(meter_num == 3)
        return sym_meter_3_4;
      else if(meter_num == 4)
        return sym_meter_4_4;
    }
  
    snprintf(str, 64, "%d/%d", meter_num, meter_den);
  
    return fts_new_symbol(str);
  }
  
  return NULL;
}

void
scomark_set_tempo(scomark_t *self, double tempo)
{
  if(tempo > 0.0)
  {
    fts_atom_t a;
    double old_tempo = 0.0;
    track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));
    scomark_get_tempo(self, &old_tempo);
    
    fts_set_float(&a, tempo);
    propobj_set_property_by_index((propobj_t *)self, scomark_propidx_tempo, &a);  
  
    marker_track_tempo_changed(marker_track, self, old_tempo, tempo);
  }
}
void
scomark_get_tempo(scomark_t *self, double *tempo)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  propobj_get_property_by_index((propobj_t *)self, scomark_propidx_tempo, &a);
  
  if(fts_is_float(&a))
    *tempo = fts_get_float(&a);
}

void
scomark_set_cue(scomark_t *self, int cue)
{
  fts_atom_t a;
  
  if(cue < 0)
    cue = 0;
  
  fts_set_int(&a, cue);
  propobj_set_property_by_index((propobj_t *)self, scomark_propidx_cue, &a);
}

void
scomark_get_cue(scomark_t *self, int *cue)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  propobj_get_property_by_index((propobj_t *)self, scomark_propidx_cue, &a);
  
  if(fts_is_int(&a))
    *cue = fts_get_int(&a);
}

void
scomark_set_label(scomark_t *self, fts_symbol_t label)
{
  fts_atom_t a;
  
  fts_set_symbol(&a, label);
  propobj_set_property_by_index((propobj_t *)self, scomark_propidx_label, &a);
}

void
scomark_get_label(scomark_t *self, fts_symbol_t *label)
{
  fts_atom_t a;
  
  fts_set_void(&a);
  propobj_get_property_by_index((propobj_t *)self, scomark_propidx_label, &a);
  
  if(fts_is_symbol(&a))
    *label = fts_get_symbol(&a);
}

void
scomark_bar_set_number(scomark_t *self, int num)
{
  if(scomark_is_bar(self))
  {
    fts_atom_t a;
    
    if(num < 0)
      num = 0;
    
    fts_set_int(&a, num);
    propobj_set_property_by_index((propobj_t *)self, scomark_propidx_bar_num, &a);
  }
}

void
scomark_bar_get_number(scomark_t *self, int *num)
{
  if(scomark_is_bar(self))
  {
    fts_atom_t a;
    
    fts_set_void(&a);
    propobj_get_property_by_index((propobj_t *)self, scomark_propidx_bar_num, &a);
    
    if(fts_is_int(&a))
      *num = fts_get_int(&a);
  }
}

void 
scomark_bar_set_meter(scomark_t *self, fts_symbol_t meter_sym)
{
  if(scomark_is_bar(self))
  {
    int num = 0;
    int den = 0;
    fts_atom_t a;
    
    scomark_meter_symbol_get_quotient(meter_sym, &num, &den);
    
    if(num > 0 && den > 0)
    {
      fts_set_symbol(&a, meter_sym);
      propobj_set_property_by_index((propobj_t *)self, scomark_propidx_meter, &a);    
    }
  }
}

void
scomark_bar_get_meter(scomark_t *self, fts_symbol_t *meter)
{
  if(scomark_is_bar(self))
  {
    fts_atom_t a;
    
    fts_set_void(&a);
    propobj_get_property_by_index((propobj_t *)self, scomark_propidx_meter, &a);
    
    if(fts_is_symbol(&a))
      *meter = fts_get_symbol(&a);
  }
}

void 
scomark_bar_set_meter_quotient(scomark_t *self, int meter_num, int meter_den)
{
  if(scomark_is_bar(self))
  {
    fts_symbol_t meter = scomark_meter_quotient_get_symbol(meter_num, meter_den);
    
    if(meter != NULL)
      scomark_bar_set_meter(self, meter);
  }
}

void 
scomark_bar_get_meter_quotient(scomark_t *self, int *meter_num, int *meter_den)
{
  if(scomark_is_bar(self))
  {
    fts_symbol_t meter = NULL;
    
    scomark_bar_get_meter(self, &meter);
    
    if(meter != NULL)
      scomark_meter_symbol_get_quotient(meter, meter_num, meter_den);
    else
      *meter_num = *meter_den = 0;
  }
}

/************************************************************
 *
 *  methods
 *
 */
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
  scomark_set_tempo(self, fts_get_number_float(at));
}

static void
_scomark_remove_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  propobj_remove_property(o, 0, NULL, ac, at);
}

static void
_scomark_get_property_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_array_t *array = fts_get_pointer(at);
  int n_types = enumeration_get_size(scomark_type_enumeration);
  int i;
  
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, seqsym_enum);
  fts_array_append_int(array, n_types);
  
  for(i=0; i<n_types; i++)
    fts_array_append_symbol(array, enumeration_get_name(scomark_type_enumeration, i));      
    
  propobj_class_append_properties(scomark_class, array);
}

static void
_scomark_append_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  fts_array_t *array = fts_get_pointer(at);
    
  fts_array_append_symbol(array, seqsym_type);
  fts_array_append_symbol(array, self->type);
  
  propobj_append_properties((propobj_t *)self, array);
}

static void 
scomark_array_function(fts_object_t *o, fts_array_t *array)
{
  scomark_t *self = (scomark_t *)o;
  
  fts_array_append_symbol(array, self->type);
  propobj_append_properties((propobj_t *)self, array);
}

static void 
scomark_description_function(fts_object_t *o, fts_array_t *array)
{
  scomark_t *self = (scomark_t *)o;
  
  fts_array_append_symbol(array, seqsym_scomark);
  scomark_array_function(o, array);
}

static void
scomark_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  propobj_init(o);  
  self->type = seqsym_marker;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t type = fts_get_symbol(at);
    
    if(enumeration_get_index(scomark_type_enumeration, type) >= 0)
      self->type = type;
  }
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
  
  /* object with properties class */
  propobj_class_init(cl);
  
  /* properties of any marker */
  propobj_class_add_float_property(cl, seqsym_tempo, _scomark_set_tempo); /* scomark_propidx_tempo = 0 */
  propobj_class_add_int_property(cl, seqsym_cue, NULL); /* scomark_propidx_cue = 1 */
  propobj_class_add_symbol_property(cl, fts_s_label, NULL); /* scomark_propidx_label = 2 */
  
  /* properties for bars only */
  propobj_class_add_int_property(cl, seqsym_bar_num, NULL); /* scomark_propidx_bar_num = 3 */
  propobj_class_add_symbol_property(cl, seqsym_meter, NULL); /* scomark_propidx_meter = 4 */
  
  fts_class_set_array_function(cl, scomark_array_function);
  fts_class_set_description_function(cl, scomark_description_function);

  fts_class_message_symbol(cl, fts_s_remove, _scomark_remove_property);

  fts_class_message_varargs(cl, seqsym_get_property_list, _scomark_get_property_list);
  fts_class_message_varargs(cl, seqsym_append_properties, _scomark_append_properties);
    
  /*fts_class_message_void(cl, seqsym_type, _scomark_get_type);*/
}

void
scomark_config(void)
{
  scomark_type_enumeration = enumeration_new(fts_new_symbol("scomark_type"));

  enumeration_add_name(scomark_type_enumeration, seqsym_bar);
  enumeration_add_name(scomark_type_enumeration, seqsym_marker);
  
  sym_meter_2_4 = fts_new_symbol("2/4");
  sym_meter_3_4 = fts_new_symbol("3/4");
  sym_meter_4_4 = fts_new_symbol("4/4");
  
  scomark_class = fts_class_install(seqsym_scomark, scomark_instantiate);
}

/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
