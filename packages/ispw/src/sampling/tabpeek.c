#include "fts.h"
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
  
  this->tab_name = tab_name;

  this->ftl_data = ftl_data_new(sampbuf_t *);
  ftl_data_copy(sampbuf_t *, this->ftl_data, &null);

  dsp_list_insert(o); /* just put object in list */
}


static void
tabpeek_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;

  ftl_data_free(this->ftl_data);
  dsp_list_remove(o);
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
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      ftl_data_copy(sampbuf_t *, this->ftl_data, &buf);
      fts_set_ftl_data(argv + 0, this->ftl_data);
      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
      fts_set_long(argv + 3, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(dsp_symbol, 4, argv);
    }
  else
    post("tabpeek: can't find table~: %s\n", fts_symbol_name(this->tab_name));
}

static void
ftl_tabpeek(fts_word_t *argv)
{
  sampbuf_t **data = (sampbuf_t **)fts_word_get_ptr(argv + 0);
  float *in = (float *)fts_word_get_ptr(argv + 1);
  float *out = (float *)fts_word_get_ptr(argv + 2);
  long n = fts_word_get_long(argv + 3);
  sampbuf_t *buf = *data;
  float *tab = buf->samples;
  long tabend = buf->size - 1;
  int i;

#ifdef HAS_UNROLL_BY8
  if (buf)
    for (i = 0; i < n; i += 8)
      {
	long idx0, idx1, idx2, idx3, idx4, idx5, idx6, idx7;

	idx0 = (long) in[i + 0];
	idx1 = (long) in[i + 1];
	idx2 = (long) in[i + 2];
	idx3 = (long) in[i + 3];
	idx4 = (long) in[i + 4];
	idx5 = (long) in[i + 5];
	idx6 = (long) in[i + 6];
	idx7 = (long) in[i + 7];


	out[i + 0] = tab[idx0 >= 0 ? (idx0 < tabend ? idx0 : tabend) : 0];
	out[i + 1] = tab[idx1 >= 0 ? (idx1 < tabend ? idx1 : tabend) : 0];
	out[i + 2] = tab[idx2 >= 0 ? (idx2 < tabend ? idx2 : tabend) : 0];
	out[i + 3] = tab[idx3 >= 0 ? (idx3 < tabend ? idx3 : tabend) : 0];
	out[i + 4] = tab[idx4 >= 0 ? (idx4 < tabend ? idx4 : tabend) : 0];
	out[i + 5] = tab[idx5 >= 0 ? (idx5 < tabend ? idx5 : tabend) : 0];
	out[i + 6] = tab[idx6 >= 0 ? (idx6 < tabend ? idx6 : tabend) : 0];
	out[i + 7] = tab[idx7 >= 0 ? (idx7 < tabend ? idx7 : tabend) : 0];
      }
  else
    {   
      for (i = 0; i < n; i+=8)
	{
	  out[i + 0] = 0.0f;
	  out[i + 1] = 0.0f;
	  out[i + 2] = 0.0f;
	  out[i + 3] = 0.0f;
	  out[i + 4] = 0.0f;
	  out[i + 5] = 0.0f;
	  out[i + 6] = 0.0f;
	  out[i + 7] = 0.0f;
	}
    }

#else
  if (buf)
    for (i = 0; i < n; i++)
      {
	long idx;

	idx = (long) in[i];

	out[i] = tab[idx >= 0 ? (idx < tabend ? idx : tabend) : 0];
      }
  else
    {   
      for (i = 0; i < n; i++)
	out[i] = 0.0f;
    }
#endif
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
  fts_symbol_t tab_name = fts_get_symbol(at);
  sampbuf_t *buf = sampbuf_get(tab_name);

  if (buf)
    {
      this->tab_name = tab_name;
      ftl_data_copy(sampbuf_t *, this->ftl_data, &buf);
    }
  else
    post("tabpeek: can't find table~: %s\n", fts_symbol_name(tab_name));
}

static void
tabpeek_set_by_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabpeek_t *this = (tabpeek_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_long(at));
  tab_name = fts_new_symbol_copy(name_str);  
  buf = sampbuf_get(tab_name);

  if (buf)
    {
      this->tab_name = tab_name;
      ftl_data_copy(sampbuf_t *, this->ftl_data, &buf);
    }
  else
    post("tabpeek: can't find table~: %s\n", fts_symbol_name(tab_name));
}
  


/******************************************************************
 *
 *    class
 *
 */

static fts_status_t
tabpeek_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(tabpeek_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, tabpeek_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, tabpeek_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, tabpeek_put);

  /* tabpeek user methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("set"), tabpeek_set, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, tabpeek_set_by_int, 1, a);

  /* Type the outlet */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  dsp_symbol = fts_new_symbol("tabpeek");
  dsp_declare_function(dsp_symbol, ftl_tabpeek);

  return fts_Success;
}

void
tabpeek_config(void)
{
  fts_metaclass_create(fts_new_symbol("tabpeek~"), tabpeek_instantiate, fts_always_equiv);
}






