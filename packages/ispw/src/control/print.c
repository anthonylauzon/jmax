
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:58 $
 *
 */

/* WARNIG: code changed for compatibility with MAX Opcode; 
   exactly one post event is invoked for every message
   Also, printing is smoother on X window Max.
*/

#include <string.h>
#include "fts.h"

typedef struct {
  fts_object_t o;
  fts_symbol_t prompt;
} print_t;


static fts_symbol_t sym_print = 0;


static void
print_bang_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: bang\n", fts_symbol_name(this->prompt));
}


static void
print_list_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: [ ", fts_symbol_name(this->prompt));
  postatoms(ac, at);
  post("]\n");
}

static void
print_atoms_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: ", fts_symbol_name(this->prompt));
  postatoms(ac, at);
  post("\n");
}


static void
print_anything_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: ", fts_symbol_name(this->prompt));

  if (ac)
    {
      post("%s ", fts_symbol_name(s));
      postatoms( ac, at);
      post("\n");
    }
  else
    post("%s\n", fts_symbol_name(s));
}

static void
print_init_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  this->prompt      = fts_get_symbol_arg(ac, at, 1, fts_new_symbol("print"));
}


static fts_status_t
print_instantiate(fts_class_t *cl, int ac, const fts_atom_t *aat)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(print_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, print_init_mth, 2, a, 1);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, print_atoms_mth, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, print_atoms_mth, 1, a);

  fts_method_define(cl, 0, fts_s_bang, print_bang_mth, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, print_atoms_mth, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, print_list_mth);

  fts_method_define_varargs(cl, 0, fts_s_anything, print_anything_mth);

  return fts_Success;
}

void
print_config(void)
{
  fts_metaclass_create(fts_new_symbol("print"),print_instantiate, fts_always_equiv);
}

