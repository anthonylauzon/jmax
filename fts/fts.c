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

#include <fts/fts.h>

extern void fts_kernel_symbol_init( void);
extern void fts_kernel_hashtable_init( void);
extern void fts_kernel_atom_init( void);
extern void fts_kernel_objtable_init( void);
extern void fts_kernel_class_init( void);
extern void fts_kernel_doctor_init( void);
extern void fts_kernel_patcher_init( void);
extern void fts_kernel_audio_init( void);
extern void fts_kernel_bytestream_init( void);
extern void fts_kernel_autosave_init( void);
extern void fts_kernel_clipboard_init( void);
extern void fts_kernel_dsp_graph_init( void);
extern void fts_kernel_dsp_init( void);
extern void fts_kernel_expression_init( void);
extern void fts_kernel_ftl_init( void);
extern void fts_kernel_midi_init( void);
extern void fts_kernel_objtable_init( void);
extern void fts_kernel_param_init( void);
extern void fts_kernel_patparser_init( void);
extern void fts_kernel_property_init( void);
extern void fts_kernel_sched_init( void);
extern void fts_kernel_selection_init( void);
extern void fts_kernel_soundfile_init( void);
extern void fts_kernel_template_init( void);
extern void fts_kernel_variable_init( void);
extern void fts_kernel_oldclient_init( void);
extern void fts_kernel_oldftsdata_init( void);
extern void fts_kernel_oldpatcherdata_init( void);

void fts_init( void)
{
  /* *** Attention !!! The order is important *** */
  fts_kernel_symbol_init();
  fts_kernel_hashtable_init();
  fts_kernel_atom_init();
  fts_kernel_objtable_init();
  fts_kernel_class_init();
  fts_kernel_doctor_init();
  fts_kernel_patcher_init();

  /* For the rest, the order is no longer important */
  fts_kernel_audio_init();
  fts_kernel_bytestream_init();
  fts_kernel_autosave_init();
  fts_kernel_clipboard_init();
  fts_kernel_dsp_graph_init();
  fts_kernel_dsp_init();
  fts_kernel_expression_init();
  fts_kernel_ftl_init();
  fts_kernel_midi_init();
  fts_kernel_objtable_init();
  fts_kernel_param_init();
  fts_kernel_patparser_init();
  fts_kernel_property_init();
  fts_kernel_sched_init();
  fts_kernel_selection_init();
  fts_kernel_soundfile_init();
  fts_kernel_template_init();
  fts_kernel_variable_init();

  fts_kernel_oldclient_init();
  fts_kernel_oldftsdata_init();
  fts_kernel_oldpatcherdata_init();
}
