#ifdef PTHREADS
#include <pthread.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/ftl.h"
#include "lang/veclib/include/veclib.h"
#include "runtime/files/post.h"

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

/* FTL opcodes */
typedef enum {
  FTL_OPCODE_NOP = 123,      /* no operation */
  FTL_OPCODE_RETURN,         /* return from FTL interpreter */
  FTL_OPCODE_CALL,           /* call a wrapper function */
} ftl_opcode_t;


typedef struct {
  fts_object_t *object;
  int profile_count;
} ftl_instruction_debug_info_t;

typedef struct {
  int size;
  int last;
  ftl_instruction_debug_info_t *info;
} ftl_debug_info_table_t;

#define DEFAULT_DEBUG_INFO_SIZE 128


struct _ftl_subroutine_t {
  fts_symbol_t name;
  fts_atom_list_t instructions;
  struct _ftl_subroutine_t *next;
  struct _ftl_subroutine_t *next_in_stack;

  fts_word_t *bytecode;

  int instruction_count;
  ftl_debug_info_table_t debug_info_table;
};


struct _ftl_program_t {
  float *buffers;		
  ftl_subroutine_t *subroutines;
  ftl_subroutine_t *current_subroutine, *main;
  fts_hash_table_t symbol_table;

  /* Stack of subroutines calls */
  ftl_subroutine_t *subroutine_tos;

  /* Program counter for PC sampling */
  int pc;

  /* Profiling thread */
#ifdef PTHREADS
  pthread_t profile_thread;
#endif
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

static void ftl_debug_info_table_init( ftl_debug_info_table_t *table)
{
  table->size = DEFAULT_DEBUG_INFO_SIZE;
  table->info = (ftl_instruction_debug_info_t *)fts_malloc( table->size * sizeof(ftl_instruction_debug_info_t));
  table->last = 0;
}

static void ftl_debug_info_table_set( ftl_debug_info_table_t *table, int index, fts_object_t *object)
{
  if (index >= table->size)
    {
      int new_size, i;

      new_size = table->size;
      while (new_size <= index)
	new_size *= 2;

      table->info = (ftl_instruction_debug_info_t *)fts_realloc( table->info, new_size * sizeof(ftl_instruction_debug_info_t));

     for ( i = table->size; i < new_size; i++)
       table->info[i].object = 0;

     table->size = new_size;
    }

  ASSERT( index >= 0 && index < table->size);

  table->info[index].object = object;
  table->info[index].profile_count = 0;

  if ( index > table->last)
    table->last = index;
}

static void ftl_debug_info_table_destroy( ftl_debug_info_table_t *table)
{
  fts_free( table->info);
  table->info = 0;
  table->size = 0;
  table->last = 0;
}

/* ********************************************************************** */
/* Subroutines handling                                                   */
/* ********************************************************************** */

static ftl_subroutine_t *
ftl_subroutine_new( fts_symbol_t name)
{
  ftl_subroutine_t *newsubr;

  newsubr = (ftl_subroutine_t *) fts_malloc( sizeof(ftl_subroutine_t));
  if (!newsubr)
    return 0;

  newsubr->name = name;
  fts_atom_list_init( &(newsubr->instructions) );

  newsubr->next = 0;
  newsubr->next_in_stack = 0;
  newsubr->bytecode = 0;
  newsubr->instruction_count = 0;

  ftl_debug_info_table_init( &newsubr->debug_info_table);

  return newsubr;
}

static void
ftl_subroutine_destroy( ftl_subroutine_t *subr)
{
  fts_atom_list_destroy( &(subr->instructions));

  if ( subr->bytecode)
    {
      fts_free( subr->bytecode);
      subr->bytecode = 0;
    }

  ftl_debug_info_table_destroy( &subr->debug_info_table);
}

static fts_status_t
ftl_subroutine_add_call( ftl_subroutine_t *subr, fts_symbol_t name, int argc, const fts_atom_t *argv, fts_object_t *object )
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_CALL);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_symbol( &a, name);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_long( &a, argc);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_atom_list_append( &(subr->instructions), argc, argv);

  /* add debugging info */
  ftl_debug_info_table_set( &subr->debug_info_table, subr->instruction_count, object);
  subr->instruction_count++;

  return fts_Success;
}

static fts_status_t
ftl_subroutine_add_return( ftl_subroutine_t *subr)
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_RETURN);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  /* add debugging info */
  ftl_debug_info_table_set( &subr->debug_info_table, subr->instruction_count, 0);
  subr->instruction_count++;

  return fts_Success;
}


/* ********************************************************************** */
/* Instructions and directives insertion                                  */
/* ********************************************************************** */

ftl_subroutine_t *
ftl_program_add_subroutine( ftl_program_t *prog, fts_symbol_t name)
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

ftl_subroutine_t *
ftl_program_set_current_subroutine( ftl_program_t *prog, ftl_subroutine_t *subr)
{
  ftl_subroutine_t *previous;

  previous = prog->current_subroutine;
  prog->current_subroutine = subr;
  return previous;
}

ftl_subroutine_t *
ftl_program_add_main( ftl_program_t *prog)
{
  prog->main = ftl_program_add_subroutine( prog, fts_new_symbol("main"));

  return prog->main;
}

fts_status_t
ftl_program_add_call( ftl_program_t *prog, fts_symbol_t name, int argc, const fts_atom_t *argv, fts_object_t *object )
{
  if ( !prog->current_subroutine)
    return &ftl_error_uninitialized_program;

  return ftl_subroutine_add_call( prog->current_subroutine, name, argc, argv, object );
}


fts_status_t
ftl_program_add_return( ftl_program_t *prog)
{
  return ftl_subroutine_add_return( prog->current_subroutine);
}

int
ftl_program_add_signal( ftl_program_t *prog, fts_symbol_t name, int size)
{
  ftl_memory_declaration *mdecl;

  mdecl = ftl_memory_declaration_new( size);

  return fts_hash_table_insert( &(prog->symbol_table), name, mdecl);
}


int
ftl_declare_function( fts_symbol_t name, ftl_wrapper_t wrapper)
{
  ftl_function_declaration *fdecl;

  if ( !ftl_functions_table)
    ftl_functions_table = fts_hash_table_new();
  else
    {
      void *data;
  
      if (fts_hash_table_lookup(ftl_functions_table, name,  &data))
	{
	  fdecl = (ftl_function_declaration *)data;
	  fts_hash_table_remove(ftl_functions_table, name);
	  ftl_function_declaration_free(fdecl);
	}
    }

  fdecl = ftl_function_declaration_new( wrapper);

  return fts_hash_table_insert( ftl_functions_table, name, fdecl);
}


/* ********************************************************************** */
/* Initialization and house-keeping                                       */
/* ********************************************************************** */

void
ftl_program_init( ftl_program_t *prog)
{
  prog->buffers = 0;
  prog->subroutines = 0;
  prog->current_subroutine = 0;
  prog->main = 0;
  prog->subroutine_tos = 0;
  prog->pc = -1;

  fts_hash_table_init( &(prog->symbol_table) );
}

ftl_program_t *
ftl_program_new( void )
{
  ftl_program_t *tmp;

  tmp = (ftl_program_t *) fts_malloc(sizeof( ftl_program_t));
  if ( tmp)
    ftl_program_init( tmp);
  return tmp;
}

static void
free_hash_element(fts_symbol_t ignore, void *data, void *user_data)
{
  fts_free(data);
}

void 
ftl_program_destroy( ftl_program_t *prog)
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


void 
ftl_program_free( ftl_program_t *prog)
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

static fts_status_t
ftl_state_machine( fts_atom_list_t *alist, state_fun_t fun, void *user_data)
{
  fts_atom_t *a = 0;	    
  fts_atom_list_iterator_t iter;
  int state, newstate, argc;
  fts_status_t ret;

  state = ST_OPCODE;
  argc  = 0;

  for( fts_atom_list_iterator_init( &iter, alist );
      !fts_atom_list_iterator_end( &iter);
      fts_atom_list_iterator_next( &iter))
    {
      a = fts_atom_list_iterator_current( &iter);

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
	      return &ftl_error_invalid_program;
	    }
	  }
	else
	  return &ftl_error_invalid_program;
	break;
      case ST_CALL_FUN:
	if ( fts_is_symbol( a))
	  newstate = ST_CALL_ARGC;
	else
	  return &ftl_error_invalid_program;
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
	  return &ftl_error_invalid_program;
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
	return ret;
      state = newstate;
    }
  
  return fts_Success;
}



/* ********************************************************************** */
/* Compilation                                                            */
/* ********************************************************************** */

static int
ftl_program_allocate_signals( ftl_program_t *prog)
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

      m = (ftl_memory_declaration *)fts_hash_table_iterator_current_data( &iter);

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

      m = (ftl_memory_declaration *)fts_hash_table_iterator_current_data( &iter);

      m->address = p;

      p = p + m->size;
    }

  return 1;
}


struct compile_info_portable {
  ftl_program_t *prog;
  fts_word_t *bytecode;
};

static fts_status_t
compile_portable_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
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
      ftl_function_declaration *fdecl;

      fts_hash_table_lookup( ftl_functions_table, fts_get_symbol(a), (void **)&fdecl);
      fts_word_set_ptr( bytecode, (void *)fdecl->wrapper);
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
	ftl_memory_declaration *mdecl;
	
	fts_hash_table_lookup( &(info->prog->symbol_table), fts_get_symbol(a), (void **)&mdecl);
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

static fts_status_t
bytecode_size_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
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

int
ftl_program_compile_portable( ftl_program_t *prog)
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
      ret = ftl_state_machine( &subr->instructions, bytecode_size_state_fun, &size);
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
      ret = ftl_state_machine( &subr->instructions, compile_portable_state_fun, &info);
      if ( ret != fts_Success)
	return 0;

#if 0
      fprintf( stderr, "subr %s last %d instruction_count %d\n", 
	       fts_symbol_name(subr->name), 
	       subr->debug_info_table.last,
	       subr->instruction_count);
#endif
    }

  return 1;
}

int
ftl_program_compile( ftl_program_t *prog)
{
  return ftl_program_compile_portable(prog);
}


/* ********************************************************************** */
/* Print functions                                                        */
/* ********************************************************************** */

#ifdef DEBUG
static void ftl_print_functions_table( void)
{
  fts_hash_table_iterator_t it;
  ftl_function_declaration *decl;
  fts_symbol_t s;

  for( fts_hash_table_iterator_init( &it, ftl_functions_table);
      ! fts_hash_table_iterator_end( &it);
      fts_hash_table_iterator_next( &it) )
    {
      decl = (ftl_function_declaration *)fts_hash_table_iterator_current_data(&it);
      s = fts_hash_table_iterator_current_symbol( &it);
      post( "/* Function %s (0x%x) */\n", fts_symbol_name(s), decl->wrapper);
    }
}
#endif

static void ftl_print_atom( char *s, const fts_atom_t *a)
{
  if (fts_is_long(a))
    sprintf( s, "%ld", fts_get_long(a));
  else if (fts_is_float(a))
    sprintf( s, "%f", fts_get_float(a));
  else if (fts_is_symbol(a))
    sprintf( s, "%s", fts_symbol_name(fts_get_symbol(a)));
  else if (fts_is_ptr(a))
    sprintf( s, "0x%x", fts_get_ptr(a));
  else
    sprintf( s, "?");
}

void ftl_program_print_signals( const ftl_program_t *prog)
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

      m = (ftl_memory_declaration *)fts_hash_table_iterator_current_data( &iter);
      s = fts_hash_table_iterator_current_symbol( &iter);
      post( "float %s[%d];  /* adress 0x%x */\n", fts_symbol_name(s), m->size, m->address);
    }
  post( "\n");
}

void ftl_program_print_signals_count( const ftl_program_t *prog)
{
  post( "ftl_program : signals %d\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));
}

struct print_info {
  int pc;
  char line[256];
  ftl_instruction_debug_info_t *debug_info;
};

static fts_status_t print_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  struct print_info *info = (struct print_info *)user_data;
  char buffer[64];
  static int pc = 0;

  buffer[0] = ' ';
  switch( state) {
  case ST_OPCODE:
    switch( fts_get_long( a)) {
    case FTL_OPCODE_RETURN:
      post( "/* %4d */   return;\n", info->pc);
      break;
    case FTL_OPCODE_CALL:
      pc = info->pc;
      info->pc++;
      break;
    }
    break;
  case ST_CALL_FUN:
    sprintf( info->line, "/* %4d */   %s( ", pc, fts_symbol_name(fts_get_symbol(a)));
    break;
  case ST_CALL_ARGV:
    if (fts_is_ptr(a))
      sprintf( buffer, "(void *)0x%x", fts_get_ptr(a));
    else
      ftl_print_atom( buffer, a);
    strcat( info->line, buffer);
    if ( newstate == ST_OPCODE)
      {
	fts_object_t *object;

	strcat( info->line, " );");
	object = info->debug_info->object;
	if (object)
	  post( "%s /* object %s */\n", info->line, fts_symbol_name( fts_object_get_class_name(object)));
	else
	  post( "%s /* object unknown */\n", info->line);
	  
	info->debug_info++;
      }
    else
      strcat( info->line, ", ");
    break;
  }

  return fts_Success;
}


void ftl_program_print( const ftl_program_t *prog )
{
  ftl_subroutine_t *subr;
  struct print_info info;

  ftl_program_print_signals(prog);

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      post( "%s()\n", fts_symbol_name( subr->name));
      post( "{\n");
      info.line[0] = 0;
      info.pc = 0;
      info.debug_info = subr->debug_info_table.info;
      ftl_state_machine( &subr->instructions, print_state_fun, &info);
      post( "}\n\n");
    }
}


void ftl_program_print_bytecode( const ftl_program_t *prog)
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

void ftl_program_call_subr( ftl_program_t *prog, ftl_subroutine_t *subr)
{
  fts_word_t *bytecode;

  prog->pc = -1;
  subr->next_in_stack = prog->subroutine_tos;
  prog->subroutine_tos = subr;

  bytecode = subr->bytecode;
  while (fts_word_get_long(bytecode))
    {
      ftl_wrapper_t w;
      int argc;
      
      prog->pc++;
      w = (ftl_wrapper_t) fts_word_get_ptr( bytecode);
      argc = fts_word_get_long( bytecode+1);
      (*w)(bytecode+2);
      bytecode += (argc + 2);
    }

  prog->pc = -1;
  prog->subroutine_tos = subr->next_in_stack;
  subr->next_in_stack = 0;
}

void ftl_program_run( ftl_program_t *prog )
{
  ftl_program_call_subr( prog, prog->main);
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Profile thread management                                                   */
/*                                                                             */
/* --------------------------------------------------------------------------- */

#ifdef SLOW
#define DEFAULT_PROFILE_PERIOD_NS 130000000
#else
#define DEFAULT_PROFILE_PERIOD_NS 23000000
#endif

#ifdef PTHREADS
static struct timespec profile_period = { 0, DEFAULT_PROFILE_PERIOD_NS };


static pthread_t profile_thread;
#endif

static void profile_thread_cleanup( void *arg)
{
#ifdef DEBUG
  fprintf( stderr, "Profile thread has exited\n");
#endif
}

static void ftl_program_pc_sample( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;
  int pc;

  subr = prog->subroutine_tos;
  pc = prog->pc;

  if (subr != 0 && pc >= 0)
    {
#if 0
      fprintf( stderr, "subr %s pc %d\n", fts_symbol_name( subr->name), pc);
#endif

      ASSERT( pc <= subr->debug_info_table.last);

      subr->debug_info_table.info[pc].profile_count += 1;
    }
}

static void *profile_thread_fun( void *arg)
{
  ftl_program_t *prog;

  prog = (ftl_program_t *)arg;

#ifdef DEBUG
  fprintf( stderr, "Profile thread has started\n");
#endif

#ifdef PTHREADS
  pthread_cleanup_push( profile_thread_cleanup, 0);


  while (1)
    {
      ftl_program_pc_sample( prog);
      nanosleep( &profile_period, NULL);
    }


  pthread_cleanup_pop( 0);
#endif
}

static void profile_thread_start( ftl_program_t *prog)
{
#ifdef PTHREADS
  pthread_create( &prog->profile_thread, 0, profile_thread_fun, prog);
#endif
}

static void profile_thread_stop( ftl_program_t *prog)
{
#ifdef PTHREADS
  pthread_cancel( prog->profile_thread);
#endif
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Report by metaclass related functions                                       */
/*                                                                             */
/* --------------------------------------------------------------------------- */

typedef struct {
  void *object;
  int profile_count;
} report_entry_t;

typedef struct _report_node_t report_node_t;

struct _report_node_t {
  report_entry_t entry;
  report_node_t *left, *right;
};

static void report_tree_add_node( report_node_t **root, void *object, int profile_count)
{
  if ( ! *root)
    {
      *root = (report_node_t *)fts_malloc( sizeof( report_node_t));
      (*root)->entry.object = object;
      (*root)->entry.profile_count = profile_count;
      (*root)->left = 0;
      (*root)->right = 0;
    }
  else if (object < (*root)->entry.object)
    report_tree_add_node( &((*root)->left), object, profile_count);
  else if (object > (*root)->entry.object)
    report_tree_add_node( &((*root)->right), object, profile_count);
  else
    (*root)->entry.profile_count += profile_count;
}

static void report_tree_build( ftl_program_t *prog, report_node_t **tree)
{
  ftl_subroutine_t *subr;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      ftl_instruction_debug_info_t *debug_info;
      int i, last;

      debug_info = subr->debug_info_table.info;
      last = subr->debug_info_table.last;

      for ( i = 0; i < last; i++)
	{
	  if (debug_info->profile_count && debug_info->object)
	    report_tree_add_node( tree, debug_info->object->cl->mcl, debug_info->profile_count);

	  debug_info++;
	}
    }
}

static void report_tree_count( report_node_t *tree, int *nobjects, int *nevents)
{
  if (tree)
    {
      *nobjects += 1;
      *nevents += tree->entry.profile_count;
      report_tree_count( tree->left, nobjects, nevents);
      report_tree_count( tree->right, nobjects, nevents);
    }
}

static void report_tree_copy_aux( report_node_t *tree, report_entry_t **p)
{
  if (tree)
    {
      memcpy( *p, &tree->entry, sizeof( tree->entry));
      (*p)++;
      report_tree_copy_aux( tree->left, p);
      report_tree_copy_aux( tree->right, p);
    }
}

static void report_tree_copy( report_node_t *tree, report_entry_t *tab)
{
  report_tree_copy_aux( tree, &tab);
}

static void report_tree_free( report_node_t *tree)
{
  if (tree)
    {
      report_tree_free( tree->left);
      report_tree_free( tree->right);
      fts_free( tree);
    }
}

static const char *metaclass_name_function(void *object)
{
  return fts_symbol_name( ((fts_metaclass_t *)object)->name);
}

static const char *object_name_function(void *object)
{
  return fts_symbol_name( fts_object_get_class_name((fts_object_t *)object));
}

static void report_tree_print( report_node_t *tree, const char *(*name_function)(void *))
{
  if (tree)
    {
      fprintf( stderr, "%s %d\n", (*name_function)(tree->entry.object), tree->entry.profile_count);
      report_tree_print( tree->left, name_function);
      report_tree_print( tree->right, name_function);
    }
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* FTL profiling API                                                           */
/*                                                                             */
/* --------------------------------------------------------------------------- */

void ftl_program_start_profiler( ftl_program_t *prog)
{
  profile_thread_start( prog);
}

void ftl_program_stop_profiler( ftl_program_t *prog)
{
  profile_thread_stop( prog);
}

void ftl_program_clear_profile_data( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      ftl_instruction_debug_info_t *debug_info;
      int i, last;

      debug_info = subr->debug_info_table.info;
      last = subr->debug_info_table.last;

      for ( i = 0; i < last; i++)
	{
	  debug_info->profile_count = 0;
	  debug_info++;
	}
    }
}

static int report_compare( const void *r1, const void *r2)
{
  return ((const report_entry_t *)r2)->profile_count - ((const report_entry_t *)r1)->profile_count;
}

void ftl_program_show_profile_by_class( ftl_program_t *prog)
{
  report_node_t *tree = 0;
  int nobjects = 0, nevents = 0, i;
  report_entry_t *tab;

  report_tree_build( prog, &tree);

  report_tree_count( tree, &nobjects, &nevents);

  tab = (report_entry_t *)fts_malloc( sizeof(report_entry_t) * nobjects);

  report_tree_copy( tree, tab);

  qsort( tab, nobjects, sizeof( report_entry_t), report_compare);

  post( "Reporting profile (%d events)\n", nevents);
  for ( i = 0; i < nobjects; i++)
    {
      float p;

      p = 100.0*((float)tab[i].profile_count/nevents);
      post( "%3.2f%% %s\n", p, metaclass_name_function( tab[i].object));
    }

  fts_free( tab);

  report_tree_free( tree);
}


