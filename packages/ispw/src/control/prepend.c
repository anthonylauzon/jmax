
/* class prepend */

#include "fts.h"
#include <math.h>

enum {
  ilow = 0,
  ihi,
  olow,
  ohi,
  logincr,
  nvalues
};

typedef struct prepend_t {
  fts_object_t _o;
  fts_symbol_t presym;
  int ac;
  fts_atom_t *at;
} prepend_t;


#define max_prepend 32

static void
prepend_set_realize(fts_object_t *o, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;

  if (x->at)
    fts_free(x->at);

  if (ac >= max_prepend)
    ac = max_prepend;

  x->presym = 0;

  if (ac && fts_is_symbol(at))
    {
      x->presym = fts_get_symbol(at);
      at++;
      ac--;
    }

  if (ac)
    {
      x->ac = ac;
      x->at = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t)*ac);
      memcpy(x->at, at, sizeof(fts_atom_t)*ac);
    }
  else
    {
      x->ac = 0;
      x->at = 0;
    }
}

static void
prepend_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  prepend_set_realize(o, ac-1, at+1);
}

static void
prepend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;

  if (x->at)
    fts_free(x->at);
}

static void
prepend_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_set_realize(o, ac, at);
}

static void
prepend_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;
  fts_atom_t sat[max_prepend];
  int n;

  if (ac + x->ac >= max_prepend)
    {
      post("prepend: input list too long (total %d max)\n", max_prepend);
      ac = max_prepend - x->ac;
    }

  n = ac + x->ac;
  memcpy(&sat[0],     x->at, x->ac * sizeof(fts_atom_t));
  memcpy(&sat[x->ac], at,    ac    * sizeof(fts_atom_t));

  if (x->presym)
    fts_outlet_send(o, 0, x->presym, n, sat);
  else
    fts_outlet_send(o, 0, fts_s_list, n, sat);
}

/* As list, but prepend the selector; shadock shadock, but for compatibility 
   with Max 0.26
*/

static void
prepend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;
  fts_atom_t sat[max_prepend];
  int n;

  if (ac + x->ac + 1 >= max_prepend)
    {
      post("prepend: input list too long (total %d max)\n", max_prepend);
      ac = max_prepend - x->ac - 1;
    }

  n = ac + x->ac + 1;
  memcpy(&sat[0],     x->at, x->ac * sizeof(fts_atom_t));
  fts_set_symbol(&sat[x->ac], s);
  memcpy(&sat[x->ac + 1], at,    ac    * sizeof(fts_atom_t));

  if (x->presym)
    fts_outlet_send(o, 0, x->presym, n, sat);
  else
    fts_outlet_send(o, 0, fts_s_list, n, sat);
}

static fts_status_t
prepend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];
  
  fts_class_init(cl, sizeof(prepend_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, prepend_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, prepend_delete, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, prepend_list, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, prepend_list, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, prepend_list, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_set, prepend_set);
  fts_method_define_varargs(cl, 0, fts_s_list, prepend_list);
  fts_method_define_varargs(cl, 0, fts_s_anything, prepend_anything);

  return fts_Success;
}

void
prepend_config(void)
{
  fts_metaclass_create(fts_new_symbol("prepend"),prepend_instantiate, fts_arg_equiv);
}
