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

#ifndef _FTS_PRIVATE_CONFIG_H_
#define _FTS_PRIVATE_CONFIG_H_ 1

typedef struct {
  fts_object_t o;
  fts_symbol_t file_name;
  fts_midiconfig_t* midi_config;
  fts_audioconfig_t* audio_config;
  int dirty;
} fts_config_t;

extern fts_class_t* config_type;

extern fts_object_t *fts_config_get( void);
extern fts_object_t *fts_audioconfig_get( void);
extern fts_object_t *fts_midiconfig_get( void);

extern void fts_config_set( fts_config_t* config);
extern void fts_config_set_dirty( fts_config_t* config, int is_dirty);

#endif /* _FTS_PRIVATE_CONFIG_H_ */
