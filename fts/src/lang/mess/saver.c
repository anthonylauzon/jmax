#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/vm.h"
#include "lang/mess/loader.h"

/* Private structure */

#define SYMBOL_TABLE_SIZE 64

typedef struct fts_bmax_file
{
  int fd;
  fts_binary_file_header_t header; 
  fts_symbol_t *symbol_table;
  int symbol_table_size;
  int symbol_table_fill;
} fts_bmax_file_t;


fts_bmax_file_t *
fts_open_bmax_file_for_writing(const char *name)
{
  fts_bmax_file_t *f;
  
  /* allocate a bmax descriptor, and initialize it */

  f = (fts_bmax_file_t *) fts_malloc(sizeof(fts_bmax_file_t));
  f->symbol_table_size = SYMBOL_TABLE_SIZE;
  f->symbol_table = (fts_symbol_t *) fts_malloc(f->symbol_table_size * sizeof(fts_symbol_t));

  /* Initialize the header */

  f->header.magic_number = FTS_BINARY_FILE_MAGIC;
  f->header.code_size = 0;
  f->header.n_symbols = 0;

  /* Open the file */

  f->fd = open(name, O_WRONLY);

  if (f->fd < 0)
    {
      perror("fts_open_bmax_file_for_writing: open ");
      return 0;
    }

  /* write the init header to the file */

  if (write(f->fd, &(f->header), sizeof(fts_binary_file_header_t)) < sizeof(fts_binary_file_header_t))
    {
      perror("fts_open_bmax_file_for_writing: write header ");
      return 0;
    }

  /* return the file */

  return f;
}


void
fts_close_bmax_file_for_writing(fts_bmax_file_t *f)
{
  int i;

  /* Write the symbol table */

  for (i = 0; i < f->header.n_symbols; i++)
    write(f->fd, fts_symbol_name(f->symbol_table[i]), strlen(fts_symbol_name(f->symbol_table[i]))+1);
  
  /* seek to the beginning and rewrite the header */

  lseek(f->fd, 0, SEEK_SET);
  write(f->fd, &(f->header), sizeof(fts_binary_file_header_t));

  /* close the file */

  close(f->fd);

  /* free the bmax file descriptor */

  fts_free(f->symbol_table);
  fts_free(f);
}

/* Aux functions  for file building */



static int fts_bmax_add_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  int i;

  /* First, search for the symbol in the symbol table */

  for (i = 0; i < f->symbol_table_fill; i++)
    if (f->symbol_table[i] == sym)
      return i;
  
  /* is not there, add it, resizing the table if there
   * is no place
   */

  if (f->symbol_table_fill >= f->symbol_table_size)
    {
      /* resize symbol table (double it) */

      f->symbol_table_size = (f->symbol_table_size * 3) / 2;
      f->symbol_table = fts_realloc((void *)f->symbol_table, f->symbol_table_size);
    }

  f->symbol_table[f->symbol_table_fill] = sym;

  return (f->symbol_table_fill)++;
}
				 


/* One functions for each opcode */

void
fts_bmax_code_return(fts_bmax_file_t *f)
{
  /* RETURN */

  fts_word_t w;

  fts_word_set_int(&w, FVM_RETURN);
  write(f->fd, &w, sizeof(fts_word_t));
}

void
fts_bmax_code_push_int(fts_bmax_file_t *f, int value)
{
  /* PUSH_INT   <int>   */

  fts_word_t w;
  fts_word_t i;

  fts_word_set_int(&w, FVM_PUSH_INT);
  fts_word_set_int(&i, value);
  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}


void
fts_bmax_code_push_float(fts_bmax_file_t *f, float value)
{
  /* PUSH_FLOAT <float> */

  fts_word_t w;
  fts_word_t fw;

  fts_word_set_int(&w, FVM_PUSH_FLOAT);
  fts_word_set_float(&fw, value);
  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &fw, sizeof(fts_word_t));
}


void
fts_bmax_code_push_sym(fts_bmax_file_t *f, fts_symbol_t sym)
{
  fts_word_t w;
  fts_word_t s;

  if (fts_is_builtin_symbol(sym))
    {
      /* PUSH_BUILTIN_SYM   <int>  */

      fts_word_set_int(&w, FVM_PUSH_BUILTIN_SYM);
      fts_word_set_int(&s, fts_get_builtin_symbol_index(sym));
    }
  else
    {
      /* PUSH_SYM   <int>   */

      fts_word_set_int(&w, FVM_PUSH_FLOAT);
      fts_word_set_int(&s, fts_bmax_add_symbol(f, sym));
    }

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &s, sizeof(fts_word_t));
}


void
fts_bmax_code_pop_arg(fts_bmax_file_t *f, int value)
{
  /* POP_ARGS    <int>   // pop n values  from the argument stack */

  fts_word_t w;
  fts_word_t i;


  fts_word_set_int(&w, FVM_POP_ARGS);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}

void
fts_bmax_code_push_obj(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ   <objidx> */

  fts_word_t w;
  fts_word_t i;


  fts_word_set_int(&w, FVM_PUSH_OBJ);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}

void
fts_bmax_code_mv_obj(fts_bmax_file_t *f, int value)
{
  /* MV_OBJ     <objidx> */

  fts_word_t w;
  fts_word_t i;

  fts_word_set_int(&w, FVM_MV_OBJ);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}



void
fts_bmax_code_pop_objs(fts_bmax_file_t *f, int value)
{
  /* POP_OBJS    <int> */

  fts_word_t w;
  fts_word_t i;

  fts_word_set_int(&w, FVM_POP_OBJS);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}


void
fts_bmax_code_make_obj(fts_bmax_file_t *f, int value)
{
  /* MAKE_OBJ   <nargs> */

  fts_word_t w;
  fts_word_t i;

  fts_word_set_int(&w, FVM_MAKE_OBJ);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}

void
fts_bmax_code_put_prop(fts_bmax_file_t *f, fts_symbol_t sym)
{
  fts_word_t w;
  fts_word_t s;

  if (fts_is_builtin_symbol(sym))
    {
      /* PUT_BUILTIN_PROP   <sym> */

      fts_word_set_int(&w, FVM_PUT_BUILTIN_PROP);
      fts_word_set_int(&s, fts_get_builtin_symbol_index(sym));
    }
  else
    {
      /* PUT_PROP   <sym> */

      fts_word_set_int(&w, FVM_PUT_PROP);
      fts_word_set_int(&s, fts_bmax_add_symbol(f, sym));
    }

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &s, sizeof(fts_word_t));
}

void
fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs)
{
  fts_word_t w;
  fts_word_t winlet;
  fts_word_t s;
  fts_word_t wnargs;

  fts_word_set_int(&winlet, inlet);
  fts_word_set_int(&wnargs, nargs);

  if (fts_is_builtin_symbol(sel))
    {
      /* OBJ_BUILTIN_MESS   <inlet> <sel> <nargs> */

      fts_word_set_int(&w, FVM_PUT_BUILTIN_PROP);
      fts_word_set_int(&s, fts_get_builtin_symbol_index(sel));
    }
  else
    {
      /* OBJ_MESS   <inlet> <sel> <nargs> */

      fts_word_set_int(&w, FVM_PUT_PROP);
      fts_word_set_int(&s, fts_bmax_add_symbol(f, sel));
    }

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &winlet, sizeof(fts_word_t));
  write(f->fd, &s, sizeof(fts_word_t));
  write(f->fd, &wnargs, sizeof(fts_word_t));
}


void
fts_bmax_code_push_obj_table(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ_TABLE <int> */

  fts_word_t w;
  fts_word_t i;

  fts_word_set_int(&w, FVM_PUSH_OBJ_TABLE);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}

void
fts_bmax_code_pop_obj_table(fts_bmax_file_t *f)
{
  /* POP_OBJ_TABLE */

  fts_word_t w;

  fts_word_set_int(&w, FVM_POP_OBJ_TABLE);

  write(f->fd, &w, sizeof(fts_word_t));
}

void
fts_bmax_code_connect(fts_bmax_file_t *f)
{
  /* CONNECT */
  /* POP_OBJ_TABLE */

  fts_word_t w;

  fts_word_set_int(&w, FVM_CONNECT);

  write(f->fd, &w, sizeof(fts_word_t));
}



/*
 * Higher level (Macro) functions, convenience functions.
 * 
 */

/* atom args*/

void
fts_bmax_code_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at)
{
  int i;

  for (i = 0; i < ac; i++)
    {
      const fts_atom_t *a = &(at[i]);

      if (fts_is_int(a))
	{
	}
      else if (fts_is_float(a))
	{
	}
      else if (fts_is_symbol(a))
	{
	}
    }
}

/* Objects, connections, patchers */

void
void_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj)
{
}


void
void_bmax_code_new_connection(fts_bmax_file_t *f)
{
}

void
void_bmax_code_new_patcher(fts_bmax_file_t *f, fts_object_t *patcher)
{
}
