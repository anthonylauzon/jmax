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

#ifndef _BUS_H_
#define _BUS_H_

#include <fts/fts.h>

extern fts_symbol_t fts_s_bus;

typedef struct fts_bus_input
{
  fts_object_t *src;
  struct fts_bus_input *next;
} fts_bus_input_t;

typedef struct fts_bus_output
{
  fts_object_t *dst;
  int outlet;
  struct fts_bus_output *next;
} fts_bus_output_t;

typedef struct fts_bus
{
  int  nch;			/* number of channels */
  fts_bus_input_t  **inputs;	/* a list for each channel */
  fts_bus_output_t **outputs;	/* a list for each channel */
} fts_bus_t;

extern void fts_bus_add_input(fts_bus_t *bus, int ch, fts_object_t *src);
extern void fts_bus_remove_input(fts_bus_t *bus, int ch, fts_object_t *src);
extern void fts_bus_add_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet);
extern void fts_bus_remove_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet);
extern void fts_bus_send_message(fts_bus_t *bus, int ch, fts_symbol_t selector, int ac, const fts_atom_t *at);

extern int fts_bus_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);

#endif
