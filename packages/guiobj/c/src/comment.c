/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/*
 * The comment object is just a place holder for a comment property; it does
 * absolutely nothing; it is called jcomment, because the comment doctor
 * translate from old comments to new ones (i.e. comment text move from
 * arguments to the propriety).
 * The property is stored as normal property, no daemons used.
 */

#include "fts.h"


typedef struct
{
  fts_object_t o;

} comment_t;


static fts_status_t
comment_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(comment_t), 0, 0, 0);

  return fts_Success;
}

void
comment_config(void)
{
  fts_metaclass_create(fts_new_symbol("jcomment"), comment_instantiate, fts_always_equiv);
}

