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

  f->fd = open(name, O_WRONLY | O_CREAT);

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
fts_close_bmax_file(fts_bmax_file_t *f)
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

static int
fts_bmax_find_objidx(fts_object_t *obj)
{
  /* As object idx, i.e. the location where to store
     the object in the object frame, we compute the
     position of the object in the child list of a patcher.
     Not so easy to store in the object, because change
     with editing operations, so at the end may be simpler
     to compute it, or at most to have an hash table, or to cache
     the value in a object property or slot
     */

  int i;
  fts_patcher_t *patcher = fts_object_get_patcher(obj);
  fts_object_t *p;

  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (obj == p)
	return i;

      i++;
    }

  /* If we exit from here, a big coding error have been done,
     or a message system inconsistency discovered !!! */

  return -1;
}

static int
fts_bmax_count_childs(fts_patcher_t *patcher)
{
  /* Count the childs in a patcher */

  fts_object_t *p;
  int i = 0;

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    i++;

  return i;
}


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

static void
fts_bmax_code_return(fts_bmax_file_t *f)
{
  /* RETURN */

  fts_word_t w;

  fts_word_set_int(&w, FVM_RETURN);
  write(f->fd, &w, sizeof(fts_word_t));
}

static void
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


static void
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


static void
fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
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


static void
fts_bmax_code_pop_args(fts_bmax_file_t *f, int value)
{
  /* POP_ARGS    <int>   // pop n values  from the argument stack */

  fts_word_t w;
  fts_word_t i;


  fts_word_set_int(&w, FVM_POP_ARGS);
  fts_word_set_int(&i, value);

  write(f->fd, &w, sizeof(fts_word_t));
  write(f->fd, &i, sizeof(fts_word_t));
}


static void
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

static void
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



static void
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


static void
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

static void
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

static void
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


static void
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

static void
fts_bmax_code_pop_obj_table(fts_bmax_file_t *f)
{
  /* POP_OBJ_TABLE */

  fts_word_t w;

  fts_word_set_int(&w, FVM_POP_OBJ_TABLE);

  write(f->fd, &w, sizeof(fts_word_t));
}

static void
fts_bmax_code_connect(fts_bmax_file_t *f)
{
  /* CONNECT */

  fts_word_t w;

  fts_word_set_int(&w, FVM_CONNECT);

  write(f->fd, &w, sizeof(fts_word_t));
}



/*
 * Higher level (Macro) functions, convenience functions.
 * 
 */


/* atom args: pushed backward, to have them in the right order
 * in the stack at execution time
 */


static void
fts_bmax_code_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at)
{
  int i;

  for (i = (ac - 1); i >= 0; i--)
    {
      const fts_atom_t *a = &(at[i]);

      if (fts_is_int(a))
	fts_bmax_code_push_int(f, fts_get_int(a));
      else if (fts_is_float(a))
	fts_bmax_code_push_float(f, fts_get_float(a));
      else if (fts_is_symbol(a))
	fts_bmax_code_push_symbol(f, fts_get_symbol(a));
    }
}

/* Objects, connections, patchers */

static void
fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx)
{
  /* Push the object arguments, make the object, and put it in the object table, then push the args 
   * and the objects (should we, or should we group the pop later ?).
   * The code generates can be optimized with peep-hole style
   * techniques.
   */

  fts_bmax_code_atoms(f, obj->argc, obj->argv);
  fts_bmax_code_make_obj(f, obj->argc);

  /* PUT HERE THE CODE TO WRITE PROPERTIES !!! */

  fts_bmax_code_mv_obj(f, objidx);
  fts_bmax_code_pop_args(f, obj->argc);
  fts_bmax_code_pop_objs(f, 1);
}


static void
fts_bmax_code_new_connection(fts_bmax_file_t *f, fts_connection_t *conn, int fromidx)
{
  /* Push the inlet and outlet (this order) in the evaluation stack */

  fts_bmax_code_push_int(f, conn->winlet);
  fts_bmax_code_push_int(f, conn->woutlet);

  /* Push the to object, push the from object in the object stack */

  fts_bmax_code_push_obj(f, fromidx);
  fts_bmax_code_push_obj(f, fts_bmax_find_objidx(conn->dst));

  /* Pop 2 values from the evaluation stack */

  fts_bmax_code_pop_args(f, 2);

  /* Pop 2 object from the object stack */

  fts_bmax_code_pop_objs(f, 2);
}


void
fts_bmax_code_new_patcher(fts_bmax_file_t *f, fts_object_t *obj)
{
  int i;
  fts_patcher_t *patcher = (fts_patcher_t *) obj;
  fts_object_t *p;

  /* Allocate a new object table frame of the right dimension */

  fts_bmax_code_push_obj_table(f, fts_bmax_count_childs(patcher));

  /* Code all the objects */

  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      fts_bmax_code_new_object(f, p, i);
      i++;
    }

  /* For each object, for each outlet, code all the connections */

  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    fts_bmax_code_new_connection(f, c, i);
	}
    }

  /* Finally, pop the object table */

  fts_bmax_code_pop_obj_table(f);
}







