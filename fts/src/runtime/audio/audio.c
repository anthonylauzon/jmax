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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include <fts/fts.h>

extern void audioport_config( void);
extern void nullaudioport_config( void);

fts_symbol_t fts_s_fifo_size = 0;

void
fts_audio_idle(void)
{
  fts_audioport_idle(0);
}

static void 
audio_init( void)
{
  fts_s_fifo_size = fts_new_symbol("fifo_size");

  nullaudioport_config(); /* must be ***before** audioport_config()!!! */
  audioport_config();
}

fts_module_t fts_audio_module = {"FTS Audio", "FTS Audio Input Output system ", audio_init, 0, 0};
