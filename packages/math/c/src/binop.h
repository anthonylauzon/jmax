#ifndef _BINOP_H_
#define _BINOP_H_

#include <fts/fts.h>
#include "ivec.h"
#include "fvec.h"

extern fts_symbol_t math_sym_add;
extern fts_symbol_t math_sym_sub;
extern fts_symbol_t math_sym_mul;
extern fts_symbol_t math_sym_div;
extern fts_symbol_t math_sym_bus;
extern fts_symbol_t math_sym_vid;

extern fts_symbol_t math_sym_ee;
extern fts_symbol_t math_sym_ne;
extern fts_symbol_t math_sym_gt;
extern fts_symbol_t math_sym_ge;
extern fts_symbol_t math_sym_lt;
extern fts_symbol_t math_sym_le;

extern fts_symbol_t math_sym_min;
extern fts_symbol_t math_sym_max;

#endif
