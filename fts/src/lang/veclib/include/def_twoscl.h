/* define/twoscl0.h */

#include "lang/veclib/include/unroll.h"
#include "lang/veclib/include/unwrap.h"


/***************************************************
 *
 *    fisrst made for clip
 *
 *      VECTOR (SCALAR SCALAR) -> VECTOR
 *      prefetches elements of input vector to temporary variables (hopefully registers)
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */

#define NO_UNROLL_TWOSCL(op) op(in_0, scl0, scl1, out[i]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_TWOSCL(op)\
  op(in_0, scl0, scl1, out[i]);\
  op(in_1, scl0, scl1, out[i+1]);\
  op(in_2, scl0, scl1, out[i+2]);\
  op(in_3, scl0, scl1, out[i+3]);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_TWOSCL(op)\
  op(in_0, scl0, scl1, out[i]);\
  op(in_1, scl0, scl1, out[i+1]);\
  op(in_2, scl0, scl1, out[i+2]);\
  op(in_3, scl0, scl1, out[i+3]);\
  op(in_4, scl0, scl1, out[i+4]);\
  op(in_5, scl0, scl1, out[i+5]);\
  op(in_6, scl0, scl1, out[i+6]);\
  op(in_7, scl0, scl1, out[i+7]);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_TWOSCL(op)\
  op(in_0, scl0, scl1, out[i]);\
  op(in_1, scl0, scl1, out[i+1]);\
  op(in_2, scl0, scl1, out[i+2]);\
  op(in_3, scl0, scl1, out[i+3]);\
  op(in_4, scl0, scl1, out[i+4]);\
  op(in_5, scl0, scl1, out[i+5]);\
  op(in_6, scl0, scl1, out[i+6]);\
  op(in_7, scl0, scl1, out[i+7]);\
  op(in_8, scl0, scl1, out[i+8]);\
  op(in_9, scl0, scl1, out[i+9]);\
  op(in_10, scl0, scl1, out[i+10]);\
  op(in_11, scl0, scl1, out[i+11]);\
  op(in_12, scl0, scl1, out[i+12]);\
  op(in_13, scl0, scl1, out[i+13]);\
  op(in_14, scl0, scl1, out[i+14]);\
  op(in_15, scl0, scl1, out[i+15]);
 
#else

#define UNROLL_TWOSCL(op) NO_UNROLL_TWOSCL(op)

#endif


/***************************************************
 *
 *              function bodies
 *
 */
 
#define FTS_VEC_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_REGS(t_in, in)\
  NO_UNROLL_IN(in)\
  NO_UNROLL_TWOSCL(op)\
 )

#define FTS_VECX_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)\
 UNROLL_LOOP(\
  UNROLL_REGS(t_in, in)\
  UNROLL_IN(in)\
  UNROLL_TWOSCL(op)\
 )

#define FTL_UNWRAP_TWOSCL(t_in, t_scl0, t_scl1, t_out)\
 UNWRAP_VEC(0, t_in, in)\
 UNWRAP_SCL(1, t_scl0, scl0)\
 UNWRAP_SCL(2, t_scl1, scl1)\
 UNWRAP_VEC(3, t_out, out)\
 UNWRAP_SIZE(4)
 
 
/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 void fts_vec_ ## name(t_in *in, t_scl0 scl0, t_scl1 scl1, t_out *out, long size)\
 {FTS_VEC_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)}
  
#define DEFINE_FTS_VECX_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 void fts_vecx_ ## name(t_in *in, t_scl0 scl0, t_scl1 scl1, t_out *out, long size)\
 {FTS_VECX_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)}

#define DEFINE_FTL_VEC_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_TWOSCL(t_in, t_scl0, t_scl1, t_out)\
  FTS_VEC_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)\
 }

#define DEFINE_FTL_VECX_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_TWOSCL(t_in, t_scl0, t_scl1, t_out)\
  FTS_VECX_TWOSCL(t_in, t_scl0, t_scl1, t_out, op)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 extern void fts_vec_ ## name(t_in *in, t_scl0 scl0, t_scl1 scl1, t_out *out, long size);
 
#define DECLARE_FTS_VECX_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 extern void fts_vecx_ ## name(t_in *in, t_scl0 scl0, t_scl1 scl1, t_out *out, long size);
 
#define DECLARE_FTL_VEC_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 extern void ftl_vec_ ## name(FTL_ARG);
 
#define DECLARE_FTL_VECX_TWOSCL(name, t_in, t_scl0, t_scl1, t_out, op)\
 extern void ftl_vecx_ ## name(FTL_ARG);

