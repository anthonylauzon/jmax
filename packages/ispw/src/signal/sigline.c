#include "fts.h"


static fts_symbol_t sigline_function = 0;
#ifdef  COMP_7_2
static fts_symbol_t sigline_64_function = 0;
#endif
typedef struct
{
  float val;
  float target;
  float steps;
  float vecsize;
} line_control_t;


typedef struct
{
  fts_object_t _o;
  int in1;
  int set;
  int vs;
  int   vecsize;
  float srate;
  ftl_data_t ftl_data;
} sigline_t;


/* #define COMP_7_2 */

#ifdef  COMP_7_2

static void ftl_line(fts_word_t *argv)
{
  float * restrict fp = (float *)fts_word_get_ptr(argv);
  line_control_t * restrict lctl = (line_control_t *)fts_word_get_ptr(argv+1);
  long int n = fts_word_get_long(argv+2);
  
  if (lctl->steps > 0)
    {
      int i;
      float val = lctl->val, diff = (lctl->target - val)/lctl->steps;

      for (i = 0; i < n; i ++)
	{
	  val += diff;
	  fp[i] = val;
	}

      lctl->val = val;
      lctl->steps -= lctl->vecsize;
    }
  else
    {
      int i;
      float target = lctl->target;

      for (i = 0; i < n; i ++)
	fp[i] = target;

      lctl->val = target;
    }
}

static void ftl_line_64(fts_word_t *argv)
{
  float * restrict fp = (float *)fts_word_get_ptr(argv);
  line_control_t * restrict lctl = (line_control_t *)fts_word_get_ptr(argv+1);
  
  if (lctl->steps > 0)
    {
      int i;
      float val = lctl->val, diff = (lctl->target - val)/lctl->steps;

      for (i = 0; i < 64; i ++)
	{
	  val += diff;
	  fp[i] = val;
	}

      lctl->val = val;
      lctl->steps -= lctl->vecsize;
    }
  else
    {
      int i;
      float target = lctl->target;

      for (i = 0; i < 64; i ++)
	fp[i] = target;

      lctl->val = target;
    }
}

#else

static void ftl_line(fts_word_t *argv)
{
  float *fp = (float *)fts_word_get_ptr(argv);
  line_control_t *lctl = (line_control_t *)fts_word_get_ptr(argv+1);
  long int n = fts_word_get_long(argv+2);
  
  if (lctl->steps > 0)
    {
      int i;
      float val = lctl->val, diff = (lctl->target - val)/lctl->steps;

      for (i = 0; i < n; i += 8)
	{
	  fp[i + 0] = (val += diff);
	  fp[i + 1] = (val += diff);
	  fp[i + 2] = (val += diff);
	  fp[i + 3] = (val += diff);
	  fp[i + 4] = (val += diff);
	  fp[i + 5] = (val += diff);
	  fp[i + 6] = (val += diff);
	  fp[i + 7] = (val += diff);
	}

      lctl->val = val;
      lctl->steps -= lctl->vecsize;
    }
  else
    {
      int i;
      float target = lctl->target;

      for (i = 0; i < n; i += 8)
	{
	  fp[i + 0] = target;
	  fp[i + 1] = target;
	  fp[i + 2] = target;
	  fp[i + 3] = target;
	  fp[i + 4] = target;
	  fp[i + 5] = target;
	  fp[i + 6] = target;
	  fp[i + 7] = target;
	}

      lctl->val = target;
    }
}

#endif

static void
sigline_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  float f;

  this->srate = fts_dsp_get_output_srate(dsp, 0);
  this->vecsize = fts_dsp_get_output_size(dsp, 0);

  f = (float) this->vecsize;
  ftl_data_set(line_control_t, this->ftl_data, vecsize, &f); 

#ifdef  COMP_7_2
  if (this->vecsize == 64)
    {
      fts_set_symbol (argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->ftl_data);
      dsp_add_funcall(sigline_64_function, 2, argv);
    }
  else
    {
#endif
      fts_set_symbol (argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->ftl_data);
      fts_set_long   (argv+2, fts_dsp_get_output_size(dsp, 0));
      dsp_add_funcall(sigline_function, 3, argv);
#ifdef  COMP_7_2
    }
#endif
}


static void
sigline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;
  int down = (long) fts_get_number_arg(ac, at, 1, 0);
  float f;

  this->in1 = 0;
  this->vecsize = DEFAULTVS;

  this->set = 0;
  this->srate = fts_dsp_get_sampling_rate(); /* should go away !!! (mdc) */

  this->ftl_data = ftl_data_new(line_control_t);

  f = 0.0f;
  ftl_data_set(line_control_t, this->ftl_data, steps, &f);
  ftl_data_set(line_control_t, this->ftl_data, target, &f);
  ftl_data_set(line_control_t, this->ftl_data, val, &f);

  if (down > 0)
    {
      fts_atom_t a;

      fts_set_long(&a, down);

      fts_object_put_prop(o, fts_s_dsp_downsampling, &a);
    }

  dsp_list_insert(o);
}


static void
sigline_float_realize(sigline_t *this, float f)
{
  float steps;

  if (!this->set || ((steps = this->vecsize * this->in1) <= 0))
    {
      steps = 0;
      ftl_data_set(line_control_t, this->ftl_data, val, &f);
    }

  ftl_data_set(line_control_t, this->ftl_data, target, &f);
  ftl_data_set(line_control_t, this->ftl_data, steps, &steps);

  this->set = 0;
}

static void
sigline_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_float_realize((sigline_t *)o, (float) fts_get_number(at));
}


static void
sigline_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;

  this->in1 = fts_get_number_arg(ac, at, 0, 0.0f) * .001f * this->srate / this->vecsize;

  this->set = 1;
}



static void
sigline_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && fts_is_number(&at[1]))
    sigline_number_1(o, winlet, fts_s_float, 1, at+1);

  if ((ac >= 1) && fts_is_number(&at[0]))
    sigline_float_realize((sigline_t *)o,
			  (float)fts_get_number_arg(ac, at, 0, 0));
}

static void
sigline_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigline_t *this = (sigline_t *)o;

  ftl_data_free(this->ftl_data);
  dsp_list_remove(o);
}


static fts_status_t
sigline_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigline_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigline_init, 2, a, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigline_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigline_put, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigline_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sigline_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, sigline_list);
  
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, sigline_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, sigline_number_1, 1, a);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  sigline_function = fts_new_symbol("line");
  dsp_declare_function(sigline_function, ftl_line);

#ifdef  COMP_7_2
  sigline_64_function = fts_new_symbol("line64");
  dsp_declare_function(sigline_64_function, ftl_line_64);
#endif

  return fts_Success;
}

void
sigline_config(void)
{
  fts_metaclass_create(fts_new_symbol("line~"),sigline_instantiate, fts_always_equiv);
}
