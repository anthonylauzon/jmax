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

#ifndef _FTS_PRIVATE_AUDIO_H_
#define _FTS_PRIVATE_AUDIO_H_

/* **********************************************************************
 *
 * audio label private API
 *
 */

extern fts_class_t *fts_audiolabel_class;

/* **********************************************************************
 *
 * audio manager private API
 *
 */

/* Returns a NULL terminated array of input audio port names */
extern fts_symbol_t *fts_audiomanager_get_input_names(void);

/* Returns a NULL terminated array of input audio port names */
extern fts_symbol_t *fts_audiomanager_get_output_names(void);


#endif
