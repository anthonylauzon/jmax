
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:57 $
 *
 * Generic message connection with a client: when receive a message
 * send it to the corresponding Java object, immediatelyu.
 */

#include "fts.h"

/*------------------------- gobj class -------------------------------------*/

typedef struct {
  fts_object_t o;
} gobj_t;


/* The event is raise iff ev and tags are different from 0 */

static void
gobj_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_send_mess(o, s, ac, at);
}

static fts_status_t
gobj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(gobj_t), 1, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, gobj_anything);

  return fts_Success;
}

void
gobj_config(void)
{
  fts_metaclass_create(fts_new_symbol("gobj"),gobj_instantiate, fts_always_equiv);
}

