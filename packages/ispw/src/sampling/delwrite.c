#include "fts.h"

#include "delbuf.h"
#include "deltable.h"

/*
  Consistency requirements for delwrite, delread to work: the logical length
  of the buffer (buf->size) and the phase (buf->phase) must be multiples of the
  vector size.  The actual delay buffer has vecsize elements at the end which are
  copies of the first vecsize elements.  Thus you can read vecsize consecutive
  samples starting anywhere as long as they don't cross the write pointer.  The
  phase can't be zero but can equal the logical length.
*/
/*
  ???
  The length of a delay line MUST be a multiple of the vector length
  The last buffer in the delay line
  (i.e. the buffer starting at adress delay->samples + length - vectorLength)
  is duplicated at beginning of delay line.
*/

static fts_symbol_t dsp_symbol = 0;
extern void ftl_delwrite(fts_word_t *a);

/**************************************************
 *
 *    object
 *
 */

typedef struct _delwrite_t
{
  fts_object_t o;
  fts_symbol_t name;
  del_buf_t *buf;
  int rec_prot; /* housekeeping for dead code elimination */
} delwrite_t;

static void
delwrite_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);
  float raw_size;
  if(unit)
    raw_size = (float)fts_get_number_arg(ac, at, 3, 0.0f);
  else{
    raw_size = (float)fts_get_number_arg(ac, at, 2, 0.0f);
    unit = fts_s_msec; /* default */
  }

  this->name = 0;
  
  if(delay_table_get_delbuf(name)){
    post("delwrite~: %s: multiply defined (last ignored)\n", fts_symbol_name(name));
    return;
  }

  this->name = name;
  this->rec_prot = 0;
  this->buf = delbuf_new(raw_size, unit);

  delay_table_add_delwrite(o, this->name, this->buf);
  dsp_list_insert(o); /* just put object in list */
}

static void
delwrite_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;

  if(this->name)
    {
      delbuf_delete_delayline(this->buf);
      delay_table_remove_delwrite(o, this->name);
      dsp_list_remove(o);
    }
}


/**************************************************
 *
 *    dsp
 *
 */

static void
delwrite_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  
  if(!this->name){
    post("error: delwrite~: dead object (guess which :-)\n");
  }

  if(delbuf_is_init(this->buf))
    {
      if(this->buf->n_tick != n_tick){
	post("error: delwrite~: %s: sample rate does not match with delread~\n", 
	     fts_symbol_name(this->name));
	return;
      }
    }
  else
    {
      int success;
      success = delbuf_init(this->buf, sr, n_tick);
      if(!success) return;
    }

  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr(argv + 1, this->buf);
  fts_set_long(argv + 2, n_tick);
  dsp_add_funcall(dsp_symbol, 3, argv);

  delay_table_delwrite_scheduled(this->name);
}

/**************************************************
 *
 *    user methods
 *
 */

static void
delwrite_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  if(this->name) delbuf_clear_delayline(this->buf);
}

static void
delwrite_realloc(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delwrite_t *this = (delwrite_t *)o;
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 0, 0);
  float raw_size;

  if(unit)
    raw_size = (float)fts_get_number_arg(ac, at, 1, 0.0f);
  else{
    raw_size = (float)fts_get_number_arg(ac, at, 0, 0.0f);
    unit = fts_s_msec; /* default */
  }

  if(!this->name) return;
  if(raw_size <= 0) raw_size = this->buf->raw_size;

  this->buf->raw_size = raw_size;
  this->buf->unit = unit;
}


/**************************************************
 *
 *    class
 *
 */


static fts_status_t
delwrite_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  fts_class_init(cl, sizeof(delwrite_t), 1, 1, 0); 

  /* System methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_anything;
  a[3] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, delwrite_init, 4, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, delwrite_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, delwrite_put, 1, a);

  fts_method_define(cl, 0, fts_new_symbol("clear"), delwrite_clear, 0, 0);
  
  a[0] = fts_s_anything; /* unit or size */
  a[1] = fts_s_number; /* size or no */
  fts_method_define_optargs(cl, 0, fts_new_symbol("realloc"), delwrite_realloc, 2, a, 1);

  /* DSP declarations */

  /* ask if any delread or vd is a sink to know if delwrite should be executed */

  /* fts_class_add_get_daemon(cl, fts_s_dsp_is_sink, delwrite_is_dsp_sink_get_daemon); */

  /* outlet 0 is used only for order forcing */
  /* BUT: the buffer size must be propagated to the following object!!! */
  /* fts_class_put_long_prop(cl, fts_s_dsp_order_forcing_outlet, 0); */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  dsp_symbol = fts_new_symbol("delwrite");
  dsp_declare_function(dsp_symbol, ftl_delwrite);

  return fts_Success;
}

void
delwrite_config(void)
{
  fts_metaclass_create(fts_new_symbol("delwrite~"),delwrite_instantiate, fts_always_equiv);
}
