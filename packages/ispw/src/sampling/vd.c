/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "deltable.h"
#include "delbuf.h"
#include "vd.h"

/* buf_ptr is incremented each sample and corresponds to zero delay;
   at the outset it points into the buffer proper but at the end it
   could point n-1 samps past the logical end.  Since the offset is
   bounded below by n, bp - offset does not point past the end;
   in fact you can read four consecutive samples starting from there
   (but bp-offset might point before the beginning; this is guarded
   against in the loop.) */

static fts_symbol_t vd_dsp_symbol = 0;
static fts_symbol_t vd_miller_dsp_symbol = 0;

/**************************************************
 *
 *    object
 *
 */

typedef struct
{
  fts_object_t  obj;
  fts_symbol_t name;
  fts_object_t *next; /* DCE: pointer to the other vd for the same delay line */ 
  fts_symbol_t unit;
  float max_incr;
  float millers_fix_del;
  ftl_data_t vd_data;
} vd_t;


static void
vd_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);

  if(!name)
    return;
  
  this->name = name;
  this->vd_data = ftl_data_new(ftl_vd_t);
  this->next = 0;

  if(unit)
    {
      this->unit = unit;
      at += 3; 
      ac -= 3;
    }
  else
    {
      this->unit = fts_s_msec;
      at += 2; 
      ac -= 2;
    }

  if(ac && fts_is_number(at))
    {
      float max_incr = fts_get_number(at);

      if(max_incr < 1.0f)
	this->max_incr = 4.0f;
      else
	this->max_incr = max_incr;

      at++;
      ac--;
    }
  else
    this->max_incr = 4.0f;

  /* up argument given -> ISPW bug compatibillity (ftl_vd_miller)
     if the argument is not given the up sampling from ratio regarding to delwrite
     is implictly detected and the bug free ftl_vd executed in DSP time */
     
  if(ac && fts_is_int(at))
    {
      int up = fts_get_int(at);

      if(up < 0)
	this->millers_fix_del = 1.0;
      else if (up > 15) 
	this->millers_fix_del = 1.0f / 32768.0f;
      else
	this->millers_fix_del = 1.0f / (1 << up);
    }
  else
    this->millers_fix_del = 0.0f;


  delay_table_add_delreader(o, this->name);
  dsp_list_insert(o);
}


static void
vd_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;

  delay_table_remove_delreader(o, this->name);

  ftl_data_free(this->vd_data);

  dsp_list_remove(o);
}

/**************************************************
 *
 *    dsp
 *
 */

static void
vd_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  ftl_vd_t *ftl = (ftl_vd_t *)ftl_data_get_ptr(this->vd_data);
  float conv;
  del_buf_t *buf;
  fts_atom_t argv[6];

  buf = delay_table_get_delbuf(this->name);
  if(!buf){
    post("vd~: can't find delay line: %s\n", fts_symbol_name(this->name));
    return;
  }

  if(delay_table_is_delwrite_scheduled(this->name))
    ftl->write_advance = (float)buf->n_tick;
  else
    ftl->write_advance = 0.0f;

  ftl->conv = fts_unit_convert_to_base(this->unit, 1.0f, &sr);
  ftl->max_span = this->max_incr * (float)n_tick;
  
  if(this->millers_fix_del == 0.0f)
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr(argv + 2, buf);
      fts_set_ftl_data(argv + 3, this->vd_data);
      fts_set_long(argv + 4, n_tick);
      dsp_add_funcall(vd_dsp_symbol, 5, argv);
    }
  else /* call millers version */
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr(argv + 2, buf);
      fts_set_ftl_data(argv + 3, this->vd_data);
      fts_set_long(argv + 4, n_tick);
      fts_set_float(argv + 5, this->millers_fix_del * (float)n_tick); /* write tick size (hopefully) */
      dsp_add_funcall(vd_miller_dsp_symbol, 6, argv);
    }

  delay_table_delreader_scheduled(this->name);
}

/**************************************************
 *
 *    class
 *
 */

static fts_status_t
vd_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  fts_class_init(cl, sizeof(vd_t), 2, 1, 0);

  /* System methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vd_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, vd_delete, 0, 0);
  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, vd_put, 1, a);

  /* DSP declarations */
  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1); 
  dsp_sig_outlet(cl, 0);        
  
  vd_dsp_symbol = fts_new_symbol("vd");
  dsp_declare_function(vd_dsp_symbol, ftl_vd);

  vd_miller_dsp_symbol = fts_new_symbol("vd_miller");
  dsp_declare_function(vd_miller_dsp_symbol, ftl_vd_miller);

  return fts_Success;
}

void
vd_config(void)
{
  fts_metaclass_create(fts_new_symbol("vd~"), vd_instantiate, fts_always_equiv);
}
