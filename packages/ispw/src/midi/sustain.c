/* The sustain object */


#include "fts.h"

struct slink
{
  long pitch;
  struct slink *next;
};


typedef struct sustain
{
  fts_object_t ob;
  long vel;
  long sust;
  struct slink *sustained_notes;
} sustain_t;


static void
sustain_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 0);

  if (this->vel || !this->sust)
    {
      fts_outlet_int(o, 1, this->vel);
      fts_outlet_int(o, 0, n);
    }
  else
    {
      struct slink *p;

      p = (struct slink *) fts_malloc(sizeof(struct slink));
      p->pitch = n;
      p->next  = this->sustained_notes;
      this->sustained_notes = p;
    }
}

static void
sustain_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;

  this->vel = fts_get_int_arg(ac, at, 0, 0);
}

static void
sustain_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;

  this->sust = fts_get_int_arg(ac, at, 0, 0);

  if ((this->sust == 0) && this->sustained_notes)
    {
      struct slink *s, *s2;

      for (s = this->sustained_notes; s; s = s2)
	{
	  s2 = s->next;
	  fts_outlet_int(o, 1, 0L);
	  fts_outlet_int(o, 0, s->pitch);
	  fts_free(s);
	}

      this->sustained_notes = 0;
    }
}

static void
sustain_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;

  if ((ac >= 3) && fts_is_number(&at[2]))
    sustain_number_2(o, winlet, s, 1, at+2);

  if ((ac >= 2) && fts_is_number(&at[1]))
    this->vel = (long) fts_get_int_arg(ac, at, 1, 0);

  if ((ac >= 1) && fts_is_number(&at[0]))
    sustain_number(o, winlet, s, 1, at);
}


/* The clear method is also installed for the $delete message */

static void
sustain_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;
  struct slink *s1, *s2;

  for (s1 = this->sustained_notes; s1; s1 = s2)
    {
      s2 = s1->next;
      fts_free(s1);
    }

  this->sustained_notes = 0;
}


static void
sustain_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sustain_t *this = (sustain_t *)o;

  this->sustained_notes = 0;
  this->vel   = fts_get_long_arg(ac, at, 1, 0);
  this->sust  = fts_get_long_arg(ac, at, 2, 0);
}


static fts_status_t
sustain_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[10];

  /* initialize the class */

  fts_class_init(cl, sizeof(sustain_t), 3, 2, 0); 

  /* system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int  ;
  a[2] = fts_s_int  ;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sustain_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sustain_clear, 0, 0); /* sustain_clear is
										installed twice */
  /* Sustain methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sustain_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sustain_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, sustain_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, sustain_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, sustain_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, sustain_number_2, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, sustain_list);

  fts_method_define(cl, 0, fts_new_symbol("clear"), sustain_clear, 0, 0);

  /* Type the outlets */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_int, 1, a);

  return fts_Success;
}


void
sustain_config(void)
{
  fts_metaclass_create(fts_new_symbol("sustain"),sustain_instantiate, fts_always_equiv);
}
