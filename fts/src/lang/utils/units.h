/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* units.h */

#ifndef _UNITS_H_
#define _UNITS_H_

extern fts_symbol_t fts_s_S;
extern fts_symbol_t fts_s_KS;
extern fts_symbol_t fts_s_msec;
extern fts_symbol_t fts_s_sec;


typedef float (*fts_unit_conv_t)(float, void *); /* float fun(float value, void *dependency); */

/*********************************************************
 *
 *    units API
 *
 */
 
extern int  fts_unit_add(fts_symbol_t unit_name, fts_symbol_t base, fts_unit_conv_t conv, int depends);

extern int  fts_is_unit(fts_symbol_t unit_name, fts_symbol_t base_name);
extern int  fts_unit_is_of_base(fts_symbol_t unit_name, fts_symbol_t base_name);
extern float fts_unit_convert_to_base(fts_symbol_t unit_name, float value, void *dependency);
extern int  fts_unit_depends(fts_symbol_t unit);

extern void fts_unit_to_samples(void);
extern void fts_unit_to_samples_factor(void);
extern void fts_get_unit_size_arg(void);

/*********************************************************
 *
 *    sample units (duration)
 *
 */

extern fts_symbol_t fts_unit_get_samples_arg(int ac, const fts_atom_t *at, int n, fts_symbol_t def);


#endif /* _UNITS_H_ */
