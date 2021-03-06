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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_FPE_H_
#define _FTS_PRIVATE_FPE_H_

extern void fts_fpe_add_object( fts_object_t *object);
extern void fts_fpe_handler( int which);

/* Commented by : alex_p (16/01/04)
   Reason : these 3 methods are allready in 'include\fts\fpe.h'

   extern void fts_fpe_start_collect( fts_objectset_t *set);
   extern void fts_fpe_stop_collect( void);
   extern void fts_fpe_empty_collection( void);
*/

#endif
