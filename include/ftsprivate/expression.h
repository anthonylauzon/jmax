/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 */

#ifndef _FTS_PRIVATE_EXPRESSION_H_
#define _FTS_PRIVATE_EXPRESSION_H_

extern void fts_expressions_init(void);



extern int fts_expression_get_result_count(fts_expression_state_t *e);

extern const char *fts_expression_get_msg(fts_expression_state_t *e);
extern const char *fts_expression_get_err_arg(fts_expression_state_t *e);

extern void fts_expression_add_variables_user(fts_expression_state_t *e, fts_object_t *obj);

extern int fts_expression_map_to_assignements(fts_expression_state_t *e, 
					      void (* f)(fts_symbol_t name, fts_atom_t *value, void *data), void *data);

/*
 * non documented 
 */
extern int fts_symbol_get_operator( fts_symbol_t s);
extern void fts_symbol_set_operator( fts_symbol_t s, int op);
extern int fts_symbol_is_operator( fts_symbol_t s);


#endif
