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
static void dsp_module_restart(void);
static void dsp_module_shutdown(void);


fts_symbol_t fts_s_put;
fts_symbol_t fts_s_sig;
fts_symbol_t fts_s_sig_zero;
fts_symbol_t fts_s_dsp_descr;

/* up and down sampling factors are now specified with properties */

fts_symbol_t fts_s_dsp_upsampling;
fts_symbol_t fts_s_dsp_downsampling;
fts_symbol_t fts_s_dsp_outputsize; /* give an absolute size for output vector */

fts_module_t fts_dsp_module = {"Dsp", "Dsp compiler", dsp_module_init, dsp_module_restart, dsp_module_shutdown};

static void
dsp_module_init(void)
{
  dsp_install_clocks();

  /* symbols used in DSP */
  fts_s_put = fts_new_symbol("put");
  fts_s_sig = fts_new_symbol("sig");
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
}


static void
dsp_module_restart(void)
{
  dsp_chain_delete();  
}


static void
dsp_module_shutdown(void)
{
  dsp_chain_delete();
}

/* The actual sampling rate is only known to the dsp compiler,
   so it is handled here */
  
static float fts_sr = 44100.0;		


void
fts_dsp_set_sampling_rate(float sr)
{
  fts_sr = sr;

}


float 
fts_dsp_get_sampling_rate(void)
{
  return fts_sr;
}

int
fts_dsp_get_vector_size(void)
{
  return 64;			/* soon to change, and to be device relative !!! */
}

