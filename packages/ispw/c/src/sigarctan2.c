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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

/***************** sigarctan2 *******************************/

#include "math.h"
#include <fts/fts.h>


#define TABLE_SIZE 4096
#define PI_OVER_TWO 1.5707963f
#define PI 3.14159265f
#define NPI -3.14159265f
#define NPI_OVER_TWO -1.5707963f
#define VERSION "arctan2~  1-94 bilp & zeep"

typedef struct sigarctan2
{
  fts_object_t obj;

  float *table;   	
} sigarctan2_t;

static fts_symbol_t sigarctan2_function = 0;
static float *atan_table = 0;

static void
ftl_arctan2(fts_word_t *argv)
{
  float *yval = (float *)fts_word_get_pointer(argv + 0);
  float *xval = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  long n = fts_word_get_int(argv + 3);
  float  inval, outval, offset, where;
  float ax,ay;
  int bin;
	
  while (n--) 
    {
      ay = (*yval < 0) ? 0-*yval:*yval;
      ax = (*xval < 0) ? 0-*xval:*xval;
      
      if(ay<ax)	/* normal atan lookup - atan(y/x) */
	{
	  inval = ay / ax;
	  where = (TABLE_SIZE * inval);
	  bin = (int) (where);
	  offset = where - bin;
	  outval = (1-offset)*(atan_table[bin] + offset * atan_table[bin+1]);
	}
      else		/* pi/2 - atan(x/y) */
	{
	  inval = (ay==0) ? 0: ax / ay; /* force phase angle of 0 for case: x=y=0 */
	  where = (TABLE_SIZE * inval);
	  bin = (int) where;
	  offset = where - bin;
	  outval = PI_OVER_TWO - (1-offset)*(atan_table[bin] + offset * atan_table[bin+1]);
	}

      if (*xval<0.0f)
	{
	  if (*yval >=0.0f)	/* -,+ 2nd */
	    *out = PI - outval;
	  else		/* -,-  3rd */
	    *out = NPI + outval;
	}
      else
	{
	  if (*yval < 0.0f)	/* +,-  4th */
	    *out = 0 - outval;
	  else		/* +,-  1st */
	    *out = outval;
	}

      yval++;
      xval++;
      out++; 
    } 	
}


static void
sigarctan2_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int  (argv + 3, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sigarctan2_function, 4, argv);
}



static void
sigarctan2_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (! atan_table)
    {
      int i;

      atan_table = fts_malloc((TABLE_SIZE + 2) * sizeof(float));

      for(i=0; i<=TABLE_SIZE; i++)
	atan_table[i] = atan(((float)i) / ((float)TABLE_SIZE));

      atan_table[TABLE_SIZE+1] = atan(1.0f);	/* extra point for interp. */
    }

  fts_dsp_add_object(o); /* just put object in list */
}


static void
sigarctan2_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}


static fts_status_t
sigarctan2_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigarctan2_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigarctan2_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigarctan2_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigarctan2_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_outlet(cl, 0);

  sigarctan2_function = fts_new_symbol("arctan2");
  fts_dsp_declare_function(sigarctan2_function, ftl_arctan2);

  return fts_Success;
}

void
sigarctan2_config(void)
{
  fts_class_install(fts_new_symbol("arctan2~"),sigarctan2_instantiate);
}
