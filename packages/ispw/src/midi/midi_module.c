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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

extern void Rreceive_config(void);
extern void Rsend_config(void);
extern void makenote_config(void);
extern void midi_config(void);
extern void midiformat_config(void);
extern void midiparse_config(void);
extern void stripnote_config(void);
extern void sustain_config(void);
extern void mididecrypt_config( void);

static void
ispw_midi_module_init(void)
{
  Rreceive_config();
  Rsend_config();
  makenote_config();
  midi_config();
  mididecrypt_config();
  /*midiformat_config();*/
  midiparse_config();
  stripnote_config();
  sustain_config();
}

fts_module_t ispw_midi_module = {"ISPW midi", "ISPW midi classes", ispw_midi_module_init, 0, 0};
