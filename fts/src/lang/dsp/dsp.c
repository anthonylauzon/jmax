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


/*
   Temporary definition of the dsp chain module.
   
   Soon to be reorganized.

*/

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"


/******************************************************************************/
/*                                                                            */
/*             DSPCHAIN SubSystem Declaration                                 */
/*                                                                            */
/******************************************************************************/

extern void dsp_compiler_init(void);
extern void dsp_install_clocks(void);
static void dsp_module_init(void);
static void dsp_module_shutdown(void);
extern void fts_dsp_control_config(void);
extern void Sig_init(void);

fts_symbol_t fts_s_put;
fts_symbol_t fts_s_sig_zero;
fts_symbol_t fts_s_dsp_descr;

/* up and down sampling factors are now specified with properties */
fts_symbol_t fts_s_dsp_upsampling;
fts_symbol_t fts_s_dsp_downsampling;
fts_symbol_t fts_s_dsp_outputsize; /* give an absolute size for output vector */

fts_module_t fts_dsp_module = {"Dsp", "Dsp compiler", dsp_module_init, dsp_module_shutdown, 0};

static void
dsp_module_init(void)
{
  dsp_install_clocks();

  /* symbols used in DSP */
  fts_s_put = fts_new_symbol("put");
  fts_s_sig_zero = fts_new_symbol("_sig_0");
  fts_s_dsp_upsampling   = fts_new_symbol("DSP_UPSAMPLING");
  fts_s_dsp_downsampling = fts_new_symbol("DSP_DOWNSAMPLING");
  fts_s_dsp_outputsize = fts_new_symbol("DSP_OUTPUTSIZE");
  fts_s_dsp_descr = fts_new_symbol("__DSP_DESCR");

  /* Initialize signals management */
  Sig_init();

  /* Initialize DSP compilation unit */
  dsp_compiler_init();

  /* Make the dsp off program  */
  dsp_make_dsp_off_chain();

  /* Initialize the dsp control data object */

  fts_dsp_control_config();
}


static void
dsp_module_shutdown(void)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}


/*
  (fd) Wonderfull !!! Magnifique !!!
*/

struct fts_dev;
typedef struct fts_dev fts_dev_t;

static fts_dev_t *dac_slip_device;

void fts_dsp_set_dac_slip_dev(fts_dev_t *dev)
{
  dac_slip_device = dev;
}

fts_dev_t * fts_dsp_get_dac_slip_dev(void)
{
  return dac_slip_device;
}
