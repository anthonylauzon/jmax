/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
