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

#ifndef _FTS_PRIVATE_AUDIOCONFIG_H_
#define _FTS_PRIVATE_AUDIOCONFIG_H_

/* Requires audiolabel */

typedef struct fts_audioconfig {
  fts_object_t o;
  fts_audiolabel_t* labels;
  int n_labels;    

  int buffer_size;
  int sample_rate;

} fts_audioconfig_t;

extern fts_class_t* audioconfig_type;

extern void fts_audioconfig_set_defaults( fts_audioconfig_t* config);
extern void fts_audioconfig_dump( fts_audioconfig_t *mc, fts_bmax_file_t *f);
extern fts_symbol_t fts_audioconfig_get_fresh_label_name( fts_audioconfig_t *config, fts_symbol_t name);
extern fts_audiolabel_t *fts_audioconfig_label_get_by_name( fts_audioconfig_t* config, fts_symbol_t name);

#endif /* _FTS_PRIVATE_AUDIOCONFIG_H_ */


