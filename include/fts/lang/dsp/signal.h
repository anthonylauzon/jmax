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

#ifndef _FTS_DSP_SIGNAL_H_
#define _FTS_DSP_SIGNAL_H_

typedef struct _fts_dsp_signal_
{
  fts_symbol_t name;
  int refcnt;
  int length;
  float srate;
} fts_dsp_signal_t;

FTS_API fts_dsp_signal_t *fts_dsp_signal_new( int vector_size, float sample_rate);
FTS_API void fts_dsp_signal_free( fts_dsp_signal_t *sig);
FTS_API int fts_dsp_signal_is_pending( fts_dsp_signal_t *sig);
FTS_API void fts_dsp_signal_unreference(fts_dsp_signal_t *sig);
FTS_API void fts_dsp_signal_reference(fts_dsp_signal_t *sig);
FTS_API void fts_dsp_signal_print(fts_dsp_signal_t *sig);
FTS_API void fts_dsp_signal_init( void);

#endif
