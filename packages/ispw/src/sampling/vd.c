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
static fts_symbol_t vd_dsp_inplace_symbol = 0;

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
  ftl_data_t vd_data;
} vd_t;


static void
vd_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);

  this->name = name;

  if(unit)
    this->unit = unit;
  else
    this->unit = fts_s_msec;

  this->vd_data = ftl_data_new(vd_ctl_t);

  this->next = 0;

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
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  float conv;
  del_buf_t *buf;

  buf = delay_table_get_delbuf(this->name);
  if(!buf){
    post("vd~: can't find delay line: %s\n", fts_symbol_name(this->name));
    return;
  }

  conv = fts_unit_convert_to_base(this->unit, 1.0f, &sr);
  ftl_data_set(vd_ctl_t, this->vd_data, conv, &conv);
  
  if(delay_table_is_delwrite_scheduled(this->name))
    {
      int i = n_tick; /* write before read! */
      ftl_data_set(vd_ctl_t, this->vd_data, write_advance, &i);
    }
  else
    {
      float i = 0;
      ftl_data_set(vd_ctl_t, this->vd_data, write_advance, &i);
    }

  if(delbuf_is_init(buf))
    {
      if(delbuf_get_tick_size(buf) != n_tick){ /* check if n_tick of delread and delwrite matches */
	post("error: vd~: %s: sample rate does not match with delay line\n", fts_symbol_name(this->name));
	return;
      }
    }
  else
    {
      /* first delwrite~ or delread~ or vd~ scheduled for this delayline inits buffer */

      if (! delbuf_init(buf, sr, n_tick))
	return;
    }

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ptr(argv + 1, buf);
      fts_set_ftl_data(argv + 2, this->vd_data);
      fts_set_long(argv + 3, n_tick);
      dsp_add_funcall(vd_dsp_inplace_symbol, 4, argv);
    }
  else
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr(argv + 2, buf);
      fts_set_ftl_data(argv + 3, this->vd_data);
      fts_set_long(argv + 4, n_tick);
      dsp_add_funcall(vd_dsp_symbol, 5, argv);
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

  a[0] = fts_s_symbol; /* class name */
  a[1] = fts_s_symbol; /* delay line name */
  a[2] = fts_s_symbol; /* opt: unit name */
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, vd_init, 3, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, vd_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, vd_put, 1, a);

  /* DSP declarations */

  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1); 
  dsp_sig_outlet(cl, 0);        
  
  vd_dsp_symbol = fts_new_symbol("vd");
  dsp_declare_function(vd_dsp_symbol, ftl_vd);

  vd_dsp_inplace_symbol = fts_new_symbol("vd_inplace");
  dsp_declare_function(vd_dsp_inplace_symbol, ftl_vd_inplace);

  return fts_Success;
}

void
vd_config(void)
{
  fts_metaclass_create(fts_new_symbol("vd~"),vd_instantiate, fts_always_equiv);
}
