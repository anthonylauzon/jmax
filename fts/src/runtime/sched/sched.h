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

#ifndef _SCHED_H_
#define _SCHED_H_

/*Scheduler  Pubblic functions */

extern fts_module_t fts_sched_module;

/* run time */

extern void fts_sched_run(void);
extern void fts_restart(void);
extern void fts_halt(void);

/* declarations Scheduler */

enum declaration_verb {provide, require, freerun};
extern void fts_sched_declare(void (* fun)(void), enum declaration_verb decl, fts_symbol_t state, const char *name);

extern void fts_sched_compile(void);

/* Debug functions */

extern void fts_sched_describe_order(void); 
extern void fts_sched_describe_declarations(void);

/* Tick length handling */

extern float fts_sched_get_tick_length(void);


extern void fts_sched_set_pause(int p);

#endif
