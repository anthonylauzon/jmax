/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:54 $
 *
 */

#include "fts.h"

/*------------------------- gfloat class -------------------------------------*/

typedef struct {
  fts_object_t o;
} ucs_t;



static void
ucs_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_ucs_execute_command_opcode(s, ac, at);
}

static fts_status_t
ucs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(ucs_t), 1, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, ucs_anything);

  return fts_Success;
}

void
ucs_config(void)
{
  fts_metaclass_create(fts_new_symbol("ucs"),ucs_instantiate, fts_always_equiv);
}

