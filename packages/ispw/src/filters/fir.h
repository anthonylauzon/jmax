#include "fts.h"


/****************************
 *  
 *  FTL nonrecursive filter functions
 *
 *    ftl_fir_1 ... 1st order
 *      float *in
 *      float *out
 *      iir_1_state_t *state
 *      iir_1_coefs_t *coefs
 *      long n_tick
 *
 *    ftl_fir_2 ... 2nd order
 *      float *in
 *      float *out
 *      iir_2_state_t *state
 *      iir_2coefs_t *coefs
 *      long n_tick
 *
 *    ftl_fir_n ... nth order
 *      float *in
 *      float *out
 *      iir_n_state_t *state
 *      iir_n_coefs_t *coefs
 *      long n_order
 *      long n_tick
 *
 *
 */

extern void ftl_fir_1(fts_word_t *argv);
extern void ftl_fir_2(fts_word_t *argv);
extern void ftl_fir_n(fts_word_t *argv);

