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

#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/fpe.h"
#include "lang/utils.h"
#include "lang/datalib.h"
#include "lang/ftl.h"
#include "lang/veclib/include/veclib.h"
#include "runtime/files/post.h"


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* FTL module and module functions                                             */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void ftl_module_init( void);
static void ftl_module_shutdown( void);

fts_module_t fts_ftl_module = {"FTL", "FTL engine", ftl_module_init, ftl_module_shutdown, 0};

static void ftl_module_init(void)
{
}

static void ftl_module_shutdown(void)
{
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Actual FTL code                                                             */
/*                                                                             */
/* --------------------------------------------------------------------------- */


#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

/* FTL opcodes */
typedef enum {
  FTL_OPCODE_NOP = 123,      /* no operation */
  FTL_OPCODE_RETURN,         /* return from FTL interpreter */
  FTL_OPCODE_CALL,           /* call a wrapper function */
} ftl_opcode_t;

struct buffer_info {
  fts_symbol_t name;
  int size;
  float *buffer;
};

struct _ftl_instruction_info_t {
  fts_object_t *object;
  int ninputs;
  struct buffer_info *input_infos;
  int noutputs;
  struct buffer_info *output_infos;
};

typedef struct {
  int size;
  int last;
  ftl_instruction_info_t *info;
} ftl_info_table_t;

#define DEFAULT_INFO_SIZE 256


struct _ftl_subroutine_t {
  fts_symbol_t name;

  fts_atom_list_t *instructions;

  struct _ftl_subroutine_t *next;
  struct _ftl_subroutine_t *next_in_stack;

  fts_word_t *bytecode;

  /* Program counter for PC sampling */
  int pc;

  ftl_instruction_info_t *current_instruction_info;

  ftl_info_table_t info_table;
};


struct _ftl_program_t {
  float *buffers;		
  ftl_subroutine_t *subroutines;
  ftl_subroutine_t *current_subroutine, *main;
  fts_hash_table_t symbol_table;

  /* Stack of subroutines calls */
  ftl_subroutine_t *subroutine_tos;

  /* NaN checking */
  int check_nan;
};

static fts_hash_table_t *ftl_functions_table = 0;

fts_status_description_t ftl_error_uninitialized_program =
{
  "uninitialized FTL program"
};

fts_status_description_t ftl_error_invalid_program =
{
  "invalid FTL program"
};


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Debug info handling (temporary)                                             */
/*                                                                             */
/* --------------------------------------------------------------------------- */

void ftl_instruction_info_set_object( ftl_instruction_info_t *info, fts_object_t *object)
{
  info->object = object;
}

void ftl_instruction_info_set_ninputs( ftl_instruction_info_t *info, int ninputs)
{
  info->ninputs = ninputs;

  if ( ninputs > 0)
      info->input_infos = (struct buffer_info *)fts_malloc( sizeof( struct buffer_info) * ninputs);
}

void ftl_instruction_info_set_input( ftl_instruction_info_t *info, int n, fts_symbol_t s, int size)
{
    ASSERT ( n >= 0 && n < info->ninputs );
    ASSERT ( info->input_infos != 0);

    info->input_infos[n].name = s;
    info->input_infos[n].size = size;
    info->input_infos[n].buffer = 0;
}

void ftl_instruction_info_set_noutputs( ftl_instruction_info_t *info, int noutputs)
{
  info->noutputs = noutputs;

  if (noutputs > 0)
      info->output_infos = (struct buffer_info *)fts_malloc( sizeof( struct buffer_info) * noutputs);
}

void ftl_instruction_info_set_output( ftl_instruction_info_t *info, int n, fts_symbol_t s, int size)
{
    ASSERT ( n >= 0 && n < info->noutputs );
    ASSERT ( info->output_infos != 0);

    info->output_infos[n].name = s;
    info->output_infos[n].size = size;
    info->output_infos[n].buffer = 0;
}

static void ftl_info_table_init( ftl_info_table_t *table)
{
  int i;

  table->size = DEFAULT_INFO_SIZE;
  table->info = (ftl_instruction_info_t *)fts_malloc( table->size * sizeof(ftl_instruction_info_t));
  for ( i = 0; i < table->size; i++)
    {
      table->info[i].object = 0;
      table->info[i].ninputs = 0;
      table->info[i].input_infos = 0;
      table->info[i].noutputs = 0;
      table->info[i].output_infos = 0;
    }

  table->last = 0;
}

static ftl_instruction_info_t *ftl_info_table_add_instruction( ftl_info_table_t *table)
{
  int index;

  if (table->last >= table->size)
    {
      int old_size, i;

      old_size = table->size;

      table->size *= 2;
      table->info = (ftl_instruction_info_t *)fts_realloc( table->info, table->size * sizeof(ftl_instruction_info_t));

      for ( i = old_size; i < table->size; i++)
	{
	  table->info[i].object = 0;

	  table->info[i].ninputs = 0;
	  table->info[i].input_infos = 0;

	  table->info[i].noutputs = 0;
	  table->info[i].output_infos = 0;
	}
    }

  ASSERT( table->last >= 0 && table->last < table->size);

  index = table->last;

  table->last++;

  return &(table->info[index]);
}

static void ftl_info_table_destroy( ftl_info_table_t *table)
{
  int i;

  for ( i = 0; i < table->size; i++)
      {
	  if (table->info[i].input_infos)
	      fts_free( table->info[i].input_infos);
	  if (table->info[i].output_infos)
	      fts_free( table->info[i].output_infos);
      }
    
  fts_free( table->info);

  table->info = 0;
  table->size = 0;
  table->last = 0;
}

/* ********************************************************************** */
/* Subroutines handling                                                   */
/* ********************************************************************** */

static ftl_subroutine_t *ftl_subroutine_new( fts_symbol_t name)
{
  ftl_subroutine_t *newsubr;

  newsubr = (ftl_subroutine_t *) fts_malloc( sizeof(ftl_subroutine_t));
  if (!newsubr)
    return 0;

  newsubr->name = name;
  newsubr->instructions = fts_atom_list_new();

  newsubr->next = 0;
  newsubr->next_in_stack = 0;
  newsubr->bytecode = 0;

  newsubr->current_instruction_info = 0;

  newsubr->pc = -1;

  ftl_info_table_init( &newsubr->info_table);

  return newsubr;
}

static void ftl_subroutine_destroy( ftl_subroutine_t *subr)
{
  fts_atom_list_free(subr->instructions);

  if ( subr->bytecode)
    {
      fts_free( subr->bytecode);
      subr->bytecode = 0;
    }

  ftl_info_table_destroy( &subr->info_table);
}

static fts_status_t ftl_subroutine_add_call( ftl_subroutine_t *subr, fts_symbol_t name, int argc, const fts_atom_t *argv)
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_CALL);
  fts_atom_list_append(subr->instructions, 1, &a);

  fts_set_symbol( &a, name);
  fts_atom_list_append(subr->instructions, 1, &a);

  fts_set_long( &a, argc);
  fts_atom_list_append(subr->instructions, 1, &a);

  fts_atom_list_append(subr->instructions, argc, argv);

  /* add debugging info */
  subr->current_instruction_info = ftl_info_table_add_instruction( &subr->info_table);

  return fts_Success;
}

static fts_status_t ftl_subroutine_add_return( ftl_subroutine_t *subr)
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_RETURN);
  fts_atom_list_append(subr->instructions, 1, &a);

  /* add debugging info */
  subr->current_instruction_info = ftl_info_table_add_instruction( &subr->info_table);

  return fts_Success;
}


/* ********************************************************************** */
/* Instructions and directives insertion                                  */
/* ********************************************************************** */

ftl_subroutine_t *ftl_program_add_subroutine( ftl_program_t *prog, fts_symbol_t name)
{
  ftl_subroutine_t **subr;
  ftl_subroutine_t *newsubr;

  for( subr = &(prog->subroutines); *subr; subr = &((*subr)->next))
    {
      if ( (*subr)->name == name)
	return 0;
    }

  newsubr = ftl_subroutine_new( name);
  *subr = newsubr;

  return newsubr;
}

ftl_subroutine_t *ftl_program_set_current_subroutine( ftl_program_t *prog, ftl_subroutine_t *subr)
{
  ftl_subroutine_t *previous;

  previous = prog->current_subroutine;
  prog->current_subroutine = subr;
  return previous;
}

ftl_subroutine_t *ftl_program_add_main( ftl_program_t *prog)
{
  prog->main = ftl_program_add_subroutine( prog, fts_new_symbol("main"));

  return prog->main;
}

fts_status_t ftl_program_add_call( ftl_program_t *prog, fts_symbol_t name, int argc, const fts_atom_t *argv )
{
  if ( !prog->current_subroutine)
    return &ftl_error_uninitialized_program;

  return ftl_subroutine_add_call( prog->current_subroutine, name, argc, argv);
}


fts_status_t ftl_program_add_return( ftl_program_t *prog)
{
  return ftl_subroutine_add_return( prog->current_subroutine);
}

int ftl_program_add_signal( ftl_program_t *prog, fts_symbol_t name, int size)
{
  fts_atom_t a;

  fts_set_ptr(&a, ftl_memory_declaration_new( size));

  return fts_hash_table_insert( &(prog->symbol_table), name, &a);
}


int ftl_declare_function( fts_symbol_t name, ftl_wrapper_t wrapper)
{
  fts_atom_t a;
  ftl_function_declaration *fdecl;

  if ( !ftl_functions_table)
    ftl_functions_table = fts_hash_table_new();
  else
    {
      fts_atom_t data;
  
      if (fts_hash_table_lookup(ftl_functions_table, name,  &data))
	{
	  fdecl = (ftl_function_declaration *)fts_get_ptr(&data);
	  fts_hash_table_remove(ftl_functions_table, name);
	  ftl_function_declaration_free(fdecl);
	}
    }

  fts_set_ptr(&a, ftl_function_declaration_new( wrapper));

  return fts_hash_table_insert( ftl_functions_table, name, &a);
}

ftl_instruction_info_t *ftl_program_get_current_instruction_info( ftl_program_t *prog)
{
  return prog->current_subroutine->current_instruction_info;
}


/* ********************************************************************** */
/* Initialization and house-keeping                                       */
/* ********************************************************************** */

void ftl_program_init( ftl_program_t *prog)
{
  prog->buffers = 0;
  prog->subroutines = 0;
  prog->current_subroutine = 0;
  prog->main = 0;
  prog->subroutine_tos = 0;
  prog->check_nan = 0;

  fts_hash_table_init( &(prog->symbol_table) );
}

ftl_program_t *ftl_program_new( void )
{
  ftl_program_t *tmp;

  tmp = (ftl_program_t *) fts_malloc(sizeof( ftl_program_t));
  if ( tmp)
    ftl_program_init( tmp);
  return tmp;
}

static void free_hash_element(fts_symbol_t ignore, fts_atom_t *data, void *user_data)
{
  fts_free(fts_get_ptr(data));
}

void ftl_program_destroy( ftl_program_t *prog)
{
  ftl_subroutine_t *subr, *nextsubr;
  
  for( subr = prog->subroutines; subr; subr = nextsubr)
    {
      nextsubr = subr->next;
      ftl_subroutine_destroy( subr);
      fts_free( subr);
    }
  prog->subroutines = 0;
  prog->main = 0;
  prog->current_subroutine = 0;

  if (prog->buffers)
    {
      fts_free(prog->buffers);
      prog->buffers = 0;
    }

  fts_hash_table_apply(&(prog->symbol_table), free_hash_element, 0);
  fts_hash_table_destroy( &(prog->symbol_table) );
}


void ftl_program_free( ftl_program_t *prog)
{
  ftl_program_destroy( prog);
  fts_free( prog );
}


/* ********************************************************************** */
/* State machine for FTL programs analysis                                */
/* ********************************************************************** */

typedef fts_status_t (*state_fun_t)( int state, int newstate, fts_atom_t *a, void *user_data);

typedef enum { 
  ST_OPCODE, 
  ST_CALL_FUN, ST_CALL_ARGC, ST_CALL_ARGV
} state_t;

static fts_status_t ftl_state_machine( fts_atom_list_t *alist, state_fun_t fun, void *user_data)
{
  fts_atom_t *a = 0;	    
  fts_atom_list_iterator_t *iter;
  int state, newstate, argc;
  fts_status_t ret;

  state = ST_OPCODE;
  argc  = 0;

  iter = fts_atom_list_iterator_new(alist);

  for( ;
      !fts_atom_list_iterator_end( iter);
      fts_atom_list_iterator_next( iter))
    {
      a = fts_atom_list_iterator_current( iter);

      switch( state) {
      case ST_OPCODE:
	if ( fts_is_long( a) )
	  {
	    switch( fts_get_long( a)) {
	    case FTL_OPCODE_CALL:
	      newstate = ST_CALL_FUN;
	      break;
	    case FTL_OPCODE_RETURN:
	    case FTL_OPCODE_NOP:
	      newstate = ST_OPCODE;
	      break;
	    default:
	      fts_atom_list_iterator_free(iter);
	      return &ftl_error_invalid_program;
	    }
	  }
	else
	  {
	    fts_atom_list_iterator_free(iter);
	    return &ftl_error_invalid_program;
	  }
	break;
      case ST_CALL_FUN:
	if ( fts_is_symbol( a))
	  newstate = ST_CALL_ARGC;
	else
	  {
	    fts_atom_list_iterator_free(iter);
	    return &ftl_error_invalid_program;
	  }
	break;
      case ST_CALL_ARGC:
	if ( fts_is_long( a) )
	  {
	    argc = fts_get_long(a);
	    if ( argc > 0)
	      newstate = ST_CALL_ARGV;
	    else
	      newstate = ST_OPCODE;
	  }
	else
	  {
	    fts_atom_list_iterator_free(iter);
	    return &ftl_error_invalid_program;
	  }
	break;
      case ST_CALL_ARGV:
	argc--;
	if ( argc <= 0)
	  newstate = ST_OPCODE;
	else
	  newstate = ST_CALL_ARGV;
	break;
      }



      ret = (*fun)(state, newstate, a, user_data);
      if (ret != fts_Success)
	{
	  fts_atom_list_iterator_free(iter);
	  return ret;
	}
      state = newstate;
    }

  fts_atom_list_iterator_free(iter);
  return fts_Success;
}



/* ********************************************************************** */
/* Compilation                                                            */
/* ********************************************************************** */

static void ftl_program_update_instruction_infos( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      int i;

      for ( i = 0; i < subr->info_table.size; i++)
	{
	  ftl_instruction_info_t *info;
	  int n;

	  info = &(subr->info_table.info[i]);

	  for ( n = 0; n < info->ninputs; n++)
	    {
	      fts_atom_t data;
	      ftl_memory_declaration *mdecl;
	      fts_symbol_t s;
	
	      s = info->input_infos[n].name;
	      fts_hash_table_lookup( &(prog->symbol_table), s, &data);
	      mdecl = (ftl_memory_declaration *)fts_get_ptr(&data);
	      info->input_infos[n].buffer = mdecl->address;
	    }

	  for ( n = 0; n < info->noutputs; n++)
	    {
	      fts_atom_t data;
	      ftl_memory_declaration *mdecl;
	      fts_symbol_t s;
	
	      s = info->output_infos[n].name;
	      fts_hash_table_lookup( &(prog->symbol_table), s, &data);
	      mdecl = (ftl_memory_declaration *)fts_get_ptr(&data);
	      info->output_infos[n].buffer = mdecl->address;
	    }
		      
	}
    }
}


static int ftl_program_allocate_signals( ftl_program_t *prog)
{
  fts_hash_table_iterator_t iter;
  unsigned long total_size;
  float *p;

  total_size = 0;

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;

      m = (ftl_memory_declaration *)fts_get_ptr(fts_hash_table_iterator_current_data( &iter));

      total_size = total_size + m->size;
    }

  if ( total_size == 0)
    return 1;

  p = (float *) fts_malloc(total_size * sizeof(float));

  if (! p)
    {
      post("ftl_program_allocate_signals (size = %d): not enough memory\n", total_size);
      return 0;
    }

  prog->buffers = p;

  fts_vecx_fzero(p, total_size);

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;

      m = (ftl_memory_declaration *)fts_get_ptr(fts_hash_table_iterator_current_data( &iter));

      m->address = p;

      p = p + m->size;
    }

  ftl_program_update_instruction_infos( prog);

  return 1;
}


struct compile_info_portable {
  ftl_program_t *prog;
  fts_word_t *bytecode;
};

static fts_status_t compile_portable_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  struct compile_info_portable *info = (struct compile_info_portable *)user_data;
  fts_word_t *bytecode;

  bytecode = info->bytecode;
  switch( state) {
  case ST_OPCODE:
    if ( fts_get_long( a) == FTL_OPCODE_RETURN)
      {
	fts_word_set_long( bytecode, 0);
	bytecode++;
      }
    break;
  case ST_CALL_FUN:
    {
      fts_atom_t data;
      ftl_function_declaration *fdecl;

      fts_hash_table_lookup( ftl_functions_table, fts_get_symbol(a), &data);
      fdecl =  (ftl_function_declaration *) fts_get_ptr(&data);
      fts_word_set_fun( bytecode, (void (*)(void)) fdecl->wrapper);
      bytecode++;
    }
    break;
  case ST_CALL_ARGC:
    fts_word_set_long( bytecode, fts_get_long(a));
    bytecode++;
    break;
  case ST_CALL_ARGV:
    if ( fts_is_symbol(a))
      {
	fts_atom_t data;
	ftl_memory_declaration *mdecl;
	
	fts_hash_table_lookup( &(info->prog->symbol_table), fts_get_symbol(a), &data);
	mdecl = (ftl_memory_declaration *)fts_get_ptr(&data);
	fts_word_set_ptr( bytecode, mdecl->address);
	bytecode++;
      }
    else
      *bytecode++ = a->value;
    break;
  }
  info->bytecode = bytecode;

  return fts_Success;
}

static fts_status_t bytecode_size_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  int *ps = (int *)user_data;

  switch( state) {
  case ST_OPCODE:
    if (fts_get_long( a) == FTL_OPCODE_RETURN)
      *ps += 1;
    return fts_Success;
  case ST_CALL_ARGC:
    *ps += (fts_get_long( a) + 2);
    return fts_Success;
  default:
    return fts_Success;
  }
}

static int ftl_program_compile_portable( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;
  fts_status_t ret;
  struct compile_info_portable info;
  int pc;

  if ( !ftl_program_allocate_signals( prog))
    return 0;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      int size;

      size = 0;
      ret = ftl_state_machine( subr->instructions, bytecode_size_state_fun, &size);
      if ( ret != fts_Success)
	return 0;

      subr->bytecode = (fts_word_t *)fts_malloc( size*sizeof( fts_word_t));
      if ( !subr->bytecode)
	{
	  post( "ftl_program_generate_bytecode : not enough memory\n");
	  return 0;
	}

      info.prog = prog;
      info.bytecode = subr->bytecode;
      ret = ftl_state_machine( subr->instructions, compile_portable_state_fun, &info);
      if ( ret != fts_Success)
	return 0;

    }

  return 1;
}

int ftl_program_compile( ftl_program_t *prog)
{
  return ftl_program_compile_portable(prog);
}


/* ********************************************************************** */
/* Print and post functions                                               */
/* ********************************************************************** */

#ifdef DEBUG
static void ftl_post_functions_table( void)
{
  fts_hash_table_iterator_t it;
  ftl_function_declaration *decl;
  fts_symbol_t s;

  for( fts_hash_table_iterator_init( &it, ftl_functions_table);
      ! fts_hash_table_iterator_end( &it);
      fts_hash_table_iterator_next( &it) )
    {
      decl = (ftl_function_declaration *)fts_get_ptr(fts_hash_table_iterator_current_data(&it));
      s = fts_hash_table_iterator_current_symbol( &it);
      post( "/* Function %s (0x%x) */\n", fts_symbol_name(s), decl->wrapper);
    }
}
#endif

static void ftl_print_atom( char *s, const fts_atom_t *a)
{
  if (fts_is_long(a))
    sprintf( s, "%d", fts_get_int(a));
  else if (fts_is_float(a))
    sprintf( s, "%f", fts_get_float(a));
  else if (fts_is_symbol(a))
    sprintf( s, "%s", fts_symbol_name(fts_get_symbol(a)));
  else if (fts_is_ptr(a))
    sprintf( s, "0x%x", (unsigned int ) fts_get_ptr(a));
  else
    sprintf( s, "?");
}

void ftl_program_post_signals( const ftl_program_t *prog)
{
  fts_hash_table_iterator_t iter;

  post( "/* %d signals declarations */\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;
      fts_symbol_t s;

      m = (ftl_memory_declaration *)fts_get_ptr(fts_hash_table_iterator_current_data( &iter));
      s = fts_hash_table_iterator_current_symbol( &iter);
      post( "float %s[%d];  /* adress 0x%x */\n", fts_symbol_name(s), m->size, m->address);
    }
  post( "\n");
}

void ftl_program_fprint_signals( FILE *f, const ftl_program_t *prog)
{
  fts_hash_table_iterator_t iter;

  fprintf( f,  "/* %d signals declarations */\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;
      fts_symbol_t s;

      m = (ftl_memory_declaration *)fts_get_ptr(fts_hash_table_iterator_current_data( &iter));
      s = fts_hash_table_iterator_current_symbol( &iter);
      fprintf(f, "float %s[%d];  /* adress 0x%x */\n", fts_symbol_name(s),
	      m->size, (unsigned int) m->address);
    }
  fprintf(f, "\n");
}

void ftl_program_post_signals_count( const ftl_program_t *prog)
{
  post( "ftl_program : signals %d\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));
}


void ftl_program_fprint_signals_count( FILE *f, const ftl_program_t *prog)
{
  fprintf( f,  "ftl_program : signals %d\n", 
	   fts_hash_table_get_count(&(prog->symbol_table)));
}

struct post_info {
  int pc;
  char line[256];
  ftl_instruction_info_t *instr_info;
};

static fts_status_t post_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  struct post_info *info = (struct post_info *)user_data;
  char buffer[64];
  static int pc = 0;

  buffer[0] = ' ';
  switch( state) {
  case ST_OPCODE:
    switch( fts_get_long( a)) {
    case FTL_OPCODE_RETURN:
      post( "/* %5d */   return;\n", info->pc);
      break;
    case FTL_OPCODE_CALL:
      pc = info->pc;
      info->pc++;
      break;
    }
    break;
  case ST_CALL_FUN:
    sprintf( info->line, "/* %5d */   %s( ", pc, fts_symbol_name(fts_get_symbol(a)));
    break;
  case ST_CALL_ARGV:
    if (fts_is_ptr(a))
      sprintf( buffer, "(void *)0x%x", (unsigned int) fts_get_ptr(a));
    else
      ftl_print_atom( buffer, a);
    strcat( info->line, buffer);
    if ( newstate == ST_OPCODE)
      {
	ftl_instruction_info_t *instr_info;
	fts_object_t *object;

	strcat( info->line, " );");
	instr_info = info->instr_info;
	object = instr_info->object;
	if (object)
	  post( "%s /* object %s", info->line, fts_symbol_name( fts_object_get_class_name(object)));
	else
	  post( "%s /* object unknown", info->line);

	{
	  int i;

	  post( ", %d inputs {", instr_info->ninputs);
	  for ( i = 0; i < instr_info->ninputs; i++)
	    post( " %s[%d]", fts_symbol_name( instr_info->input_infos[i].name), instr_info->input_infos[i].size);
	  post( "} ");
	}

	{
	  int i;

	  post( ", %d outputs {", instr_info->noutputs);
	  for ( i = 0; i < instr_info->noutputs; i++)
	    post( " %s[%d]", fts_symbol_name( instr_info->output_infos[i].name), instr_info->output_infos[i].size);
	  post( "}");
	}

	post( "*/ \n");
	  
	info->instr_info++;
      }
    else
      strcat( info->line, ", ");
    break;
  }

  return fts_Success;
}


struct print_info {
  FILE *f;
  int pc;
  char line[256];
  ftl_instruction_info_t *instr_info;
};

static fts_status_t fprint_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  struct print_info *info = (struct print_info *)user_data;
  char buffer[64];
  static int pc = 0;

  buffer[0] = ' ';
  switch( state) {
  case ST_OPCODE:
    switch( fts_get_long( a)) {
    case FTL_OPCODE_RETURN:
      fprintf(info->f, "/* %5d */   return;\n", info->pc);
      break;
    case FTL_OPCODE_CALL:
      pc = info->pc;
      info->pc++;
      break;
    }
    break;
  case ST_CALL_FUN:
    sprintf( info->line, "/* %5d */   %s( ", pc, fts_symbol_name(fts_get_symbol(a)));
    break;
  case ST_CALL_ARGV:
    if (fts_is_ptr(a))
      sprintf( buffer, "(void *)0x%x", (unsigned int) fts_get_ptr(a));
    else
      ftl_print_atom( buffer, a);
    strcat( info->line, buffer);
    if ( newstate == ST_OPCODE)
      {
	fts_object_t *object;

	strcat( info->line, " );");
	object = info->instr_info->object;
	if (object)
	  fprintf(info->f, "%s /* object %s */\n", info->line, fts_symbol_name( fts_object_get_class_name(object)));
	else
	  fprintf(info->f, "%s /* object unknown */\n", info->line);
	  
	info->instr_info++;
      }
    else
      strcat( info->line, ", ");
    break;
  }

  return fts_Success;
}


void ftl_program_post( const ftl_program_t *prog )
{
  ftl_subroutine_t *subr;
  struct post_info info;

  ftl_program_post_signals(prog);

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      post( "%s()\n", fts_symbol_name( subr->name));
      post( "{\n");
      info.line[0] = 0;
      info.pc = 0;
      info.instr_info = subr->info_table.info;
      ftl_state_machine( subr->instructions, post_state_fun, &info);
      post( "}\n\n");
    }
}


void ftl_program_fprint( FILE *f, const ftl_program_t *prog )
{
  ftl_subroutine_t *subr;
  struct print_info info;

  ftl_program_fprint_signals(f, prog);

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      fprintf( f, "%s()\n", fts_symbol_name( subr->name));
      fprintf( f, "{\n");
      info.f = f;
      info.line[0] = 0;
      info.pc = 0;
      info.instr_info = subr->info_table.info;
      ftl_state_machine( subr->instructions, fprint_state_fun, &info);
      fprintf( f, "}\n\n");
    }
}


void ftl_program_post_bytecode( const ftl_program_t *prog)
{
#if 0
  ftl_subroutine_t *subr;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      fts_word_t *bytecode = subr->bytecode;
      int pc = 0;

      post( "subroutine %s bytecode 0x%x{\n", fts_symbol_name( subr->name), bytecode);
      if (bytecode)
	for( ; fts_word_get_long(bytecode); bytecode++)
	  {
	    post( "[%d] 0x%x %d\n", pc, fts_word_get_long( bytecode), fts_word_get_long( bytecode));
	    pc++;
	  }
      post( "}\n");
    }
#endif
}


/* ********************************************************************** */
/* Run functions                                                          */
/* ********************************************************************** */

static int ftl_check_nan_buffers( int n, struct buffer_info *infos)
{
  int i, j;
  float *vector;

  for ( i = 0; i < n; i++)
    {
      vector = infos[i].buffer;

      for ( j = 0; j < infos[i].size; j++)
	if (fts_isnanf( vector[j]))
	  {
	    post_vector(infos[i].size, vector);
	    
	    return 1;
	  }
    }

  return 0;
}

void ftl_program_call_subr( ftl_program_t *prog, ftl_subroutine_t *subr)
{
  fts_word_t *bytecode;

  subr->next_in_stack = prog->subroutine_tos;
  prog->subroutine_tos = subr;

  bytecode = subr->bytecode;
  if ( prog->check_nan)
    {
      int nan_detected = 0;

      subr->pc = 0;

      while (fts_word_get_long(bytecode))
	{
	  ftl_wrapper_t w;
	  int argc;
	  ftl_instruction_info_t *instr_info;
      
	  w = (ftl_wrapper_t) fts_word_get_fun( bytecode);
	  argc = fts_word_get_long( bytecode+1);
	  (*w)(bytecode+2);

	  if ( !nan_detected)
	    {
	      instr_info = &(subr->info_table.info[ subr->pc]);
	      if (ftl_check_nan_buffers( instr_info->noutputs, instr_info->output_infos))
		{
		  fts_fpe_add_object( instr_info->object);
		  nan_detected = 1;
		}
	    }

	  bytecode += (argc + 2);
	  subr->pc++;
	}
    }
  else
    {
      subr->pc = 0;

      while (fts_word_get_long(bytecode))
	{
	  ftl_wrapper_t w;
	  int argc;
      
	  w = (ftl_wrapper_t) fts_word_get_fun( bytecode);
	  argc = fts_word_get_long( bytecode+1);
	  (*w)(bytecode+2);
	  bytecode += (argc + 2);
	  subr->pc++;
	}
    }

  subr->pc = -1;
  prog->subroutine_tos = subr->next_in_stack;
  subr->next_in_stack = 0;
}

void ftl_program_run( ftl_program_t *prog )
{
  ftl_program_call_subr( prog, prog->main);
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Auxiliary function for profiling                                            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

fts_object_t *ftl_program_get_current_object( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;
  int pc;

  subr = prog->subroutine_tos;
  if (subr && subr->pc >= 0)
    return subr->info_table.info[subr->pc].object;
  else
    return 0;
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Debugging utilities                                                         */
/*                                                                             */
/* --------------------------------------------------------------------------- */

void ftl_program_set_check_nan( ftl_program_t *prog, int check_nan)
{
  prog->check_nan = check_nan;
}
