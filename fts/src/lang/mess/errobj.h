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

#ifndef _ERROBJ_H_
#define _ERROBJ_H_

extern fts_object_t *fts_error_object_new(fts_patcher_t *parent, int ac, const fts_atom_t *at,
					  const char *format, ...);

extern void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet);
extern void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet);
extern void fts_recompute_errors(void);
extern void fts_do_recompute_errors(void);

extern void fts_object_set_error(fts_object_t *obj, const char *format, ...);
#endif
