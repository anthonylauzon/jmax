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
static fts_symbol_t sym_meter_empty = NULL;

static scomark_t *marker_track_get_previous_tempo(track_t *marker_track, scomark_t *scomark, double *tempo);         
static scomark_t *marker_track_get_previous_meter(track_t *marker_track, scomark_t *scomark,fts_symbol_t *meter);
static scomark_t *marker_track_get_next_bar(track_t *marker_track, scomark_t *scomark);
static scomark_t *marker_track_get_next_bar_by_time(track_t *marker_track, scomark_t *start, double time);
static void marker_track_tempo_changed(track_t *marker_track, scomark_t *scomark, double old_tempo, double new_tempo, int upload);
static void marker_track_meter_changed(track_t * marker_track, scomark_t *scomark, fts_symbol_t old_meter, fts_symbol_t new_meter, int upload);
static double get_next_bar_time( event_t *last_bar, fts_symbol_t last_meter, double last_tempo, event_t **event_at_same_time);



/******************************************************************************
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
scomark_set_tempo(scomark_t *self, double tempo, double *old_tempo)
{
  if(tempo > 0.0)
  {
    fts_atom_t a;
    track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));
    scomark_get_tempo(self, old_tempo);
    
    if(*old_tempo == 0.0)
      marker_track_get_previous_tempo(marker_track, self, old_tempo);
      
    fts_set_float(&a, tempo);
    propobj_set_property_by_index((propobj_t *)self, scomark_propidx_tempo, &a);  
  }
}

void
scomark_unset_tempo(scomark_t *self)
{
  double new_tempo = 0.0;
  double old_tempo = 0.0;
  scomark_t *prev;

  track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));
  
  scomark_get_tempo(self, &old_tempo);
  if(old_tempo > 0.0)
  {
    prev = marker_track_get_previous_tempo(marker_track, self, &new_tempo);

    if(new_tempo > 0.0)
      marker_track_tempo_changed(marker_track, self, old_tempo, new_tempo, 1);
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
scomark_bar_set_meter(scomark_t *self, fts_symbol_t meter_sym, fts_symbol_t *old_meter)
{
  if(scomark_is_bar(self))
  {
    int num = 0;
    int den = 0;
    int set_ok = 0;
    fts_atom_t a;
    
    track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));
    scomark_bar_get_meter(self, old_meter);
    
    if(*old_meter == NULL)
      marker_track_get_previous_meter(marker_track, self, old_meter);
        
    if(meter_sym == sym_meter_empty)
      set_ok = 1;
    else
    {
      scomark_meter_symbol_get_quotient(meter_sym, &num, &den);
      if(num > 0 && den > 0) set_ok = 1;
    }
    if(set_ok)
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
    fts_symbol_t old_meter;
        
    if(meter != NULL)
      scomark_bar_set_meter(self, meter, &old_meter);
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

/* set or get type */
static void
_scomark_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *) o;
  
  if (ac > 0)
  {
    if (fts_is_symbol(at)  &&  (fts_get_symbol(at) == seqsym_marker ||
                                fts_get_symbol(at) == seqsym_bar))
    { 
      self->type = fts_get_symbol(at);
    }
    else
    {
      fts_return_symbol(self->type);
    }
  }
}

static void
_scomark_set_tempo(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  double old_tempo = 0.0;
  scomark_set_tempo(self, fts_get_number_float(at), &old_tempo);
}

static void
_scomark_set_tempo_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  double old_tempo = 0.0;
  double tempo = fts_get_number_float(at);
  track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));    
  
  scomark_set_tempo(self, tempo, &old_tempo);
  
  marker_track_tempo_changed(marker_track, self, old_tempo, tempo, 1);
}

static void
_scomark_set_meter_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  if( scomark_is_bar(self) && ac==1 && fts_is_symbol(at))
  {
    track_t * marker_track = (track_t *)fts_object_get_context((fts_object_t *)fts_object_get_context((fts_object_t *)self));   
    fts_symbol_t meter = fts_get_symbol(at);
    fts_symbol_t old_meter = NULL;
        
    scomark_bar_set_meter(self, meter, &old_meter);
    
    marker_track_meter_changed(marker_track, self, old_meter, meter, 1);
  }
}
static void
_scomark_remove_property(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  if( fts_get_symbol(at) == seqsym_tempo)
    scomark_unset_tempo(self);
    
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

void
scomark_set_properties(scomark_t *self, int ac, const fts_atom_t *at)
{
  int i;
  
  switch(ac)
  {
    default:
      for(i=1; i<ac-1; i+=2)
      {  
        if(fts_is_symbol(at + i))
          propobj_set_property_by_name((propobj_t *)self, fts_get_symbol(at + i), at + i + 1);
      }
      
    case 1:
      if(fts_is_symbol(at))
      {
        fts_symbol_t type = fts_get_symbol(at);
        
        if(enumeration_get_index(scomark_type_enumeration, type) >= 0)
          self->type = type;
      }
      
    case 0:
      break;
  }
}


static void
scomark_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scomark_t *self = (scomark_t *)o;
  
  propobj_init(o);  
  self->type = seqsym_marker;

  if(ac > 0)
    scomark_set_properties(self, ac, at);
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

  fts_class_message_void  (cl, fts_s_type, _scomark_type);      /* get type */
  fts_class_message_symbol(cl, fts_s_type, _scomark_type);      /* set type */
  fts_class_message_varargs(cl, fts_new_symbol("tempo_change"), _scomark_set_tempo_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("meter_change"), _scomark_set_meter_from_client);
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
  sym_meter_empty = fts_new_symbol("*");
  
  scomark_class = fts_class_install(seqsym_scomark, scomark_instantiate);
}

/******************************************************************************
*
*  marker track functions
*
*/

static scomark_t *
marker_track_get_previous_meter(track_t *marker_track, scomark_t *scomark, fts_symbol_t *meter)
{
  event_t *mark_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
  event_t *prev = event_get_prev(mark_evt);
  scomark_t *marker;
        
  while(prev != NULL && *meter == NULL)
  {
    marker = (scomark_t *)fts_get_object( event_get_value(prev));
    if(scomark_is_bar(marker))
      scomark_bar_get_meter(marker, meter);
    prev = event_get_prev(prev);
  }
  
  if(prev != NULL)
    return marker;
  else
    return NULL;
}

static scomark_t *
marker_track_get_next_bar_by_time(track_t *marker_track, scomark_t *start, double time)
{
  event_t *start_evt = NULL;
  event_t *next_evt = NULL;
  scomark_t *next_marker = NULL;

  if(start != NULL)
    start_evt = (event_t *)fts_object_get_context((fts_object_t *)start);    
  else
    start_evt = track_get_first(marker_track);
  
  next_evt = event_get_next(start_evt);
  
  while(next_evt != NULL)
  {
    next_marker = (scomark_t *)fts_get_object( event_get_value(next_evt));
    if(scomark_is_bar(next_marker) && event_get_time(next_evt) > time)
      break;
    next_evt = event_get_next(next_evt);
  }
  if(next_evt != NULL)
    return next_marker;
  else
    return NULL;
}


static scomark_t *
marker_track_get_next_bar(track_t *marker_track, scomark_t *scomark)
{
  event_t *mark_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
  event_t *next_evt = event_get_next(mark_evt);
  scomark_t *next_marker = NULL;
  
  while(next_evt != NULL)
  {
    next_marker = (scomark_t *)fts_get_object( event_get_value(next_evt));
    if(scomark_is_bar(next_marker))
      break;
    next_evt = event_get_next(next_evt);
  }
  if(next_evt != NULL)
    return next_marker;
  else
    return NULL;
}

static void
marker_track_meter_changed(track_t * marker_track, scomark_t *scomark, fts_symbol_t old_meter, fts_symbol_t new_meter, int upload)
{  
  if(new_meter != NULL && old_meter != NULL)
  {
    track_t *track = (track_t *)fts_object_get_context((fts_object_t *)marker_track);
    event_t *marker_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
    scomark_t *next_bar = marker_track_get_next_bar(marker_track, scomark);
    
    if(new_meter == sym_meter_empty)
    {
      fts_symbol_t meter = NULL;
      scomark_bar_get_meter(next_bar, &meter);
      if(meter == NULL) 
      {
        scomark_bar_set_meter( next_bar, old_meter, &meter);
        event_set_at_client((event_t *)fts_object_get_context((fts_object_t *)next_bar));
      }              
    }
    else
    {
      double next_bar_time = -1.0;
      event_t *next_evt = NULL;
    
      if(next_bar != NULL)
      {
        fts_symbol_t meter = NULL;
        double next_time = -1.0;
        double bar_duration = 0.0;
        double tempo = -1.0;
        int numerator = 0;
        int denominator = 0;
        int last_number = -1;
        scomark_t *new_bar = NULL;
        event_t *new_event = NULL;
        
        /* get timetag of actual next bar */
        next_evt = (event_t *)fts_object_get_context((fts_object_t *)next_bar);    
        next_bar_time = event_get_time(next_evt);      
    
        /* calculate timetag of future next bar  */
        scomark_get_tempo(scomark, &tempo);
        if(tempo < 0.0)
          marker_track_get_previous_tempo(marker_track, scomark, &tempo);
        
        scomark_meter_symbol_get_quotient(new_meter, &numerator, &denominator);
        bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
        next_time = event_get_time(marker_evt) + bar_duration; 
        
        /* is very near to the next event so nothing to do */
        if(next_time < next_bar_time + MARKERS_BAR_TOLERANCE && next_time > next_bar_time - MARKERS_BAR_TOLERANCE)
          return;
      
        scomark_bar_get_number( scomark, &last_number);
        
        /* FIRST CASE: new_bar is before next_bar */
        if(next_time < next_bar_time - MARKERS_BAR_TOLERANCE)
        {
          /* insert enough new bars */
          while(next_time < next_bar_time - MARKERS_BAR_TOLERANCE)
          {
            new_bar = marker_track_insert_marker(marker_track, next_time, seqsym_bar, &new_event);     
            scomark_bar_set_number( new_bar, ++last_number);
            next_time = next_time + bar_duration;
            
            track_upload_event(marker_track, new_event);
          }
          /* renumber bars after inserction */
          marker_track_renumber_bars(marker_track, next_evt, last_number+1, 1);
          /* set meter of next bar */
          scomark_bar_get_meter(next_bar, &meter);
          if(meter == NULL) 
          {
            scomark_bar_set_meter( next_bar, old_meter, &meter);
            event_set_at_client((event_t *)fts_object_get_context((fts_object_t *)next_bar));
          }        
          /* last bar (if not == to next_bar) will have free metrics */
          if(next_time > next_bar_time + MARKERS_BAR_TOLERANCE)
          {
            scomark_bar_set_meter( new_bar, sym_meter_empty, &old_meter);
            event_set_at_client(new_event);
          }
        }
        else /* SECOND CASE: new_bar is after next_bar */
        {
          fts_atom_t a[256];
          int to_remove = 0;
          int i = 0;
          event_t *stop_evt = NULL;
          event_t *next_evt = NULL;
          fts_symbol_t last_removed_meter = NULL;

          /* remove bars between current_bar and the one after next_time */
          scomark_t *stop_bar = marker_track_get_next_bar_by_time(marker_track, next_bar, next_time);
          if(stop_bar != NULL)
            stop_evt = (event_t *)fts_object_get_context((fts_object_t *)stop_bar); 
        
          /* find bars to be removed */
          next_evt = event_get_next(marker_evt); 
          while(next_evt != stop_evt && next_evt != NULL)
          {
            scomark_t *next_scomark = (scomark_t *)fts_get_object( event_get_value(next_evt));
            if( scomark_is_bar(next_scomark))
            {
              fts_symbol_t meter = NULL;        
              scomark_bar_get_meter(next_scomark, &meter);
              if(meter != NULL) last_removed_meter = meter;
                                                        
              fts_set_object(a + to_remove, next_evt);
              to_remove++;
            }
            next_evt = event_get_next(next_evt); 
          }
          /* remove bars */        
          marker_track_unset_tempo_on_selection(marker_track, to_remove, a);
          fts_client_send_message((fts_object_t *)marker_track, seqsym_removeEvents, to_remove, a);
          for(i=0; i<to_remove; i++)
          {
            fts_object_t *event = fts_get_object(a + i);
            track_remove_event(marker_track, (event_t *)event);
          }         
        
          /* insert new bar at next_time */
          new_bar = marker_track_insert_marker(marker_track, next_time, seqsym_bar, &new_event); 
          scomark_bar_set_number( new_bar, ++last_number);
          scomark_bar_set_meter( new_bar, sym_meter_empty, &old_meter);
          track_upload_event(marker_track, new_event);
          
          /* renumber bars after inserction */
          marker_track_renumber_bars(marker_track, stop_evt, last_number+1, 1);
          
          /* set meter of next bar */
          if(stop_bar != NULL)
          {
            scomark_bar_get_meter(stop_bar, &meter);
            if(meter == NULL) 
            {
              if(last_removed_meter != NULL)
                scomark_bar_set_meter( stop_bar, last_removed_meter, &meter);
                  else  
                scomark_bar_set_meter( stop_bar, old_meter, &meter);
                                                        
              event_set_at_client((event_t *)fts_object_get_context((fts_object_t *)stop_bar));
            }   
          }
        }
      }
    }
  }
}

static void
marker_track_tempo_changed(track_t * marker_track, scomark_t *scomark, double old_tempo, double new_tempo, int upload)
{  
  if(new_tempo != 0.0 && old_tempo != 0.0)
  {
    track_t *track = (track_t *)fts_object_get_context((fts_object_t *)marker_track);
    event_t *first_mark_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
    event_t *mark_evt = first_mark_evt;
    double begin = event_get_time(mark_evt);
    double end = 2.0 * track_get_duration(track);
    double t = -1.0;
    event_t *first = NULL;
    event_t *after = NULL;
    double stretch = old_tempo/new_tempo;
    scomark_t *marker = NULL;
    
    mark_evt = event_get_next(mark_evt);
    if(mark_evt != NULL)
    {
      scomark_get_tempo((scomark_t *)fts_get_object( event_get_value(mark_evt)), &t);
    
      while(mark_evt != NULL && t < 0.0)
      {
        scomark_get_tempo((scomark_t *)fts_get_object( event_get_value(mark_evt)), &t);
        mark_evt = event_get_next(mark_evt);
      }
    
      if(mark_evt != NULL)
        end = event_get_time(mark_evt);
    }
    /* stretch scoob track */
    track_segment_get(track, begin, end, &first, &after);
    if(first != NULL)
    {
      track_segment_stretch(track, first, after, begin, end, stretch);
      if(upload)
        track_move_events_at_client(track, first, NULL);
    }
    
    /* stretch marker track */
    track_segment_stretch(marker_track, first_mark_evt, mark_evt, begin, end, stretch);  
    if(upload)
      track_move_events_at_client(marker_track, first_mark_evt, NULL);
  }
}

/***************************************************************************
*  unset tempo on selection of scomarks: stretch track to previous tempo if any
*****************************/

static scomark_t *
marker_track_get_previous_tempo(track_t *marker_track, scomark_t *scomark, double *tempo)
{
  event_t *mark_evt = (event_t *)fts_object_get_context((fts_object_t *)scomark);    
  event_t *prev = event_get_prev(mark_evt);
  scomark_t *marker = NULL;
  
  while(prev != NULL && *tempo <= 0.0)
  {
    marker = (scomark_t *)fts_get_object( event_get_value(prev));
    scomark_get_tempo(marker, tempo);
    prev = event_get_prev(prev);
  }
  
  if(prev != NULL)
    return marker;
  else
    return NULL;
}

void
marker_track_unset_tempo_on_selection(track_t *marker_track, int ac, const fts_atom_t *at)
{
  int i;
  event_t *first = NULL;
  event_t *last = NULL;
  event_t *evt = NULL;
  double last_time, first_time, evt_time;
  double new_tempo = 0.0;
  
  if(ac == 1)
  {
    scomark_t *marker;
    double old_tempo = 0.0;
    first = (event_t *)fts_get_object(at);
    marker = (scomark_t *)fts_get_object( event_get_value(first));
    
    scomark_get_tempo(marker, &old_tempo);
    if(old_tempo > 0.0)
    {
      marker_track_get_previous_tempo(marker_track, marker, &new_tempo);
      if(new_tempo > 0.0)
        marker_track_tempo_changed(marker_track, marker, old_tempo, new_tempo, 1);
    }
  }
  else if(ac > 1)
  {
    int upload = 0;
    /* search first and last object in selection */
    first = (event_t *)fts_get_object(at);
    last = first;
    last_time = first_time = event_get_time(last);
    for(i=1; i<ac; i++)
    {
      evt = ((event_t *)fts_get_object(at + i));
      evt_time = event_get_time(evt);
      if(evt_time > last_time) 
      {
        last = evt;
        last_time = evt_time;
      }
      else
        if(evt_time < first_time)
        {
          first = evt;
          first_time = evt_time;
        }
    }
    /* find previous tempo in track: that will be the new tempo in selection */
    marker_track_get_previous_tempo(marker_track, (scomark_t *)fts_get_object( event_get_value(first)), &new_tempo);
    
    /* change tempo to new_tempo in selection */
    if(new_tempo > 0.0)
    {
      scomark_t *marker;
      event_t *prev = NULL;
      track_t *track = (track_t *)fts_object_get_context((fts_object_t *)marker_track);
      
      prev = last;
      while( prev != NULL && prev!=first)
      {
        double old_tempo = 0.0;
        marker = (scomark_t *)fts_get_object( event_get_value(prev));
        scomark_get_tempo(marker, &old_tempo);
        if( old_tempo > 0.0)
        {
          marker_track_tempo_changed(marker_track, marker, old_tempo, new_tempo, 0);
          upload = 1;
        }
        prev = event_get_prev(prev);
      }   
      // upload track
      if(upload)
      {
        track_move_events_at_client(marker_track, first, NULL);
        track_segment_get(track, first_time, last_time, &first, &last);
        track_move_events_at_client(track, first, NULL);
      }
    }
  }
}

void
marker_track_dump_state(track_t *self, fts_dumper_t *dumper)
{
  event_t *event = track_get_first(self);
  
  /* save markers */
  while(event)
  { 
    fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_marker);
    fts_atom_t *value = event_get_value(event);
    fts_object_t *marker = fts_get_object(value);
    
    fts_message_append_float(mess, event_get_time(event));
    scomark_array_function(marker, fts_message_get_args(mess));
    fts_dumper_message_send(dumper, mess);
    
    event = event_get_next(event);
  }
}

static scomark_t *
marker_track_create_marker(track_t *marker_track, fts_symbol_t type, event_t **event)
{
  fts_atom_t a;
  scomark_t *scomark;
  
  if(type != NULL)
  {
    fts_set_symbol(&a, type);
    scomark = (scomark_t *)fts_object_create(scomark_class, 1, &a);
  }
  else
    scomark = (scomark_t *)fts_object_create(scomark_class, 0,  NULL);  
  
  /* create a new event with the scomark */
  fts_set_object(&a, (fts_object_t *)scomark);
  *event = (event_t *)fts_object_create(event_class, 1, &a);
  
  /* point back to event and marker track */
  fts_object_set_context((fts_object_t *)(*event), (fts_context_t *)marker_track);
  fts_object_set_context((fts_object_t *)scomark, (fts_context_t *)(*event));
  
  return scomark;
}

scomark_t *
marker_track_append_marker(track_t *marker_track, double time, int ac, const fts_atom_t *at, event_t **event)
{
  scomark_t *scomark = marker_track_create_marker(marker_track, NULL, event);
  
  scomark_set_properties(scomark, ac, at);
  track_append_event(marker_track, time, *event);
  
  return scomark;
}

scomark_t *
marker_track_insert_marker(track_t *marker_track, double time, fts_symbol_t type, event_t **event)
{
  scomark_t *scomark = marker_track_create_marker(marker_track, type, event);
    
  track_add_event(marker_track, time, *event);
  
  return scomark;
}

void 
marker_track_renumber_bars(track_t *marker_track, event_t *start, int start_num, int upload)
{  
  int num = start_num;
  event_t *event = start;
  if(event == NULL) event = track_get_first(marker_track);
  
  while(event)
  { 
    scomark_t *marker = (scomark_t *)fts_get_object( event_get_value(event));
    if(marker->type == seqsym_bar)
    {
      scomark_bar_set_number(marker, num++);
      if(upload) event_set_at_client(event);
    }
    event = event_get_next(event);
  }  
}

#define MARKERS_BAR_EPSILON 0.001

static double 
get_next_bar_time( event_t *last_bar, fts_symbol_t last_meter, double last_tempo, event_t **event_at_same_time)
{
  int numerator = 0;
  int denominator = 0;
  double bar_duration = 0.0;
  double next_bar_time = 0.0;
  event_t *marker_event = NULL;
        
  scomark_meter_symbol_get_quotient(last_meter, &numerator, &denominator);
        
  bar_duration = ((double)numerator * 240000.0) / (last_tempo * (double)denominator);
  next_bar_time = event_get_time(last_bar) + bar_duration;
        
  marker_event = event_get_next(last_bar);
  if(marker_event != NULL) 
  {
    double time = event_get_time(marker_event);
    double tempo = last_tempo;      
    while(marker_event != NULL && time < next_bar_time + MARKERS_BAR_EPSILON)
    {
      double old_tempo = tempo;
      scomark_t *scomark = (scomark_t *)fts_get_object(event_get_value(marker_event));        
      scomark_get_tempo(scomark, &tempo);
                                        
      /* recompute bar duration and time of next bar from new tempo */
      if(tempo != old_tempo)
      {
        bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
        next_bar_time = time + old_tempo * (next_bar_time - time) / tempo;
      }
                                        
      /* founded marker falling on next bar time */
      if((next_bar_time <= time + MARKERS_BAR_EPSILON) &&
         (next_bar_time >= time - MARKERS_BAR_EPSILON))
      {
        *event_at_same_time = marker_event;
        return next_bar_time;        
      }
                                        
      /* advance to next marker */
      marker_event = event_get_next(marker_event);
      if(marker_event != NULL)
        time = event_get_time(marker_event);
    }
  }
  return next_bar_time;
}

static event_t *
marker_track_append_last_bar(track_t *marker_track, event_t *last_bar, fts_symbol_t last_meter, double last_tempo)
{
  event_t *event_at_same_time = NULL;
  scomark_t *new_bar = NULL;
  event_t *new_event = NULL;
  fts_symbol_t meter = NULL;
  fts_symbol_t old_meter = NULL;
  scomark_t *last_scomark_bar = (scomark_t *)fts_get_object(event_get_value(last_bar));
        
  double next_bar_time = get_next_bar_time(last_bar, last_meter, last_tempo, &event_at_same_time);
  if(event_at_same_time != NULL) /* a marker is at the same time of new bar: transform the marker in bar*/
  {
    fts_atom_t a;
    new_bar = (scomark_t *)fts_get_object(event_get_value(event_at_same_time));
    scomark_set_type( new_bar, seqsym_bar);
    new_event = event_at_same_time; 
  }
  else
    new_bar = marker_track_insert_marker(marker_track, next_bar_time, seqsym_bar, &new_event);     

  /*unset meter on last bar*/
  scomark_bar_get_meter(last_scomark_bar, &meter);
  if(meter == sym_meter_empty)
    event_unset_property(last_bar, seqsym_meter);                       
  /*set empty_meter on last bar*/
  scomark_bar_set_meter( new_bar, sym_meter_empty, &old_meter);

  marker_track_renumber_bars(marker_track, NULL, FIRST_BAR_NUMBER, 0);
  return new_event;
}

event_t*
marker_track_append_bar(track_t *marker_track, event_t *start_evt)
{
  event_t *new_event = NULL;
  if(marker_track != NULL)
  {
    event_t *marker_event = NULL;
    event_t *last_bar = NULL;
    scomark_t *last_scomark_bar = NULL;
    scomark_t *scomark = NULL;
    scomark_t *new_bar = NULL;
    fts_symbol_t last_meter = NULL;
    fts_symbol_t meter = NULL;
    fts_symbol_t old_meter = NULL;
    int last_number = 0;
    int numerator = 0;
    int denominator = 0;
    double last_tempo = 0.0;
    double tempo = 0.0;
    double time = 0.0;
    double bar_duration = 0.0;
    double next_bar_time = 0.0;

    if(start_evt != NULL)
    {
      scomark = (scomark_t *)fts_get_object(event_get_value(start_evt));
      if(scomark_is_bar(scomark))
      {
        scomark_bar_get_meter(scomark, &meter);
        if(meter == sym_meter_empty)
        {       
          marker_track_get_previous_meter(marker_track, scomark, &last_meter);
          if(last_meter != NULL && last_meter != sym_meter_empty)
          {
            scomark_t *next_bar = NULL; 
            event_t *next_evt = NULL;
            double new_bar_time = 0.0;
            event_t *event_at_same_time = NULL;
                                                
            scomark_get_tempo(scomark, &tempo);
            if(tempo <= 0.0) 
              marker_track_get_previous_tempo(marker_track, scomark, &tempo);
                                                
            next_bar = marker_track_get_next_bar(marker_track, scomark);
            if(next_bar == NULL) /* append last bar */
              return marker_track_append_last_bar(marker_track, start_evt, last_meter, tempo);
            else
            {
              /* get timetag of new bar */
              new_bar_time = get_next_bar_time(start_evt, last_meter, tempo, &event_at_same_time);
                                                
              /* get timetag of actual next bar */
              next_evt = (event_t *)fts_object_get_context((fts_object_t *)next_bar);    
              next_bar_time = event_get_time(next_evt);      
              if(new_bar_time < next_bar_time - MARKERS_BAR_TOLERANCE)
              {
                event_unset_property(start_evt, seqsym_meter);
                /* append new bar at new_bar_time */
                new_bar = marker_track_insert_marker(marker_track, new_bar_time, seqsym_bar, &new_event);     
                scomark_bar_set_meter( new_bar, sym_meter_empty, &old_meter);/*set empty_meter on last bar*/
                marker_track_renumber_bars(marker_track, NULL, FIRST_BAR_NUMBER, 1);
                return new_event;
              }
              else
              {
                /* - do the same as in change_meter; */         
                fts_symbol_t old_meter = NULL;
                scomark_bar_set_meter(scomark, last_meter, &old_meter);
                marker_track_meter_changed(marker_track, scomark, old_meter, last_meter, 1);
                event_unset_property( start_evt, seqsym_meter);
                return start_evt;
              }
            }
          }
          else
          {
            /* 
               senza metrics lo mette a un default....
            */          
            fts_post("lo mette a crai!!!!");
            return NULL;
          }
        }
      }
    }
    else
    {
      marker_event = track_get_first(marker_track);
                        
      /* get last bar meter and tempo */
      while(marker_event != NULL)
      {
        double t = -1.0;
        scomark = (scomark_t *)fts_get_object(event_get_value(marker_event));
        time = event_get_time(marker_event);
      
        scomark_get_tempo(scomark, &t);
        if(t > 0.0) tempo = t;
      
        if(scomark_is_bar(scomark))
        {
          last_bar = marker_event;
          last_scomark_bar = scomark;
          scomark_bar_get_meter(scomark, &meter);
          if( meter != NULL && meter != sym_meter_empty) last_meter = meter;
          last_tempo = tempo;
          scomark_bar_get_number( scomark, &last_number);
        }  
      
        marker_event = event_get_next(marker_event);
      }
                
      if(last_bar == NULL)/* insert first bar with default metrics and tempo */
      {
        double old_tempo = 0.0;
        new_bar = marker_track_insert_marker(marker_track, 0.0, seqsym_bar, &new_event);      
        scomark_bar_set_meter( new_bar, sym_meter_4_4, &old_meter);
        scomark_set_tempo( new_bar, 60, &old_tempo);
        scomark_bar_set_number( new_bar, FIRST_BAR_NUMBER);
      }
      else 
        new_event = marker_track_append_last_bar(marker_track, last_bar, last_meter, last_tempo);
    }
  }
  return new_event;
}


/** EMACS **
* Local variables:
* mode: c
* c-basic-offset:2
* End:
*/
