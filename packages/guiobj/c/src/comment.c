/*
 * jMax
 * Copyright (C) 1999 by IRCAM
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
  fts_class_install(fts_new_symbol("jcomment"), comment_instantiate);
}

