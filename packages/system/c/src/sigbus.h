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
 */

#ifndef _FTS_SIGBUS_H_
#define _FTS_SIGBUS_H_

#include <fts/fts.h>

#define FTS_SIGNAL_BUS_MAX_CHANNELS 64

typedef struct _fts_signal_bus_
{
  fts_object_t o;
  int n_channels;
  ftl_data_t buf[2]; 
  ftl_data_t toggle; /* toggle (0/1) indicating current write buffer (swapping buffers each tick) */
  int n_tick;
} fts_signal_bus_t;

extern fts_symbol_t fts_signal_bus_symbol;
extern fts_metaclass_t *fts_signal_bus_type;

#define fts_signal_bus_get_size(b) ((b)->n_channels)

#define fts_signal_bus_get_buffer_data(b, i) ((b)->buf[i])
#define fts_signal_bus_get_toggle_data(b) ((b)->toggle)

#define fts_signal_bus_get_buffer(b, i) ((float *)ftl_data_get_ptr((b)->buf[i]))

#endif
