/* define/set.h */

#include "lang/veclib/include/unroll.h"
#include "lang/veclib/include/unwrap.h"



/***************************************************
 *
 *    no input, outputs vector
 *
 *      -> VECTOR
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */

#define NO_UNROLL_SET(fun) fun(out[i+0]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);\
  fun(out[i+4]);\
  fun(out[i+5]);\
  fun(out[i+6]);\
  fun(out[i+7]);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);\
  fun(out[i+4]);\
  fun(out[i+5]);\
  fun(out[i+6]);\
  fun(out[i+7]);\
  fun(out[i+8]);\
  fun(out[i+9]);\
  fun(out[i+10]);\
  fun(out[i+11]);\
  fun(out[i+12]);\
  fun(out[i+13]);\
  fun(out[i+14]);\
  fun(out[i+15]);
 
#else

#define UNROLL_SET(fun) NO_UNROLL_SET(fun)

#endif


/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_SET(t_out, fun)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_SET(fun)\
 )

#define FTS_VECX_SET(t_out, fun)\
 UNROLL_LOOP(\
  UNROLL_SET(fun)\
 )

#define FTL_UNWRAP_SET(t_out)\
 UNWRAP_VEC(0, t_out, out)\
 UNWRAP_SIZE(1)


/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_SET(name, t_out, fun)\
 void fts_vec_ ## name(t_out *out, long size)\
 {FTS_VEC_SET(t_out, fun)}
  
#define DEFINE_FTS_VECX_SET(name, t_out, fun)\
 void fts_vecx_ ## name(t_out *out, long size)\
 {FTS_VECX_SET(t_out, fun)}

#define DEFINE_FTL_VEC_SET(name, t_out, fun)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_SET(t_out)\
  FTS_VEC_SET(t_out, fun)\
 }

#define DEFINE_FTL_VECX_SET(name, t_out, fun)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_SET(t_out)\
  FTS_VECX_SET(t_out, fun)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_SET(name, t_out, fun)\
 extern void fts_vec_ ## name(t_out *out, long size);
 
#define DECLARE_FTS_VECX_SET(name, t_out, fun)\
 extern void fts_vecx_ ## name(t_out *out, long size);
 
#define DECLARE_FTL_VEC_SET(name, t_out, fun)\
 extern void ftl_vec_ ## name(FTL_ARG);

#define DECLARE_FTL_VECX_SET(name, t_out, fun)\
 extern void ftl_vecx_ ## name(FTL_ARG);

