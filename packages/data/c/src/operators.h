/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include "fts.h"
#include "types.h"

/************************************************
 *
 *  operand types
 *
 */

typedef struct _op_type
{
  fts_symbol_t name;
  int id;
} op_type_t;

#define op_type_get_name(ot) ((ot).name)
#define op_type_get_id(ot) ((ot).id)

#define op_type_is_primitive(ot) (data_type_id_is_primitive((ot).id))

#define op_type_set(otp, n) (((otp)->name != n)? ((otp)->id = data_type_get_id_by_name(n), (otp)->name = (n)): (n))

#define op_type_set_int(otp) ((otp)->id = data_type_id_int, (otp)->name = fts_s_int)
#define op_type_set_float(otp) ((otp)->id = data_type_id_float, (otp)->name = fts_s_float)
#define op_type_set_symbol(otp) ((otp)->id = data_type_id_symbol, (otp)->name = fts_s_symbol)
#define op_type_set_integer_vector(otp) ((otp)->id = data_type_id_integer_vector, (otp)->name = fts_s_integer_vector)
#define op_type_set_float_vector(otp) ((otp)->id = data_type_id_float_vector, (otp)->name = fts_s_float_vector)

/************************************************
 *
 *  operands
 *
 */

typedef struct _op_
{
  op_type_t type;
  fts_atom_t atom;
} op_t;

#define op_get_ap(op) (&((op).atom))
#define opp_get_ap(opp) (&((opp)->atom))

#define op_is_int(op) (op_get_type_id(op) == data_type_id_int)
#define op_is_float(op) (op_get_type_id(op) == data_type_id_float)
#define op_is_symbol(op) (op_get_type_id(op) == data_type_id_symbol)
#define op_is_integer_vector(op) (op_get_type_id(op) == data_type_id_integer_vector)
#define op_is_float_vector(op) (op_get_type_id(op) == data_type_id_float_vector)

#define op_get_atom(op) ((op).atom)
#define op_get_type_id(op) (op_type_get_id((op).type))
#define op_get_type_name(op) (op_type_get_name((op).type))
#define op_get_int(op) (fts_get_int(op_get_ap(op)))
#define op_get_float(op) (fts_get_float(op_get_ap(op)))
#define op_get_symbol(op) (fts_get_symbol(op_get_ap(op)))
#define op_get_integer_vector(op) ((fts_integer_vector_t *)fts_get_data(op_get_ap(op)))
#define op_get_float_vector(op) ((fts_float_vector_t *)fts_get_data(op_get_ap(op)))

#define op_get_number(op) (fts_get_number(op_get_ap(op)))

#define _op_get_data(op) (fts_get_data(op_get_ap(op)))

#define op_ref_refer(opp) fts_data_refer(fts_get_data(&(opp)->atom))
#define op_ref_release(opp) fts_data_derefer(_op_get_data(*opp))
#define op_ref_can_be_reused(op) (fts_data_has_just_one_reference(_op_get_data(op)))

#define op_refer(opp) {if(!op_type_is_primitive((opp)->type)) op_ref_refer(opp);}
#define op_release(opp) {if(!op_type_is_primitive((opp)->type)) op_ref_release(opp);}
#define op_can_be_reused(op) \
  ((op_type_is_primitive((op).type))? 1: fts_data_has_just_one_reference(_op_get_data(op)))

#define op_init(opp, a) \
do{ \
  op_type_set(&((opp)->type), data_atom_get_type(a)); \
  (opp)->atom = a; \
  op_refer(opp); \
}while(0)

#define op_init_void(opp) \
do{ \
  fts_atom_t a; \
  fts_set_void(&a); \
  op_type_set(&((opp)->type), fts_s_void); \
  (opp)->atom = a; \
}while(0)

#define op_set_with_type(opp, a, t) \
do{ \
  op_release(opp); \
  if(op_get_type_name(*opp) != (t)) \
    op_type_set(&((opp)->type), (t)); \
  (opp)->atom = a; \
  op_refer(opp); \
}while(0);

#define _op_set_data(opp, d) {fts_set_data(opp_get_ap(opp), (d)); fts_data_refer(d);}

#define op_set_int(opp, i) {op_type_set_int(&(opp)->type); fts_set_int(opp_get_ap(opp), (i));}
#define op_set_float(opp, f) {op_release(opp); op_type_set_float(&(opp)->type); fts_set_float(opp_get_ap(opp), (f));}
#define op_set_symbol(opp, s) {op_release(opp); op_type_set_symbol(&(opp)->type); fts_set_symbol(opp_get_ap(opp), (s));}
#define op_set_integer_vector(opp, iv) \
  {op_release(opp); op_type_set_integer_vector(&(opp)->type); _op_set_data((opp), (fts_data_t *)(iv));}
#define op_set_float_vector(opp, fv) \
  {op_release(opp); op_type_set_float_vector(&(opp)->type); _op_set_data((opp), (fts_data_t *)(fv));}

#define op_void(opp) {op_release(opp); fts_set_void(opp_get_ap(opp));}
#define op_is_void(op) fts_is_void(op_get_ap(op))

#define op_outlet(o, i, op) fts_outlet_send(o, 0, op_get_type_name(op), 1, op_get_ap(op))

extern fts_integer_vector_t *op_recycle_to_integer_vector(op_t *opp, int size);
extern fts_float_vector_t *op_recycle_to_float_vector(op_t *opp, int size);
extern fts_integer_vector_t *op_recycle_to_integer_vector_replicate(op_t *opp, fts_integer_vector_t *original);
extern fts_float_vector_t *op_recycle_to_float_vector_replicate(op_t *opp, fts_float_vector_t *original);

/************************************************
 *
 *  operator functions
 *
 */

typedef op_t * (* operator_fun_t) (op_t *);
typedef operator_fun_t (*operator_matrix_t)[MAX_N_DATA_TYPES];
typedef operator_fun_t *operator_vector_t;

#endif
