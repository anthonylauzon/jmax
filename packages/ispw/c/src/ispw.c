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
 */

#include "ispw.h"

/* control */
extern void ispw_naming_init(void);
extern void bangbang_config(void);
extern void change_config(void);
extern void delay_config(void);
extern void drunk_config(void);
extern void gate_config(void);
extern void ispwcomp_config(void);
extern void line_config(void);
extern void loadbang_config(void);
extern void atom_list_config(void);
extern void messbox_config(void);
extern void messbox_doctor_init(void);
extern void metro_config(void);
extern void nbangs_config(void);
extern void pack_config(void);
extern void pipe_config(void);
extern void poly_config(void);
extern void prepend_config(void);
extern void random_config(void);
extern void route_config(void);
extern void select_config(void);
extern void speedlim_config(void);
extern void split_config(void);
extern void timer_config(void);
extern void trigger_config(void);
extern void unpack_config(void);
extern void voxalloc_config(void);
extern void abs_config(void);
extern void clip_config(void);
extern void exscale_config(void);
extern void ftom_config(void);
extern void logscale_config(void);
extern void mtof_config(void);
extern void scale_config(void);

/* data */
extern void accum_config(void);
extern void bag_config(void);
extern void float_config(void);
extern void symbol_obj_config(void);
extern void funbuff_config(void);
extern void integer_config(void);
extern void pbank_config(void);
extern void table_config(void);
extern void ispw_value_config(void);
extern void expr_config(void);
extern void expr_doctor_init(void);

/* filters */
extern void biquad_config(void);
extern void sigcoef_bpass2_config(void);
extern void sigcoef_hlshelf1_config(void);
extern void iir_config(void);
extern void sigapass3_config(void);
extern void wahwah_config(void);

/* io */
extern void dac_tilda_config(void);
extern void adc_tilda_config(void);

/* midi */
extern void makenote_config(void);
extern void stripnote_config(void);
extern void sustain_config(void);
extern void Rreceive_config(void);
extern void Rsend_config(void);
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
extern void zerocross_config(void);
extern void sigthru_config(void);
extern void sigthrow_config(void);
extern void sigthreshold_config(void);
extern void sigswitch_config(void);
extern void sigsnapshot_config(void);
extern void sigline_config(void);
extern void sig_config(void);
extern void sigdown_config(void);
extern void sigup_config(void);
extern void phasor_config(void);
extern void osc_config(void);
extern void sigtab1_config(void);
extern void noise_config(void);
extern void sigparam_config(void);
extern void sigprint_config(void);
extern void vectrigon_config(void);
extern void vecsqrt_config(void);
extern void veclog_config(void);
extern void vecclip_config(void);
extern void sigarctan2_config(void);
extern void sigabs_config(void);
extern void fft_config(void);
extern void sigsamphold_config(void);



void
ispw_config(void)
{
  /* control init */
  ispw_naming_init();
  bangbang_config();
  change_config();
  delay_config();
  drunk_config();
  gate_config();
  ispwcomp_config();
  line_config();
  loadbang_config();
  atom_list_config();
  messbox_config();
  messbox_doctor_init();
  metro_config();
  nbangs_config();
  pack_config();
  pipe_config();
  poly_config();
  prepend_config();
  random_config();
  route_config();
  select_config();
  speedlim_config();
  split_config();
  timer_config();
  trigger_config();
  unpack_config();
  voxalloc_config();
  abs_config();
  clip_config();
  exscale_config();
  ftom_config();
  logscale_config();
  mtof_config();
  scale_config();

  /* data init */
  accum_config();
  bag_config();
  float_config();
  integer_config();
  symbol_obj_config();
  funbuff_config();
  pbank_config();
  table_config();
  ispw_value_config();

  /* Note: expr is here because it needs to be in the same
     .so library than table

     The expr doctor evaluating new expressions.
     */

  expr_config();
  expr_doctor_init();

  /* filters */
  biquad_config();
  sigcoef_bpass2_config();
  sigcoef_hlshelf1_config();
  iir_config();
  sigapass3_config();
  wahwah_config();

  /* io */
  dac_tilda_config();
  adc_tilda_config();

  /* midi */
  makenote_config();
  stripnote_config();
  sustain_config();
  Rreceive_config();
  Rsend_config();
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
  zerocross_config();
  sigthru_config();
  sigthrow_config();
  sigthreshold_config();
  sigswitch_config();
  sigsnapshot_config();
  sigline_config();
  sig_config();
  sigdown_config();
  sigup_config();
  phasor_config();
  osc_config();
  sigtab1_config();
  noise_config();
  sigparam_config();
  sigprint_config();
  vectrigon_config();
  vecsqrt_config();
  veclog_config();
  vecclip_config();
  sigarctan2_config();
  sigabs_config();
  fft_config();
  sigsamphold_config();

}


