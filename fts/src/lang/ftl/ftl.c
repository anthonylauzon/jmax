#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/ftl.h"
#include "lang/veclib/include/veclib.h"



/* FTL opcodes */
typedef enum {
  FTL_OPCODE_NOP = 123,      /* no operation */
  FTL_OPCODE_RETURN,         /* return from FTL interpreter */
  FTL_OPCODE_CALL,           /* call a wrapper function */
  FTL_OPCODE_CALL_SUBR_COND  /* call a FTL subroutine conditionnaly */
} ftl_opcode_t;

struct _ftl_subroutine_t {
  fts_symbol_t name;
  fts_atom_list_t instructions;
  struct _ftl_subroutine_t *next;

  fts_word_t *bytecode;
};

struct _ftl_program_t {
  float *buffers;		
  ftl_subroutine_t *subroutines;
  ftl_subroutine_t *current_subroutine, *main;
  fts_hash_table_t symbol_table;
};

static fts_hash_table_t *ftl_functions_table = 0;

fts_status_description_t ftl_error =
{
  "FTL unspecified error : appeler le service apres-vente"
};

fts_status_description_t ftl_error_invalid_program =
{
  "invalid FTL program"
};

fts_status_description_t ftl_error_uninitialized_program =
{
  "uninitialized FTL program"
};

static void ftl_call_subr_cond( fts_word_t *argv);

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
  newsubr->bytecode = 0;

  newsubr->next = 0;

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
}

static fts_status_t
ftl_subroutine_add_call( ftl_subroutine_t *subr, fts_symbol_t name, int argc, const fts_atom_t *argv )
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_CALL);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_symbol( &a, name);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_long( &a, argc);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_atom_list_append( &(subr->instructions), argc, argv);

  return fts_Success;
}

static fts_status_t
ftl_subroutine_add_return( ftl_subroutine_t *subr)
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_RETURN);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  return fts_Success;
}

static fts_status_t
ftl_subroutine_add_call_subroutine_cond( ftl_subroutine_t *subr, int *pstate, ftl_subroutine_t *called_subr)
{
  fts_atom_t a;

  fts_set_long( &a, FTL_OPCODE_CALL_SUBR_COND);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_ptr( &a, pstate);
  fts_atom_list_append( &(subr->instructions), 1, &a);

  fts_set_ptr( &a, called_subr);
  fts_atom_list_append( &(subr->instructions), 1, &a);

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
ftl_program_add_call( ftl_program_t *prog, fts_symbol_t name, int argc, const fts_atom_t *argv )
{
  if ( !prog->current_subroutine)
    return &ftl_error_uninitialized_program;

  return ftl_subroutine_add_call( prog->current_subroutine, name, argc, argv );
}


fts_status_t
ftl_program_add_call_subroutine_cond( ftl_program_t *prog, int *pstate, ftl_subroutine_t *called_subr)
{
  return ftl_subroutine_add_call_subroutine_cond( prog->current_subroutine, pstate, called_subr);
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
  ST_CALL_FUN, ST_CALL_ARGC, ST_CALL_ARGV,
  ST_CSUBRCND_STATE, ST_CSUBRCOND_SUBR 
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
	    case FTL_OPCODE_CALL_SUBR_COND:
	      newstate = ST_CSUBRCND_STATE;
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
      case ST_CSUBRCND_STATE:
	if ( fts_is_ptr( a))
	  newstate = ST_CSUBRCOND_SUBR;
	else
	  return &ftl_error_invalid_program;
	break;
      case ST_CSUBRCOND_SUBR:
	if ( fts_is_ptr( a))
	  newstate = ST_OPCODE;
	else
	  return &ftl_error_invalid_program;
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
/* Validity check                                                         */
/* ********************************************************************** */

static fts_status_t
is_valid_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  return fts_Success;
}

int
ftl_program_is_valid( ftl_program_t *prog)
{
  ftl_subroutine_t *subr;
  fts_status_t ret;

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      ret = ftl_state_machine( &subr->instructions, is_valid_state_fun, 0);
      if( ret != fts_Success)
	return 0;
    }
  return 1;
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
  case ST_CSUBRCND_STATE:
    fts_word_set_ptr( bytecode++, (void *) ftl_call_subr_cond);
    fts_word_set_long( bytecode++, 2);
    fts_word_set_ptr( bytecode++, (void *) fts_get_ptr(a));
    break;
  case ST_CSUBRCOND_SUBR:
    fts_word_set_ptr( bytecode++, (void *) fts_get_ptr(a));
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
  case ST_CSUBRCND_STATE:
    *ps += 4;
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

  if ( !ftl_program_is_valid( prog))
    return 0;

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
    }

  return 1;
}


int
ftl_program_compile( ftl_program_t *prog)
{
  return ftl_program_compile_portable(prog);
}



/* ********************************************************************** */
/* C code generation functions                                            */
/* ********************************************************************** */

static void
ftl_print_atom( char *s, const fts_atom_t *a)
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

static void
ftl_program_generate_C_signals_declarations( const ftl_program_t *prog)
{
  fts_hash_table_iterator_t iter;

  post( "/* FTL signals declarations */\n");

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;
      fts_symbol_t s;

      m = (ftl_memory_declaration *)fts_hash_table_iterator_current_data( &iter);
      s = fts_hash_table_iterator_current_symbol( &iter);
      post( "static float %s[%d];\n", fts_symbol_name(s), m->size); 
    }
    
  post("\n");
}

static int variable_counter = 0;

typedef struct variable_t {
  struct {
    const char *type_name;
    char variable_name[32];
    void *p;
  } var;
  struct variable_t *next;
} variable_t;

static variable_t *variable_list = 0;

static void
variable_new( void *p)
{
  variable_t **ppv, *tmp;
  const char *type_name;

  ppv = &variable_list;
  while (*ppv)
    {
      if ( (*ppv)->var.p == p)
	return;

      ppv = &( (*ppv)->next );
    }

  tmp = fts_malloc( sizeof( variable_t));
  if (!tmp)
    return;

  type_name = ftl_data_get_type_name( p);
  if (!type_name)
    return;

  tmp->var.type_name = type_name;
  sprintf( tmp->var.variable_name, "_ftl_data__%d", variable_counter);
  variable_counter++;
  tmp->var.p = p;
  tmp->next = 0;

  *ppv = tmp;
}

static void
variable_list_free( void)
{
  variable_t *pv, *next;

  pv = variable_list;
  while (pv)
    {
      next = pv->next;
      fts_free( pv);
      pv = next;
    }

  variable_list = 0;
}

static char *
variable_get( void *p)
{
  variable_t *pv;

  pv = variable_list;
  while (pv)
    {
      if (pv->var.p == p)
	return pv->var.variable_name;
      pv = pv->next;
    }

  return 0;
}


static void
variable_generate_declarations( void)
{
  variable_t *pv;

  post( "/* FTL states declarations */\n");
  for ( pv = variable_list; pv; pv = pv->next)
    {
      post( "static %s %s;\n", pv->var.type_name, pv->var.variable_name);
    }
  post("\n");

}


static fts_status_t
generate_C_variable_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  if ( state == ST_CALL_ARGV && fts_is_ptr(a))
    {
      variable_new( fts_get_ptr(a) );
    }

  return fts_Success;
}

struct generate_c_info {
  int call_count;
  char line[256];
};

static fts_status_t
generate_C_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  struct generate_c_info *info = (struct generate_c_info *)user_data;
  char buffer[64];

  switch( state) {
  case ST_OPCODE:
    break;
  case ST_CALL_FUN:
    sprintf( info->line, "  %s_ ## PART ( %d, ", fts_symbol_name(fts_get_symbol(a)), info->call_count);
    info->call_count++;
    break;
  case ST_CALL_ARGV:
    if (fts_is_ptr(a))
      {
	const char *name;

	name = variable_get( fts_get_ptr(a) );
	if (name)
	  sprintf( buffer, "%s", name); /* &%s or not &%s. We choose not & */
	else
	  sprintf( buffer, "(void *)0x%x", fts_get_ptr(a));
      }
    else
      ftl_print_atom( buffer, a);
    strcat( info->line, buffer);
    if ( newstate == ST_OPCODE)
      {
	strcat( info->line, " ); \\\n");
	post( info->line);
      }
    else
      strcat( info->line, ", ");
    break;
  case ST_CSUBRCND_STATE:
    post( "  if ( *((int *)0x%x))\n", fts_get_ptr(a));
    break;
  case ST_CSUBRCOND_SUBR:
    {
      ftl_subroutine_t *subr = (ftl_subroutine_t *)fts_get_ptr(a);

      post( "    %s();\n", fts_symbol_name( subr->name));
    }
    break;
  }
  return fts_Success;
}

void
ftl_program_generate_C_code( const ftl_program_t *prog )
{
  ftl_subroutine_t *subr;

  ftl_program_generate_C_signals_declarations( prog);

  variable_list_free();
  for( subr = prog->subroutines; subr; subr = subr->next)
    ftl_state_machine( &subr->instructions, generate_C_variable_state_fun, 0);

  variable_generate_declarations();

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      struct generate_c_info info;

      info.call_count = 0;

      post( "#undef %s_DSPCHAIN\n", fts_symbol_name( subr->name));
      post( "#define %s_DSPCHAIN(PART) \\\n", fts_symbol_name( subr->name));
      ftl_state_machine( &subr->instructions, generate_C_state_fun, &info);
      post( "\n");

      post( "%s( void )\n", fts_symbol_name( subr->name));
      post( "{\n");
      post( "  int i;\n");
      post( "  /* Declarations */\n");
      post( "  %s_DSPCHAIN(declarations);\n", fts_symbol_name( subr->name));
      post( "\n");

      post( "  /* Loop prelude */\n");
      post( "  %s_DSPCHAIN(prelude);\n", fts_symbol_name( subr->name));
      post( "\n");

      post( "  /* Loop body */\n");
      post( "  for ( i = 0; i < 64 /* !!! */; i++)\n");
      post( "    {\n");
      post( "      %s_DSPCHAIN(body);\n", fts_symbol_name( subr->name));
      post( "    }\n");
      post( "\n");

      post( "  /* Loop postlude */\n");
      post( "  %s_DSPCHAIN(postlude);\n", fts_symbol_name( subr->name));
      post( "\n");

      post( "}\n");
    }
}

/* ********************************************************************** */
/* Print functions                                                        */
/* ********************************************************************** */

#ifdef DEBUG
static void
ftl_print_functions_table( void)
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
      post( ".function %s,0x%x\n", fts_symbol_name(s), decl->wrapper);
    }
}
#endif

void
ftl_program_print_signals( const ftl_program_t *prog)
{
  fts_hash_table_iterator_t iter;

  post( "ftl_program : signals %d\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));

  for( fts_hash_table_iterator_init( &iter, &(prog->symbol_table));
      ! fts_hash_table_iterator_end( &iter);
      fts_hash_table_iterator_next( &iter) )
    {
      ftl_memory_declaration *m;
      fts_symbol_t s;

      m = (ftl_memory_declaration *)fts_hash_table_iterator_current_data( &iter);
      s = fts_hash_table_iterator_current_symbol( &iter);
      post( ".signal %s,%d,0x%x\n", fts_symbol_name(s), m->size, m->address);
    }
}

void
ftl_program_print_signals_count( const ftl_program_t *prog)
{
  post( "ftl_program : signals %d\n", 
       fts_hash_table_get_count(&(prog->symbol_table)));
}

static fts_status_t
print_state_fun( int state, int newstate, fts_atom_t *a, void *user_data)
{
  char *line = (char *)user_data;
  char buffer[64];

  buffer[0] = ' ';
  switch( state) {
  case ST_OPCODE:
    switch( fts_get_long( a)) {
    case FTL_OPCODE_RETURN:
      post( "return\n");
      break;
    case FTL_OPCODE_CALL:
      strcpy( line, "call ");
      break;
    case FTL_OPCODE_CALL_SUBR_COND:
      strcpy( line, "call subroutine conditionnaly ");
      break;
    }
    break;
  case ST_CALL_FUN:
    strcat( line, fts_symbol_name(fts_get_symbol(a)));
    break;
  case ST_CALL_ARGC:
    if ( newstate == ST_OPCODE)
      post("%s\n", line);
    break;
  case ST_CALL_ARGV:
    ftl_print_atom( buffer+1, a);
    strcat( line, buffer);
    if ( newstate == ST_OPCODE)
      post("%s\n",line);
    break;
  case ST_CSUBRCND_STATE:
    ftl_print_atom( buffer+1, a);
    strcat( line, buffer);
    break;
  case ST_CSUBRCOND_SUBR:
    {
      ftl_subroutine_t *subr = (ftl_subroutine_t *)fts_get_ptr(a);

      sprintf( buffer+1, "%s", fts_symbol_name( subr->name));
    }
    strcat( line, buffer);
    post( "%s\n", line);
    break;
  }
  return fts_Success;
}


void
ftl_program_print( const ftl_program_t *prog )
{
  ftl_subroutine_t *subr;
  char line[256];

  ftl_program_print_signals(prog);

  for( subr = prog->subroutines; subr; subr = subr->next)
    {
      post( "subroutine %s {\n", fts_symbol_name( subr->name));
      line[0] = 0;
      ftl_state_machine( &subr->instructions, print_state_fun, line);
      post( "}\n");
    }
}


void
ftl_program_print_bytecode( const ftl_program_t *prog)
{
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
}


/* ********************************************************************** */
/* Run functions                                                          */
/* ********************************************************************** */

static void
ftl_run_portable_bytecode( fts_word_t *bytecode)
{
  for( ; fts_word_get_long(bytecode); )
    {
      ftl_wrapper_t w;
      int argc;
      
      w = (ftl_wrapper_t) fts_word_get_ptr( bytecode);
      argc = fts_word_get_long( bytecode+1);
      (*w)(bytecode+2);
      bytecode += (argc + 2);
    }
}

void
ftl_program_run( const ftl_program_t *prog )
{
  ftl_run_portable_bytecode( prog->main->bytecode);
}


static void
ftl_call_subr_cond( fts_word_t *argv)
{
  ftl_subroutine_t *subr = (ftl_subroutine_t *)fts_word_get_ptr(argv+1);
  int *p = (int *)fts_word_get_ptr(argv);

  if (*p)
    ftl_run_portable_bytecode( subr->bytecode);
}




