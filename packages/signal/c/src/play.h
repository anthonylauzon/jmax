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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

/************************************************************
 *
 *  generic play~ class
 *
 */

typedef struct _signal_play_
{
  fts_object_t o;

  fts_object_t *object;

  double position; /* current position */
  double begin; /* begin position */
  double end; /* end position */
  double step; /* playing step */
  double conv_position; /* position conversion factor */
  double conv_step; /* speed to step conversion factor */

  enum play_mode {mode_stop, mode_pause, mode_play, mode_loop, mode_cycle} mode;
  int cycle_direction;

  fts_timer_t *timer;
} signal_play_t;

#define signal_play_get_conv_position(p) ((p)->conv_position)
#define signal_play_get_conv_step(p) ((p)->conv_step)

extern void signal_play_init(signal_play_t *this, int ac, const fts_atom_t *at);
extern void signal_play_delete(signal_play_t *this);
extern void signal_play_class_register(fts_symbol_t name, fts_instantiate_fun_t fun);
extern void signal_play_class_init(fts_class_t *cl, fts_symbol_t type);

extern void signal_play_set_conv_position(signal_play_t *this, double c);
extern void signal_play_set_conv_step(signal_play_t *this, double c);
