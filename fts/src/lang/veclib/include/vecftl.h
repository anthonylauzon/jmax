/* vecftl.h */

#ifndef _VECFTL_H_
#define _VECFTL_H_

#include "lang/mess.h"

#include "lang/veclib/include/declare_ftl.h"
#include "lang/veclib/include/vec_fun.h"
#include "lang/veclib/include/vec_arith.h"
#include "lang/veclib/include/vec_cmp.h"
#include "lang/veclib/include/vec_cpy.h"
#include "lang/veclib/include/vec_bit.h"
#include "lang/veclib/include/vec_misc.h"

/* hand coded */
extern void ftl_vec_csplit(fts_word_t *a);
extern void ftl_vecx_csplit(fts_word_t *a);
extern void ftl_vec_cmerge(fts_word_t *a);
extern void ftl_vecx_cmerge(fts_word_t *a);

#endif /* _VECFTL_H_ */
