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
#include "sampbuf.h"

typedef struct tabpeek
{
  fts_object_t o;
  fts_symbol_t tab_name;	/* symbol bound to table we'll use */
  ftl_data_t ftl_data;		/* ftl_data pointing to the buf */
} tabpeek_t;


static fts_symbol_t dsp_symbol = 0;

/******************************************************************
 *
 *    object
 *
 */

static void
tabpeek_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 1, 0);
  sampbuf_t *null = 0;

  this->tab_name = 0;
  this->ftl_data = 0;
  
  if(ac > 0 && fts_is_symbol(at))
    {
      sampbuf_t **data;

      this->tab_name = tab_name;
      
      this->ftl_data = ftl_data_alloc(sizeof(sampbuf_t *));
      data = (sampbuf_t **)ftl_data_get_ptr(this->ftl_data);

      *data = 0;

      fts_dsp_add_object(o); /* just put object in list */
    }
  else
    fts_object_set_error(o, "Argument (name of table~) required");
}


static void
tabpeek_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;

  if(this->ftl_data)
    {
      ftl_data_free(this->ftl_data);
      fts_dsp_remove_object(o);
    }
}


/******************************************************************
 *
 *    dsp
 *
 */

static void
tabpeek_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  sampbuf_t **data = (sampbuf_t **)ftl_data_get_ptr(this->ftl_data);
  
  if(this->tab_name)
    {
      sampbuf_t *buf = sampbuf_get(this->tab_name);

      if(buf)
	{
	  *data = buf;
      
	  fts_set_ftl_data(argv + 0, this->ftl_data);
	  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(argv + 3, fts_dsp_get_input_size(dsp, 0));
	  fts_dsp_add_function(dsp_symbol, 4, argv);

	  return;
	}

      post("tabpeek: can't find table~: %s\n", this->tab_name);
    }
}

static void
ftl_tabpeek(fts_word_t *argv)
{
  sampbuf_t **data = (sampbuf_t **)fts_word_get_pointer(argv + 0);
  float *in = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  sampbuf_t *buf = *data;
  float *tab = buf->samples;
  int size = buf->size;
  int i;

  if(tab)
    {
      for(i=0; i<n; i++)
	{
	  int idx = (int)in[i];

	  if(idx >= size)
	    out[i] = tab[size - 1];
	  else if(idx < 0)
	    out[i] = tab[0];
	  else
	    out[i] = tab[idx];
	}
    }
  else
    {
      for(i=0; i<n; i++)
	out[i] = 0.0;
    }
}


/******************************************************************
 *
 *    user methods
 *
 */

static void
tabpeek_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;
  
  if(fts_is_symbol(at))
    {
      fts_symbol_t tab_name = fts_get_symbol(at);
      sampbuf_t *buf = sampbuf_get(tab_name);
      
      if (buf)
	{
	  sampbuf_t **data = (sampbuf_t **)ftl_data_get_ptr(this->ftl_data);

	  *data = buf;
	  this->tab_name = tab_name;
	}
      else
	post("tabpeek: can't find table~: %s\n", tab_name);
    }
}

static void
tabpeek_set_by_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_int(at));
  tab_name = fts_new_symbol(name_str);  
  buf = sampbuf_get(tab_name);

  if (buf)
    {
      sampbuf_t **data = (sampbuf_t **)ftl_data_get_ptr(this->ftl_data);
      
      *data = buf;
      this->tab_name = tab_name;
    }
  else
    post("tabpeek: can't find table~: %s\n", tab_name);
}
  


/******************************************************************
 *
 *    class
 *
 */

static void
tabpeek_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tabpeek_t), tabpeek_init, tabpeek_delete);

  fts_class_message_varargs(cl, fts_s_put, tabpeek_put);

  fts_class_message_varargs(cl, fts_s_set, tabpeek_set);
  fts_class_inlet_int(cl, 0, tabpeek_set_by_int);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  dsp_symbol = fts_new_symbol("tabpeek");
  fts_dsp_declare_function(dsp_symbol, ftl_tabpeek);
}

void
tabpeek_config(void)
{
  fts_class_install(fts_new_symbol("tabpeek~"), tabpeek_instantiate);
}
