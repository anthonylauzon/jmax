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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

typedef struct _sysex_
{
  fts_object_t o;
  fts_midiport_t *port;
} sysex_t;

/************************************************************
 *
 *  MIDI system exclusive input
 *
 */

static int 
sysexin_check(int ac, const fts_atom_t *at, fts_midiport_t **port)
{
  *port = 0;

  if(ac == 1)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);
	  
	  if(fts_object_is_midiport(obj) && fts_midiport_is_input((fts_midiport_t *)obj))
	    *port = (fts_midiport_t *)fts_get_object(at);	  
	  else
	    return 0;
	}
      else
	return 0;
    }

  /* if there is still no port try to get default */
  if(!*port)
    *port = fts_midiport_get_default();
  
  if(!*port)
    return 0;  

  return 1;
}

static void
sysexin_callback(fts_object_t *o, int ac, const fts_atom_t *at, double time)
{
  sysex_t *this = (sysex_t *)o;

  fts_outlet_send(o, 0, fts_s_list, ac, at);
}

static void
sysexin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  sysexin_check(ac - 1, at + 1, &this->port);
  fts_midiport_add_listener_system_exclusive(this->port, o, sysexin_callback);
}

static void 
sysexin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  if(this->port)
    fts_midiport_remove_listener_system_exclusive(this->port, o);
}

static int 
sysexin_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  fts_midiport_t *port;

  return sysexin_check(ac1 - 1, at1 + 1, &port);
}

static fts_status_t
sysexin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;

  if(sysexin_check(ac - 1, at + 1, &port))
    {
      fts_class_init(cl, sizeof(sysex_t), 0, 1, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysexin_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysexin_delete);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

/************************************************************
 *
 *  MIDI system exclusive output
 *
 */

static int 
sysexout_check(int ac, const fts_atom_t *at, fts_midiport_t **port)
{
  *port = 0;

  if(ac == 1)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);
	  
	  if(fts_object_is_midiport(obj) && fts_midiport_is_output((fts_midiport_t *)obj))
	    *port = (fts_midiport_t *)fts_get_object(at);	  
	  else
	    return 0;
	}
      else
	return 0;
    }

  /* if there is still no port try to get get default */
  if(!*port)
    *port = fts_midiport_get_default();
  
  if(!*port)
    return 0;  

  return 1;
}

static void
sysexout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysex_t *this = (sysex_t *)o;

  if(this->port)
    {
      int i;
      
      for(i=0; i<ac; i++)
	if(fts_is_int(at + i))
	  fts_midiport_output_system_exclusive_byte(this->port, fts_get_int(at + i));

      fts_midiport_output_system_exclusive_flush(this->port, 0.0);
    }
}

static void
sysexout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  sysexin_check(ac - 1, at + 1, &this->port);
}

static void 
sysexout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
}

static int 
sysexout_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  fts_midiport_t *port;  

  return sysexout_check(ac1 - 1, at1 + 1, &port);
}

static fts_status_t
sysexout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;  

  if(sysexout_check(ac - 1, at + 1, &port))
    {
      fts_class_init(cl, sizeof(sysex_t), 1, 0, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysexout_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysexout_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, sysexout_send);
      fts_method_define_varargs(cl, 0, fts_s_float, sysexout_send);
      fts_method_define_varargs(cl, 0, fts_s_list, sysexout_send);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
sysex_config(void)
{
  fts_metaclass_install(fts_new_symbol("sysexin"), sysexin_instantiate, sysexin_equiv);
  fts_metaclass_install(fts_new_symbol("sysexout"), sysexout_instantiate, sysexout_equiv);
}
