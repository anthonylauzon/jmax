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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette, Zack Settel.
 *
 */
#include <math.h>
#include <fts/fts.h>
#include "Rsend.h"

typedef struct 
{
  fts_object_t o;
  fts_array_t args;
} Rreceive_t;

static void
scan_atom(fts_atom_t *atom, int type, char *string)
{
  switch(type)
    {

    case RSEND_LONG:
      {
	int d;
	
	sscanf(string,"%d",&d);
	fts_set_int(atom, d);
      }
      break;

    case RSEND_FLOAT:
      {
	float f;
	
	sscanf(string,"%f",&f);
	fts_set_float(atom, f);
      }
      break;

    case RSEND_SYM:

      fts_set_symbol(atom, fts_new_symbol_copy(string)); 
      break;

    default:
      fts_set_void(atom);
    }
}

static void
Rreceive_input(fts_object_t *o, int winlet, fts_symbol_t sym, int ac, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;
  char string[STRLENMAX];
  fts_symbol_t target = 0;
  fts_label_t *label = 0;
  int type = 0;
  int c = 0;
  int j = 0;
  int i;

  fts_array_set_size(&this->args, 0);

  /* skip sysex real-time id */
  for(i=1; i<ac; i++)
    {
      int c = fts_get_int(at + i);
      fts_atom_t a;

      if(c == 0)
	{
	  string[j] = '\0';
	  
	  if(type == 0)
	    target = fts_new_symbol_copy(string);
	  else
	    {
	      scan_atom(&a, type, string);
	      fts_array_append(&this->args, 1, &a);
	    }
	  
	  j = 0;
	}
      else if(c <= RSEND_SYM)
	type = c;
      else if(j < STRLENMAX)
	string[j++] = c;
    }

  if(target)
    {
      label = fts_label_get(fts_object_get_patcher(o), target);
  
      if(label)
	{
	  int size = fts_array_get_size(&this->args);
	  
	  if(size > 0)
	    {      
	      fts_atom_t *atoms = fts_array_get_atoms(&this->args);
	      
	      if (fts_is_int(atoms))
		{
		  if (size > 1)
		    fts_label_send(label, fts_s_list, size, atoms);
		  else
		    fts_label_send(label, fts_s_int, size, atoms);
		}
	      else if (fts_is_float(atoms))
		{
		  if (size > 1)
		    fts_label_send(label, fts_s_list, size, atoms);
		  else
		    fts_label_send(label, fts_s_float, size, atoms);
		}
	      else if (fts_is_symbol(atoms))
		fts_label_send(label, fts_get_symbol(atoms), size - 1, atoms + 1);
	    }
	}
      else
	fts_outlet_send(o, 0, target, fts_array_get_size(&this->args), fts_array_get_atoms(&this->args));	
    }
  else
    {
      int size = fts_array_get_size(&this->args);
	
      if(size)
	fts_outlet_atoms(o, 0, size, fts_array_get_atoms(&this->args));
    }
}

static void
Rreceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;
  
  fts_array_init(&this->args, 0, 0);
}

static void
Rreceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;

  fts_array_destroy(&this->args);
}

static fts_status_t
Rreceive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(Rreceive_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, Rreceive_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, Rreceive_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, Rreceive_input);
  fts_method_define_varargs(cl, 0, fts_s_float, Rreceive_input);
  fts_method_define_varargs(cl, 0, fts_s_list, Rreceive_input);

  return fts_Success;
}


void
Rreceive_config(void)
{
  fts_class_install(fts_new_symbol("Rreceive"), Rreceive_instantiate);
}
