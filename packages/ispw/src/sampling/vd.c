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

static fts_symbol_t dsp_symbol = 0;

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

  float maxinc; /* maximum inc allowed per samp */
  float fixdel;	/* (sr of write) / (sr of vd) */
  fts_symbol_t unit;
  ftl_data_t vd_data;
} vd_t;


static void
vd_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vd_t *this = (vd_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);
  float maxinc;
  long down;

  if(unit){
    maxinc = (float)fts_get_number_arg(ac, at, 3, 0.0f);
    down = (long)fts_get_long_arg(ac, at, 4, 0);
  }else{
    maxinc = (float)fts_get_number_arg(ac, at, 2, 0.0f);
    down = (long)fts_get_long_arg(ac, at, 3, 0);
    unit = fts_s_msec; /* default */
  }

  this->name = name;
  this->next = 0;

  if (maxinc < 1.0f) 
    this->maxinc = 4.0f;
  else
    this->maxinc = maxinc;

  if (down < 0)
    down = 0;
  else if(down > 15)
    down = 15;

  this->fixdel = 1.0f/(1 << down);
  this->unit = unit;

  this->vd_data = ftl_data_new(vd_ctl_t);

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
  float f, conv;
  long l;
  del_buf_t *buf;

  buf = delay_table_get_delbuf(this->name);
  if(!buf){
    post("vd~: can't find delay line: %s\n", fts_symbol_name(this->name));
    return;
  }

  l = this->maxinc * n_tick;
  l += ((-l) & (MINVS));
  ftl_data_set(vd_ctl_t, this->vd_data, maxspan, &l); 
  
  f = ((n_tick > 1)? 1.0f/(n_tick - 1): 0);
  ftl_data_set(vd_ctl_t, this->vd_data, fudge, &f); 
  
  f =  n_tick * this->fixdel;
  ftl_data_set(vd_ctl_t, this->vd_data, writevecsize, &f); 
  
  conv = fts_unit_convert_to_base(this->unit, 1.0f, &sr);
  ftl_data_set(vd_ctl_t, this->vd_data, conv, &conv);
  
  if(delay_table_is_delwrite_scheduled(this->name))
    {
      f = -n_tick / conv; /* write before read (data is at least one tick old) */
      ftl_data_set(vd_ctl_t, this->vd_data, delonset, &f); 
    }
  else
    {
      f = 0;
      ftl_data_set(vd_ctl_t, this->vd_data, delonset, &f);
    }

  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ptr(argv + 2, buf);
  fts_set_ftl_data(argv + 3, this->vd_data);
  fts_set_long(argv + 4, n_tick);
  dsp_add_funcall(dsp_symbol, 5, argv);
    
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

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_anything;
  a[3] = fts_s_number;
  a[4] = fts_s_int ;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, vd_init, 4, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, vd_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, vd_put, 1, a);

  /* DSP declarations */

  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1); 
  dsp_sig_outlet(cl, 0);        
  
  dsp_symbol = fts_new_symbol("vd");
  dsp_declare_function(dsp_symbol, ftl_vd);

  return fts_Success;
}

void
vd_config(void)
{
  fts_metaclass_create(fts_new_symbol("vd~"),vd_instantiate, fts_always_equiv);
}

