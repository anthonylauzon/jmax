/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
midi_module_init(void)
{
  Rreceive_config();
  Rsend_config();
  makenote_config();
  midi_config();
  mididecrypt_config();
  midiformat_config();
  midiparse_config();
  stripnote_config();
  sustain_config();
}

fts_module_t midi_module = {"midi", "ISPW midi classes", midi_module_init};
