/* define/vecscl.h */

#include "lang/veclib/include/unroll.h"
#include "lang/veclib/include/unwrap.h"


/***************************************************
 *
 *    scalar vector conversion
 *
 *      VECTOR -> SCALAR
 *      no explicit register allocation
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */
 
#define NO_UNROLL_VECSCL(op) op(in[i+0], scl);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_VECSCL(op)\
 op(in[i+0], scl);\
 op(in[i+1], scl);\
 op(in[i+2], scl);\
 op(in[i+3], scl);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_VECSCL(op)\
 op(in[i+0], scl);\
 op(in[i+1], scl);\
 op(in[i+2], scl);\
 op(in[i+3], scl);\
 op(in[i+4], scl);\
 op(in[i+5], scl);\
 op(in[i+6], scl);\
 op(in[i+7], scl);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_VECSCL(op)\
 op(in[i+0], scl);\
 op(in[i+1], scl);\
 op(in[i+2], scl);\
 op(in[i+3], scl);\
 op(in[i+4], scl);\
 op(in[i+5], scl);\
 op(in[i+6], scl);\
 op(in[i+7], scl);\
 op(in[i+8], scl);\
 op(in[i+9], scl);\
 op(in[i+10], scl);\
 op(in[i+11], scl);\
 op(in[i+12], scl);\
 op(in[i+13], scl);\
 op(in[i+14], scl);\
 op(in[i+15], scl);
  
#else

#define UNROLL_VECSCL(op) NO_UNROLL_VECSCL(op)

#endif


/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_VECSCL(t_in, t_ptr, op, init)\
 t_ptr scl = init;\
 NO_UNROLL_LOOP(\
  NO_UNROLL_VECSCL(op)\
 )\
 *ptr = scl;


#define FTS_VECX_VECSCL(t_in, t_ptr, op, init)\
 t_ptr scl = init;\
 UNROLL_LOOP(\
  UNROLL_VECSCL(op)\
 )\
 *ptr = scl;

#define FTL_UNWRAP_VECSCL(t_in, t_ptr)\
 UNWRAP_VEC(0, t_in, in)\
 UNWRAP_PTR(1, t_ptr, ptr)\
 UNWRAP_SIZE(2)

 

/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_VECSCL(name, t_in, t_ptr, op, init)\
 void fts_vec_ ## name(t_in *in, t_ptr *ptr, int size)\
 {FTS_VEC_VECSCL(t_in, t_ptr, op, init)}
  
#define DEFINE_FTS_VECX_VECSCL(name, t_in, t_ptr, op, init)\
 void fts_vecx_ ## name(t_in *in, t_ptr *ptr, int size)\
 {FTS_VECX_VECSCL(t_in, t_ptr, op, init)}

#define DEFINE_FTL_VEC_VECSCL(name, t_in, t_ptr, op, init)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_VECSCL(t_in, t_ptr)\
  FTS_VEC_VECSCL(t_in, t_ptr, op, init)\
 }

#define DEFINE_FTL_VECX_VECSCL(name, t_in, t_ptr, op, init)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_VECSCL(t_in, t_ptr)\
  FTS_VECX_VECSCL(t_in, t_ptr, op, init)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_VECSCL(name, t_in, t_ptr, op, init)\
 extern void fts_vec_ ## name(t_in *in, t_ptr *ptr, int size);
 
#define DECLARE_FTS_VECX_VECSCL(name, t_in, t_ptr, op, init)\
 extern void fts_vecx_ ## name(t_in *in, t_ptr *ptr, int size);
 
#define DECLARE_FTL_VEC_VECSCL(name, t_in, t_ptr, op, init)\
 extern void ftl_vec_ ## name(FTL_ARG);
 
#define DECLARE_FTL_VECX_VECSCL(name, t_in, t_ptr, op, init)\
 extern void ftl_vecx_ ## name(FTL_ARG);
 
