/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
#ifndef _FTS_OUTLET_H_
#define _FTS_OUTLET_H_

/**
 * Output bang (void) from outlet.
 *
 * @fn void fts_outlet_bang(fts_object_t *o, int woutlet)
 * @param o the object
 * @param woutlet outlet index
 */
FTS_API void fts_outlet_bang(fts_object_t *o, int woutlet);

/**
 * Output integer value from outlet.
 *
 * @fn void fts_outlet_int(fts_object_t *o, int woutlet, int n)
 * @param o the object
 * @param woutlet outlet index
 * @param n the value
 */
FTS_API void fts_outlet_int(fts_object_t *o, int woutlet, int n);

/**
 * Output (double) float value from outlet.
 *
 * @fn void fts_outlet_float(fts_object_t *o, int woutlet, double f)
 * @param o the object
 * @param woutlet outlet index
 * @param f the value
 */
FTS_API void fts_outlet_float(fts_object_t *o, int woutlet, double f);

/**
 * Output symbol from outlet.
 *
 * @fn void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
 * @param o the object
 * @param woutlet outlet index
 * @param s the symbol
 */
FTS_API void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s);

/**
* Output object value from outlet.
 *
 * @fn void fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
 * @param o the object
 * @param woutlet outlet index
 * @param obj the object value
 */
FTS_API void fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj);

/**
 * Output any atom from outlet.
 *
 * @fn void fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* at)
 * @param o the object
 * @param woutlet outlet index
 * @param at the atom
 */
FTS_API void fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* at);

/**
 * Output array of values from outlet.
 *
 * @fn void fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
 * @param o the object
 * @param woutlet outlet index
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at);

/**
 * Output message (with varargs) from outlet.
 *
 * @fn void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the object
 * @param woutlet outlet index
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Output message (with varargs) or any values from outlet.
 *
 * @fn void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the object
 * @param woutlet outlet index
 * @param s the message symbol or NULL to output values
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at);

#endif
