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
#ifndef _BUS_H_
#define _BUS_H_

#include "fts.h"

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
