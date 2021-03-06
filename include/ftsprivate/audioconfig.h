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

extern fts_class_t* fts_audioconfig_class;

extern void fts_audioconfig_set_defaults( fts_audioconfig_t* config);
extern void fts_audioconfig_dump( fts_audioconfig_t *mc, fts_bmax_file_t *f);
extern fts_symbol_t fts_audioconfig_get_fresh_label_name( fts_audioconfig_t *config, fts_symbol_t name);
extern fts_audiolabel_t *fts_audioconfig_label_get_by_name( fts_audioconfig_t* config, fts_symbol_t name);
extern int audioconfig_check_sample_rate(fts_audioconfig_t* config, double sample_rate);
extern int audioconfig_check_buffer_size(fts_audioconfig_t* config, int buffer_size);
extern int fts_audioconfig_get_buffer_size(fts_audioconfig_t* self);
extern int fts_audioconfig_set_buffer_size(fts_audioconfig_t* config, int buffer_size);
extern int fts_audioconfig_get_sample_rate(fts_audioconfig_t* config);
extern int fts_audioconfig_set_sample_rate(fts_audioconfig_t* config, int sample_rate);

#endif /* _FTS_PRIVATE_AUDIOCONFIG_H_ */


