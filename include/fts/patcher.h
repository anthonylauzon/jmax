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

#ifndef _FTS_PATCHER_H_
#define _FTS_PATCHER_H_

struct fts_patcher
{
  fts_object_t o;

  fts_inlet_t  **inlets;	/* the patcher inlet array */
  fts_outlet_t **outlets;	/* the patcher outlet array */
  fts_object_t *objects;	/* the patcher content, organized as a list */

  int open;			/* the open flag */
  int load_init_fired;		/* the multiple load init protection flag*/
  int deleted;			/* set to one during content deleting */

  fts_array_t *args;	/* the arguments used for the "args" variable */

  enum {fts_p_standard, fts_p_abstraction, fts_p_error, fts_p_template} type;

  /* If this patcher is a template, point to the template definition */
  fts_template_t *template;

  /* The pointer to the patcher data  */
  fts_patcher_data_t *data;

  /* Variables */
  fts_env_t env;
};    

#define fts_patcher_get_args(p)    ((p)->args)
#define fts_patcher_is_open(p)     ((p)->open)

FTS_API fts_patcher_t *fts_get_root_patcher(void);

#endif