/* define/op.h */

#include "lang/veclib/include/unroll.h"
#include "lang/veclib/include/unwrap.h"


/***************************************************
 *
 *    vector/vector operations
 *
 *      VECTOR (x) VECTOR -> VECTOR
 *      prestores elements of output vector in temporary variables (hopefully registers)
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */

#define NO_UNROLL_OP(op) op(in0[i+0], in1[i+0], out_0);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_OP(op)\
 op(in0[i+0], in1[i+0], out_0);\
 op(in0[i+1], in1[i+1], out_1);\
 op(in0[i+2], in1[i+2], out_2);\
 op(in0[i+3], in1[i+3], out_3);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_OP(op)\
 op(in0[i+0], in1[i+0], out_0);\
 op(in0[i+1], in1[i+1], out_1);\
 op(in0[i+2], in1[i+2], out_2);\
 op(in0[i+3], in1[i+3], out_3);\
 op(in0[i+4], in1[i+4], out_4);\
 op(in0[i+5], in1[i+5], out_5);\
 op(in0[i+6], in1[i+6], out_6);\
 op(in0[i+7], in1[i+7], out_7);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_OP(op)\
 op(in0[i+0], in1[i+0], out_0);\
 op(in0[i+1], in1[i+1], out_1);\
 op(in0[i+2], in1[i+2], out_2);\
 op(in0[i+3], in1[i+3], out_3);\
 op(in0[i+4], in1[i+4], out_4);\
 op(in0[i+5], in1[i+5], out_5);\
 op(in0[i+6], in1[i+6], out_6);\
 op(in0[i+7], in1[i+7], out_7);\
 op(in0[i+8], in1[i+8], out_8);\
 op(in0[i+9], in1[i+9], out_9);\
 op(in0[i+10], in1[i+10], out_10);\
 op(in0[i+11], in1[i+11], out_11);\
 op(in0[i+12], in1[i+12], out_12);\
 op(in0[i+13], in1[i+13], out_13);\
 op(in0[i+14], in1[i+14], out_14);\
 op(in0[i+15], in1[i+15], out_15);
  
#else

#define UNROLL_OP(op) NO_UNROLL_OP(op)

#endif

/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_OP(t_in0, t_in1, t_out, op)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_REGS(t_out, out)\
  NO_UNROLL_OP(op)\
  NO_UNROLL_OUT(out)\
 )

#define FTS_VECX_OP(t_in0, t_in1, t_out, op)\
 UNROLL_LOOP(\
  UNROLL_REGS(t_out, out)\
  UNROLL_OP(op)\
  UNROLL_OUT(out)\
 )

#define FTL_UNWRAP_OP(t_in0, t_in1, t_out)\
 UNWRAP_VEC(0, t_in0, in0)\
 UNWRAP_VEC(1, t_in1, in1)\
 UNWRAP_VEC(2, t_out, out)\
 UNWRAP_SIZE(3)
 

/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_OP(name, t_in0, t_in1, t_out, op)\
 void fts_vec_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, long size)\
 {FTS_VEC_OP(t_in0, t_in1, t_out, op)}
  
#define DEFINE_FTS_VECX_OP(name, t_in0, t_in1, t_out, op)\
 void fts_vecx_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, long size)\
 {FTS_VECX_OP(t_in0, t_in1, t_out, op)}

#define DEFINE_FTL_VEC_OP(name, t_in0, t_in1, t_out, op)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_OP(t_in0, t_in1, t_out)\
  FTS_VEC_OP(t_in0, t_in1, t_out, op)\
 }

#define DEFINE_FTL_VECX_OP(name, t_in0, t_in1, t_out, op)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_OP(t_in0, t_in1, t_out)\
  FTS_VECX_OP(t_in0, t_in1, t_out, op)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_OP(name, t_in0, t_in1, t_out, op)\
 extern void fts_vec_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, long size);
 
#define DECLARE_FTS_VECX_OP(name, t_in0, t_in1, t_out, op)\
 extern void fts_vecx_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, long size);
 
#define DECLARE_FTL_VEC_OP(name, t_in0, t_in1, t_out, op)\
 extern void ftl_vec_ ## name(FTL_ARG);
 
#define DECLARE_FTL_VECX_OP(name, t_in0, t_in1, t_out, op)\
 extern void ftl_vecx_ ## name(FTL_ARG);

