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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#ifndef WIN32
#include <unistd.h>
#endif
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

/************************************************************
 *
 *  midiparse
 *
 */
typedef struct _midiparse_
{
  fts_object_t o;
  fts_midiparser_t parser;
} midiparse_t;

static void
midiparse_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiparse_t *this = (midiparse_t *)o;
  int i;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	{
	  int byte = fts_get_number_int(at + i);
	  fts_midievent_t *event = fts_midiparser_byte(&this->parser, byte);
	  
	  if(event != NULL)
	    {
	      fts_object_refer((fts_object_t *)event);
	      fts_outlet_object(o, 0, (fts_object_t *)event);
	      fts_object_release((fts_object_t *)event);
	    }
	}
    }
}

static void
midiparse_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiparse_t *this = (midiparse_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;

  fts_midiparser_init(&this->parser);
}

static void 
midiparse_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiparse_t *this = (midiparse_t *)o;

  fts_midiparser_reset(&this->parser);
}

static void
midiparse_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiparse_t), midiparse_init, midiparse_delete);

  fts_class_inlet_int(cl, 0, midiparse_input);
  fts_class_inlet_float(cl, 0, midiparse_input);
  fts_class_inlet_varargs(cl, 0, midiparse_input);
  
  fts_class_outlet(cl, 0, fts_midievent_type);
}

/************************************************************
 *
 *  midiunparse
 *
 */

#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_END 0xf7

static void
midiunparse_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiparse_t *this = (midiparse_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  if(fts_midievent_is_channel_message(event))
    {
      if(fts_midievent_channel_message_has_second_byte(event))
	{
	  fts_atom_t a[3];

	  fts_set_int(a + 0, fts_midievent_channel_message_get_status_byte(event));
	  fts_set_int(a + 1, fts_midievent_channel_message_get_first(event) & 0x7f);
	  fts_set_int(a + 2, fts_midievent_channel_message_get_second(event) & 0x7f);

	  fts_outlet_varargs(o, 0, 3, a);
	}
      else
	{
	  fts_atom_t a[2];

	  fts_set_int(a + 0, fts_midievent_channel_message_get_status_byte(event));
	  fts_set_int(a + 1, fts_midievent_channel_message_get_first(event) & 0x7f);

	  fts_outlet_varargs(o, 0, 2, a);
	}
    }
  else 
    {
      switch(fts_midievent_get_type(event))
	{
	case midi_system_exclusive:
	  {
	    int size = fts_midievent_system_exclusive_get_size(event);
	    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
	    fts_atom_t *a = alloca((size + 2) * sizeof(fts_atom_t));
	    int i;
	    	
	    fts_set_int(a + 0, STATUS_BYTE_SYSEX);
    
	    for(i=0; i<size; i++)
	      a[i + 1] = atoms[i];
	    
	    fts_set_int(a + size + 1, STATUS_BYTE_SYSEX_END);

	    fts_outlet_varargs(o, 0, size + 2, a);
	  }
	  break;
	  
	case midi_real_time:
	  fts_outlet_int(o, 0, fts_midievent_real_time_get_status_byte(event));
	  break;
	  
	default:
	  break;
	}
    }
}

static void
midiunparse_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0);

  fts_class_message_varargs(cl, fts_s_midievent, midiunparse_input);

  fts_class_outlet_varargs(cl, 0);
}

void
midiparse_config(void)
{
  fts_class_install(fts_new_symbol("midiparse"), midiparse_instantiate);
  fts_class_install(fts_new_symbol("midiunparse"), midiunparse_instantiate);
}
