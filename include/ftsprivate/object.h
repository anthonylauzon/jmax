/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_OBJECT_H_
#define _FTS_PRIVATE_OBJECT_H_

/* object status */
#define FTS_OBJECT_STATUS_CREATE ((unsigned long)1)
#define FTS_OBJECT_STATUS_INVALID ((unsigned long)2)
#define FTS_OBJECT_STATUS_PENDING_DELETE ((unsigned long)3)

#define fts_object_get_status(o) ((o)->flag.status)
#define fts_object_set_status(o,f) ((o)->flag.status = (f))


extern fts_object_t *fts_object_new( fts_class_t *cl);
extern void fts_object_free(fts_object_t *obj);
extern void fts_object_reset_client(fts_object_t *obj);

/* client id */
#define fts_object_set_id(o, i) ((o)->flag.id = (i))
#define fts_object_set_client_id(o,i) ((o)->flag.client_id = (i))

#define fts_object_set_definition(o, d) ((o)->definition = (d))
#define fts_object_get_definition(o) ((o)->definition)

#endif
