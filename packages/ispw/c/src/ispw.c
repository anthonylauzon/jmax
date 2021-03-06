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

#include "ispw.h"

/* control */
extern void ispw_naming_init(void);
extern void bangbang_config(void);
extern void ispwcomp_config(void);
extern void line_config(void);
extern void loadbang_config(void);
extern void atom_list_config(void);
extern void messbox_config(void);
extern void nbangs_config(void);
extern void pack_config(void);
extern void pipe_config(void);
extern void poly_config(void);
extern void prepend_config(void);
extern void gate_config(void);
extern void route_config(void);
extern void split_config(void);
extern void trigger_config(void);
extern void unpack_config(void);
extern void voxalloc_config(void);
extern void scale_config(void);

/* data */
extern void accum_config(void);
extern void bag_config(void);
extern void int_config(void);
extern void explay_config(void);
extern void explode_config(void);
extern void float_config(void);
extern void funbuff_config(void);
extern void pbank_config(void);
extern void qlist_config(void);
extern void table_config(void);
extern void ispw_value_config(void);

/* expression */
extern void expr_config(void);

/* filters */
extern void sigcoef_bpass2_config(void);
extern void sigcoef_hlshelf1_config(void);
extern void sig2p2z_config(void);
extern void sigapass3_config(void);

/* io */
extern void dac_tilda_config(void);
extern void adc_tilda_config(void);

/* midi */
extern void makenote_config(void);
extern void stripnote_config(void);
extern void sustain_config(void);
extern void mididecrypt_config( void);

/* sampling */
extern void fts_fourpoint_init(void);
extern void sampbuf_table_init(void);
extern void sigtable_config(void);
extern void sampread_config(void);
extern void sampwrite_config(void);
extern void samppeek_config(void);
extern void tabpoke_config(void);
extern void tabpeek_config(void);
extern void tabcycle_config(void);
extern void delay_table_init(void);
extern void delwrite_config(void);
extern void delread_config(void);
extern void vd_config(void);
extern void samples_unit_init(void);

/* pitch */
extern void pitch_config(void);
extern void pt_config(void);

/* signal */
extern void sigthru_config(void);
extern void sigswitch_config(void);
extern void sigline_config(void);
extern void sig_config(void);
extern void sigdown_config(void);
extern void sigup_config(void);
extern void ispw_osc_config(void);
extern void noise_config(void);
extern void sigparam_config(void);
extern void vectrigon_config(void);
extern void vecsqrt_config(void);
extern void veclog_config(void);
extern void sigarctan2_config(void);
extern void fft_ispw_config(void);



void
ispw_config(void)
{
  /* control init */
  ispw_naming_init();
  bangbang_config();
  ispwcomp_config();
  line_config();
  loadbang_config();
  atom_list_config();
  messbox_config();
  nbangs_config();
  pack_config();
  pipe_config();
  poly_config();
  prepend_config();
  gate_config();
  route_config();
  split_config();
  trigger_config();
  unpack_config();
  voxalloc_config();
  scale_config();

  /* data init */
  accum_config();
  bag_config();
  int_config();
  explay_config();
  explode_config();
  float_config();
  funbuff_config();
  pbank_config();
  qlist_config();
  table_config();
  ispw_value_config();

  /* expr */
  expr_config();

  /* filters */
  sigcoef_bpass2_config();
  sigcoef_hlshelf1_config();
  sig2p2z_config();
  sigapass3_config();

  /* io */
  dac_tilda_config();
  adc_tilda_config();

  /* midi */
  makenote_config();
  stripnote_config();
  sustain_config();
  mididecrypt_config();

  /* sampling */
  fts_fourpoint_init();
  sampbuf_table_init();  
  sigtable_config();
  sampwrite_config();
  sampread_config();
  samppeek_config();
  tabpeek_config();
  tabpoke_config();
  tabcycle_config();
  delay_table_init();
  delwrite_config();
  delread_config();
  vd_config();
  samples_unit_init();

  /* pitch */
  pitch_config();
  pt_config();

  /* signal */
  sigthru_config();
  sigswitch_config();
  sigline_config();
  sig_config();
  sigdown_config();
  sigup_config();
  ispw_osc_config();
  noise_config();
  sigparam_config();
  vectrigon_config();
  vecsqrt_config();
  veclog_config();
  sigarctan2_config();
  fft_ispw_config();
}


