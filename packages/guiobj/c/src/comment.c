/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 * The comment object is just a place holder for comment objects, but do nothing.
 */

#include "fts.h"


typedef struct
{
  fts_object_t o;

} comment_t;

static void
comment_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_set_description_and_class(o, fts_new_symbol("comment"), ac, at);
}


static fts_status_t
comment_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(comment_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, comment_set);

  return fts_Success;
}

void
comment_config(void)
{
  fts_metaclass_create(fts_new_symbol("comment"), comment_instantiate, fts_always_equiv);
}

