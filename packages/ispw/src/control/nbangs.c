
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/09/19 14:36:10 $
 *
 */

#include "fts.h"

/*------------------------- gint class -------------------------------------*/

typedef struct
{
  fts_object_t o;
  long count;
} nbangs_t;



static void
nbangs_any(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;
  int i;
    
  for (i = 0; i < this->count; i++)
    {
      fts_outlet_int(o, 1, i);
      fts_outlet_bang(o, 0);
    }
}


static void
nbangs_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;
  int i;

  if ((ac >= 2) && fts_is_number(&at[1]))
    this->count = fts_get_number(&at[1]);
    
  for (i = 0; i < this->count; i++)
    {
      fts_outlet_int(o, 1, i);
      fts_outlet_bang(o, 0);
    }
}


static void
nbangs_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  this->count = fts_get_number(at);
}


static void
nbangs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  this->count = fts_get_int_arg(ac, at, 1, 0);
}


static fts_status_t
nbangs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(nbangs_t), 2, 2, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, nbangs_init, 2, a, 1);

  fts_method_define_varargs(cl, 0, fts_s_list, nbangs_list);

  fts_method_define_varargs(cl, 0, fts_s_anything, nbangs_any);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, nbangs_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, nbangs_number_1, 1, a);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_bang, 0, 0);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 1, fts_s_int, 1, a);

  return fts_Success;
}

void
nbangs_config(void)
{
  fts_metaclass_create(fts_new_symbol("nbangs"),nbangs_instantiate, fts_always_equiv);
}

