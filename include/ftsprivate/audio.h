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

#ifndef _FTS_PRIVATE_AUDIO_H_
#define _FTS_PRIVATE_AUDIO_H_

/* **********************************************************************
 *
 * audio port private api
 * 
 */

extern void fts_audioport_set_channel_used( fts_audioport_t *port, int direction, int channel, int used);
extern int fts_audioport_is_channel_used( fts_audioport_t *port, int direction, int channel);

extern void fts_audioport_add_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label);
extern void fts_audioport_remove_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label);

#define fts_audioport_get_io_fun( port, direction) ((port)->inout[(direction)].io_fun)
#define fts_audioport_get_copy_fun( port, direction) ((port)->inout[(direction)].copy_fun)
#define fts_audioport_get_mute_fun( port, direction) ((port)->inout[(direction)].mute_fun)

#define fts_audioport_is_direction( port, direction) ((port)->inout[(direction)].valid)
#define fts_audioport_is_input( port) fts_audioport_is_direction( (port), FTS_AUDIO_INPUT)
#define fts_audioport_is_output( port) fts_audioport_is_direction( (port), FTS_AUDIO_OUTPUT)


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
