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

#ifndef _FTS_FTL_H_
#define _FTS_FTL_H_

extern fts_module_t fts_ftl_module;

typedef void (*ftl_wrapper_t)(fts_word_t *);

typedef struct _ftl_instruction_info_t ftl_instruction_info_t;

typedef struct _ftl_program_t ftl_program_t;

typedef struct _ftl_subroutine_t ftl_subroutine_t;


extern void ftl_program_init( ftl_program_t *prog);

extern ftl_program_t *ftl_program_new( void );

extern void ftl_program_destroy( ftl_program_t *prog);

extern void ftl_program_free( ftl_program_t *prog );

/* Instruction info */
extern void ftl_instruction_info_set_object( ftl_instruction_info_t *info, fts_object_t *object);

extern void ftl_instruction_info_set_ninputs( ftl_instruction_info_t *info, int ninputs);

extern void ftl_instruction_info_set_input( ftl_instruction_info_t *info, int n, fts_symbol_t s, int size);

extern void ftl_instruction_info_set_noutputs( ftl_instruction_info_t *info, int noutputs);

extern void ftl_instruction_info_set_output( ftl_instruction_info_t *info, int n, fts_symbol_t s, int size);


/* Subroutines */
extern ftl_subroutine_t *ftl_program_add_subroutine( ftl_program_t *prog, fts_symbol_t name);

extern ftl_subroutine_t *ftl_program_add_main( ftl_program_t *prog);

extern ftl_subroutine_t *ftl_program_set_current_subroutine( ftl_program_t *prog, ftl_subroutine_t *subr);


/* Program */
extern fts_status_t ftl_program_add_call( ftl_program_t *prog, fts_symbol_t name, int argc, const fts_atom_t *argv);

extern fts_status_t ftl_program_add_return( ftl_program_t *prog);

extern int ftl_program_add_signal( ftl_program_t *prog, fts_symbol_t name, int vector_size);

extern int ftl_declare_function( fts_symbol_t name, ftl_wrapper_t wrapper);

extern ftl_instruction_info_t *ftl_program_get_current_instruction_info( ftl_program_t *prog);


extern void ftl_program_post( const ftl_program_t *prog );
extern void ftl_program_post_signals( const ftl_program_t *prog );
extern void ftl_program_post_signals_count( const ftl_program_t *prog);
extern void ftl_program_post_bytecode( const ftl_program_t *prog);

extern void ftl_program_fprint( FILE *f, const ftl_program_t *prog );
extern void ftl_program_fprint_signals( FILE *f, const ftl_program_t *prog );
extern void ftl_program_fprint_signals_count( FILE *f, const ftl_program_t *prog);


extern int ftl_program_compile( ftl_program_t *prog);

extern void ftl_program_call_subr( ftl_program_t *prog, ftl_subroutine_t *subr);
extern void ftl_program_run( ftl_program_t *prog );

extern fts_object_t *ftl_program_get_current_object( ftl_program_t *prog);

/* Debugging utilities */
extern void ftl_program_set_check_nan( ftl_program_t *prog, int check);

#endif
