#ifndef _BINOP_H_
#define _BINOP_H_

#include "fts.h"
#include "intvec.h"
#include "floatvec.h"

typedef struct
{
  fts_object_t o;
  fts_symbol_t right;
} binop_symbol_t;

typedef struct
{
  fts_object_t o;
  int_vector_t *right;
} binop_int_vector_t;

typedef struct
{
  fts_object_t o;
  float_vector_t *right;
} binop_float_vector_t;

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
