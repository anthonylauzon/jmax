/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

#include "lang/mess/saver.h"


/* #define SAVER_DEBUG  */

struct fts_bmax_file
{
  FILE *fd;
  fts_binary_file_header_t header; 
  fts_symbol_t *symbol_table;
  unsigned int symbol_table_size;
};


union swap_union_t {
  unsigned long l;
  char c[4];
};

static int has_to_swap(void)
{
  union swap_union_t u;

  u.l = 0x11223344;
  return u.c[0] != 0x11;
}

static void swap_long( unsigned long *p)
{
  union swap_union_t *pu;
  char tmp;

  pu = (union swap_union_t *)p;
  tmp = pu->c[0];
  pu->c[0] = pu->c[3];
  pu->c[3] = tmp;
  tmp = pu->c[1];
  pu->c[1] = pu->c[2];
  pu->c[2] = tmp;
}

#define SYMBOL_TABLE_SIZE 64


static fts_bmax_file_t *
fts_open_bmax_file_for_writing(fts_symbol_t file, int dobackup)
{
  fts_bmax_file_t *f;
  const char *name;
  fts_binary_file_header_t header;

  /* Get the file name */

  name = fts_symbol_name(file);

  if (dobackup)
    {
      char backup[PATH_MAX];

      /* Get the backup file name */

      sprintf(backup, "%s.backup", name);

      /* if the file exists, rename the old file, to keep a backup; ignore the error,
	 if we cannot autosave, we need anyway to try saving ! also, ignoring
	 the error catch the case where "name" do not exists.
	 */

      rename(name, backup);
    }

  /* allocate a bmax descriptor, and initialize it */

  f = (fts_bmax_file_t *) fts_malloc(sizeof(fts_bmax_file_t));
  f->symbol_table_size = SYMBOL_TABLE_SIZE;
  f->symbol_table = (fts_symbol_t *) fts_malloc(f->symbol_table_size * sizeof(fts_symbol_t));

  /* Initialize the header */

  f->header.magic_number = FTS_BINARY_FILE_MAGIC;
  f->header.code_size = 0;
  f->header.n_symbols = 0;

  /* Open the file */

  f->fd = fopen(name, "w");

  if (f->fd == 0)
    {
      post("Cannot save to: %s\n", name);
      return 0;
    }

  /* write the init header to the file */

  header = f->header;
  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if ( fwrite( &header, sizeof(fts_binary_file_header_t), 1, f->fd) < 1)
    {
      perror("fts_open_bmax_file_for_writing: write header ");
      return 0;
    }

  /* return the file */

  return f;
}


static void
fts_close_bmax_file(fts_bmax_file_t *f)
{
  unsigned int i;
  char c;
  fts_binary_file_header_t header;

  /* Write the symbol table */

#ifdef SAVER_DEBUG
  fprintf(stderr, "Writing symbol table [%d symbols]\n", f->header.n_symbols);
#endif

  for (i = 0; i < f->header.n_symbols; i++)
    {
#ifdef SAVER_DEBUG
      fprintf(stderr, "\t- %s\n", fts_symbol_name(f->symbol_table[i]));
#endif
      fwrite(fts_symbol_name(f->symbol_table[i]), sizeof(char),
	     strlen(fts_symbol_name(f->symbol_table[i]))+1, f->fd);
    }


  /* Write the ending zero */

  c = '\0';
  fwrite(&c, sizeof(char), 1, f->fd);

  /* seek to the beginning and rewrite the header */

#ifdef SAVER_DEBUG
  fprintf(stderr, "Writing header\n");
#endif

  fseek(f->fd, 0, SEEK_SET);

  header = f->header;
  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  fwrite( &header, sizeof(fts_binary_file_header_t), 1, f->fd);

  /* close the file */

#ifdef SAVER_DEBUG
  fprintf(stderr, "End of file\n");
#endif

  fclose(f->fd);

  /* free the bmax file descriptor */

  fts_free(f->symbol_table);
  fts_free(f);
}

/* Aux functions  for file building */

static int
fts_bmax_find_objidx_in_selection(fts_object_t *obj, fts_selection_t *sel)
{
  /* As object idx, i.e. the location where to store
     the object in the object frame, we compute the
     position of the object in the object list for the selection,
     without counting null pointers.
     */

  int i, idx;
  fts_object_t  *p;

  idx = 0;

  for (i = 0; i < sel->objects_size; i++)
    {
      if (obj == sel->objects[i])
	return idx;
      
      if (sel->objects[i])
	idx++;
    }

  /* If we exit from here, a big coding error have been done,
     or a message system inconsistency discovered !!! */

  return -1;
}


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
  unsigned int i;
  
  /* First, search for the symbol in the symbol table */

  for (i = 0; i < f->header.n_symbols; i++)
    if (f->symbol_table[i] == sym)
      return i;
  
  /* is not there, add it, resizing the table if there
   * is no place
   */

  if (f->header.n_symbols >= f->symbol_table_size)
    {
      /* resize symbol table  */

      f->symbol_table_size = (f->symbol_table_size * 3) / 2;
      f->symbol_table = fts_realloc((void *)f->symbol_table, f->symbol_table_size * sizeof(fts_symbol_t));
    }

  f->symbol_table[f->header.n_symbols] = sym;

  return f->header.n_symbols ++ ; /* !!! POST increment here !!! */
}
				 

/* Utilities */

static unsigned char fts_bmax_get_argcode(int value)
{
  if ((127 >= value) && (value >= -128))
    return FVM_B_ARG;
  else if ((32767 >= value) && (value >= -32768))
    return FVM_S_ARG;
  else
    return FVM_L_ARG;
}


static void fts_bmax_write_opcode_for(fts_bmax_file_t *f, unsigned char opcode, int value)
{
  unsigned char c = opcode | fts_bmax_get_argcode(value);

  fwrite(&c, sizeof(char), 1, f->fd);  
  f->header.code_size++;
}


static void fts_bmax_write_opcode(fts_bmax_file_t *f, unsigned char opcode)
{
  unsigned char c = opcode;
  
  fwrite(&c, sizeof(char), 1, f->fd);  
  f->header.code_size++;
}



static void fts_bmax_write_b_int(fts_bmax_file_t *f, int value)
{
  unsigned char c;

  c = (char) ((unsigned int) (value & 0x000000ff));
  fwrite(&c, sizeof(char), 1, f->fd);  
  f->header.code_size++;
}


static void fts_bmax_write_s_int(fts_bmax_file_t *f, int value)
{
  unsigned char c[2];
      
  c[1] = (unsigned char) ((unsigned int) value & 0x000000ff);
  c[0] = (unsigned char) (((unsigned int) value & 0x0000ff00) >> 8);

  fwrite(c, sizeof(char), 2, f->fd);        
  f->header.code_size += 2;
}

static void fts_bmax_write_l_int(fts_bmax_file_t *f, int value)
{
  unsigned char c[4];
      
  c[3] = (unsigned char) (((unsigned int) value & 0x000000ff) >> 0);
  c[2] = (unsigned char) (((unsigned int) value & 0x0000ff00) >> 8);
  c[1] = (unsigned char) (((unsigned int) value & 0x00ff0000) >> 16);
  c[0] = (unsigned char) (((unsigned int) value & 0xff000000) >> 24);

  fwrite(c, sizeof(char), 4, f->fd);        
  f->header.code_size += 4;
}

static void fts_bmax_write_int(fts_bmax_file_t *f, int value)
{
  unsigned char argcode  = fts_bmax_get_argcode(value);

  if (argcode == FVM_B_ARG)
    fts_bmax_write_b_int(f, value);
  else if (argcode == FVM_S_ARG)
    fts_bmax_write_s_int(f, value);
  else if (argcode == FVM_L_ARG)
    fts_bmax_write_l_int(f, value);
}

static void fts_bmax_write_float(fts_bmax_file_t *f, float value)
{
  float fv = value;
  unsigned int fx = *((unsigned int *)&fv);
  unsigned char c[4];
      
  c[3] = (unsigned char) (((unsigned int) fx & 0x000000ff) >> 0);
  c[2] = (unsigned char) (((unsigned int) fx & 0x0000ff00) >> 8);
  c[1] = (unsigned char) (((unsigned int) fx & 0x00ff0000) >> 16);
  c[0] = (unsigned char) (((unsigned int) fx & 0xff000000) >> 24);

  fwrite(c, sizeof(char), 4, f->fd);        
  f->header.code_size += 4;
}


/* One functions for each opcode (without considering the argument length) */

static void fts_bmax_code_return(fts_bmax_file_t *f)
{
  /* RETURN */

  fts_word_t w;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tRETURN\n");
#endif

  fts_bmax_write_opcode(f, FVM_RETURN);
}


void fts_bmax_code_push_int(fts_bmax_file_t *f, int value)
{
  /* PUSH_INT   <int>   */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUSH_INT %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_PUSH_INT, value);
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_push_float(fts_bmax_file_t *f, float value)
{
  /* PUSH_FLOAT <float> */

  fts_word_t w;
  fts_word_t fw;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUSH_FLOAT %f\n", value);
#endif

  fts_bmax_write_opcode(f, FVM_PUSH_FLOAT);
  fts_bmax_write_float(f, value);
}


void fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* PUSH_SYM   <int>   */

  int value;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUSH_SYM %d (%s)\n",
	  fts_bmax_add_symbol(f, sym),
	  fts_symbol_name(sym));
#endif
  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_PUSH_SYM, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_set_int(fts_bmax_file_t *f, int value)
{
  /* SET_INT   <int>   */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tSET_INT %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_SET_INT, value);
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_set_float(fts_bmax_file_t *f, float value)
{
  /* SET_FLOAT <float> */

  fts_word_t w;
  fts_word_t fw;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tSET_FLOAT %f\n", value);
#endif

  fts_bmax_write_opcode(f, FVM_SET_FLOAT);
  fts_bmax_write_float(f, value);
}


void fts_bmax_code_set_symbol(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* SET_SYM   <int>   */

  int value;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tSET_SYM %d (%s)\n",
	  fts_bmax_add_symbol(f, sym),
	  fts_symbol_name(sym));
#endif
  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_SET_SYM, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_pop_args(fts_bmax_file_t *f, int value)
{
  /* POP_ARGS    <int>   // pop n values  from the argument stack */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPOP_ARGS %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_POP_ARGS, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_push_obj(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ   <objidx> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUSH_OBJ %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_PUSH_OBJ, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_mv_obj(fts_bmax_file_t *f, int value)
{
  /* MV_OBJ     <objidx> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tMV_OBJ %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_MV_OBJ, value );
  fts_bmax_write_int(f, value);
}



void fts_bmax_code_pop_objs(fts_bmax_file_t *f, int value)
{
  /* POP_OBJS    <int> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPOP_OBJS %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_POP_OBJS, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_make_obj(fts_bmax_file_t *f, int value)
{
  /* MAKE_OBJ   <nargs> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tMAKE_OBJ %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_MAKE_OBJ, value );
  fts_bmax_write_int(f, value);
}

static void fts_bmax_code_make_top_obj(fts_bmax_file_t *f, int value)
{
  /* MAKE_TOP_OBJ   <nargs> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tMAKE_TOP_OBJ %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_MAKE_TOP_OBJ, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_put_prop(fts_bmax_file_t *f, fts_symbol_t sym)
{
  /* PUT_PROP   <sym> */

  int value;

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUT_PROP %d (%s)\n",
	  fts_bmax_add_symbol(f, sym),
	  fts_symbol_name(sym));
#endif

  value = fts_bmax_add_symbol(f, sym);

  fts_bmax_write_opcode_for(f, FVM_PUT_PROP, value );
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs)
{
  /* OBJ_MESS   <inlet> <sel> <nargs> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tOBJ_MESS %d %d (%s) %d\n",
	  inlet,
	  fts_bmax_add_symbol(f, sel),
	  fts_symbol_name(sel),
	  nargs);
#endif

  fts_bmax_write_opcode(f, FVM_OBJ_MESS);
  fts_bmax_write_l_int(f, inlet);
  fts_bmax_write_l_int(f, fts_bmax_add_symbol(f, sel));
  fts_bmax_write_l_int(f, nargs);
}


void fts_bmax_code_push_obj_table(fts_bmax_file_t *f, int value)
{
  /* PUSH_OBJ_TABLE <int> */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPUSH_OBJ_TABLE %d\n", value);
#endif

  fts_bmax_write_opcode_for(f, FVM_PUSH_OBJ_TABLE, value);
  fts_bmax_write_int(f, value);
}


void fts_bmax_code_pop_obj_table(fts_bmax_file_t *f)
{
  /* POP_OBJ_TABLE */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tPOP_OBJ_TABLE\n");
#endif

  fts_bmax_write_opcode(f, FVM_POP_OBJ_TABLE);
}

void fts_bmax_code_connect(fts_bmax_file_t *f)
{
  /* CONNECT */

#ifdef SAVER_DEBUG
  fprintf(stderr, "\tCONNECT\n");
#endif

  fts_bmax_write_opcode(f, FVM_CONNECT);
}



/*
 * Higher level (Macro) functions, convenience functions.
 * 
 */


/* push atom args: pushed backward, to have them in the right order
 * in the stack at execution time
 */


void fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at)
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


/* set atom arg: set an atom as top of the stack
 */


void fts_bmax_code_set_atom(fts_bmax_file_t *f, const fts_atom_t *a)
{
  if (fts_is_int(a))
    fts_bmax_code_set_int(f, fts_get_int(a));
  else if (fts_is_float(a))
    fts_bmax_code_set_float(f, fts_get_float(a));
  else if (fts_is_symbol(a))
    fts_bmax_code_set_symbol(f, fts_get_symbol(a));
}

/* Objects, connections, patchers */

/* Code a new property: push the property value, code the put
   instruction, pop the value.

   The object passed must be on the top of the object stack.
   */


static void fts_bmax_code_new_property(fts_bmax_file_t *f, fts_object_t *obj, fts_symbol_t prop)
{
  fts_atom_t value;
  
  fts_object_get_prop(obj, prop, &value);

  if (! fts_is_void(&value))
    {
      fts_bmax_code_set_atom(f, &value);
      fts_bmax_code_put_prop(f, prop);
    }
}


/* Code a new object, and leave him in the top of the object stack */

static void
fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx)
{
  fts_atom_t a;

  /* Push the object arguments, make the object, and put it in the object table, then push the args 
   * and the objects (should we, or should we group the pop later ?).
   * 
   * The pop  of the arguments is done at the end, because we reuse
   * the top of the stack for properties (use set instead of push)
   */

#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Object %lx %d: ", obj,  obj->id);
  fprintf_atoms(stderr, obj->argc, obj->argv);
  fprintf(stderr, "\n");
#endif

  fts_bmax_code_push_atoms(f, obj->argc, obj->argv);

  fts_bmax_code_make_obj(f, obj->argc);

  if (objidx >= 0)
    fts_bmax_code_mv_obj(f, objidx);

  /* If the argc is zero, we push a zero value to the value
     stack, in order to use "set" and not "push/pop" in the
     property coding; in this case, we don't care about optimizing code size:
     an object with zero argument is an empty "error" object, left there by mistake
     during editing.
     */

  if (obj->argc == 0)
    fts_bmax_code_push_int(f, 0);

  /* Write persistent properties to the file.
     Here, it should use some property data base to find out
     the good properties.
     */

  fts_bmax_code_new_property(f, obj, fts_s_x);
  fts_bmax_code_new_property(f, obj, fts_s_y);
  fts_bmax_code_new_property(f, obj, fts_s_height);
  fts_bmax_code_new_property(f, obj, fts_s_width);

  fts_bmax_code_new_property(f, obj, fts_s_font);
  fts_bmax_code_new_property(f, obj, fts_s_fontSize);

  fts_bmax_code_new_property(f, obj, fts_s_min_value);
  fts_bmax_code_new_property(f, obj, fts_s_max_value);

  fts_bmax_code_new_property(f, obj, fts_s_comment);
  fts_bmax_code_new_property(f, obj, fts_s_layer);

  if (fts_object_is_patcher(obj))
    {
      fts_bmax_code_new_property(f, obj, fts_s_wx);
      fts_bmax_code_new_property(f, obj, fts_s_wy);
      fts_bmax_code_new_property(f, obj, fts_s_wh);
      fts_bmax_code_new_property(f, obj, fts_s_ww);
      fts_bmax_code_new_property(f, obj, fts_s_ninlets);
      fts_bmax_code_new_property(f, obj, fts_s_noutlets);
    }

  /* If argc is zero, we pop the 0 value pushed above */

  if  (obj->argc == 0)
    fts_bmax_code_pop_args(f, 1);
  else
    fts_bmax_code_pop_args(f, obj->argc);

  /* Here, send a message to the object to give it the opportunity
     to save its data, if any; ignore error return ! */

  fts_set_ptr(&a, f);

  fts_send_message(obj, fts_SystemInlet, fts_s_save_bmax, 1, &a);
}


/*
  Code the top level patcher object instantiation and properties 
  */
   
static void
fts_bmax_code_new_top_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx)
{
  fts_atom_t a;

  /* Push the object arguments, make the object, and put it in the object table, then push the args 
   * and the objects (should we, or should we group the pop later ?).
   * 
   * The pop  of the arguments is done at the end, because we reuse
   * the top of the stack for properties (use set instead of push)
   */

#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Top Object %lx %d: ", obj,  obj->id);
  fprintf_atoms(stderr, obj->argc, obj->argv);
  fprintf(stderr, "\n");
#endif

  fts_set_symbol(&a, fts_s_patcher);
  fts_bmax_code_push_atoms(f, 1, &a);

  fts_bmax_code_make_top_obj(f, 1);

  if (objidx >= 0)
    fts_bmax_code_mv_obj(f, objidx);

  /* Write persistent properties to the file.
     Here, it should use some property data base to find out
     the good properties.
     */

  fts_bmax_code_new_property(f, obj, fts_s_x);
  fts_bmax_code_new_property(f, obj, fts_s_y);
  fts_bmax_code_new_property(f, obj, fts_s_height);
  fts_bmax_code_new_property(f, obj, fts_s_width);

  fts_bmax_code_new_property(f, obj, fts_s_font);
  fts_bmax_code_new_property(f, obj, fts_s_fontSize);

  fts_bmax_code_new_property(f, obj, fts_s_wx);
  fts_bmax_code_new_property(f, obj, fts_s_wy);
  fts_bmax_code_new_property(f, obj, fts_s_wh);
  fts_bmax_code_new_property(f, obj, fts_s_ww);
  fts_bmax_code_new_property(f, obj, fts_s_ninlets);
  fts_bmax_code_new_property(f, obj, fts_s_noutlets);

  fts_bmax_code_new_property(f, obj, fts_s_comment);
  fts_bmax_code_new_property(f, obj, fts_s_layer);

  /* pop the argument */

  fts_bmax_code_pop_args(f, 1);
}


static void
fts_bmax_code_new_connection(fts_bmax_file_t *f, fts_connection_t *conn, int fromidx)
{
#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Connection (%d.%d -> %d.%d)\n",
	  conn->src->id, conn->woutlet, conn->dst->id, conn->winlet);
#endif


  /* Push the inlet and outlet (this order) in the evaluation stack */

  fts_bmax_code_push_int(f, conn->winlet);
  fts_bmax_code_push_int(f, conn->woutlet);

  /* Push the to object, push the from object in the object stack */

  fts_bmax_code_push_obj(f, fts_bmax_find_objidx(conn->dst));
  fts_bmax_code_push_obj(f, fromidx);

  /* code the connect command */

  fts_bmax_code_connect(f);

  /* Pop 2 values from the evaluation stack */

  fts_bmax_code_pop_args(f, 2);

  /* Pop 2 object from the object stack */

  fts_bmax_code_pop_objs(f, 2);
}


static void
fts_bmax_code_new_connection_in_selection(fts_bmax_file_t *f, fts_connection_t *conn, fts_selection_t *sel)
{
#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Connection in selection(%d.%d -> %d.%d)\n",
	  conn->src->id, conn->woutlet, conn->dst->id, conn->winlet);
#endif

  /* Push the inlet and outlet (this order) in the evaluation stack */

  fts_bmax_code_push_int(f, conn->winlet);
  fts_bmax_code_push_int(f, conn->woutlet);

  /* Push the to object, push the from object in the object stack */

  fts_bmax_code_push_obj(f, fts_bmax_find_objidx_in_selection(conn->dst, sel));
  fts_bmax_code_push_obj(f, fts_bmax_find_objidx_in_selection(conn->src, sel));

  /* code the connect command */

  fts_bmax_code_connect(f);

  /* Pop 2 values from the evaluation stack */

  fts_bmax_code_pop_args(f, 2);

  /* Pop 2 object from the object stack */

  fts_bmax_code_pop_objs(f, 2);
}


/* Code a new patcher, and leave it in the top of the stack !!! */

static void
fts_bmax_code_new_patcher(fts_bmax_file_t *f, fts_object_t *obj, int idx, int top)
{
  int i;
  fts_patcher_t *patcher = (fts_patcher_t *) obj;
  fts_object_t *p;

#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Patcher %d\n", obj->id);
#endif

  /* First generate the code to push the patcher in the top of the stack,
     asking for the top_level special code (template argument fetch and
     patcher object instead of 
     (no effect for not a template or no args).
     */

  if (top)
    fts_bmax_code_new_top_object(f, obj, idx);
  else
    fts_bmax_code_new_object(f, obj, idx);
    
  /* Allocate a new object table frame of the right dimension */

  fts_bmax_code_push_obj_table(f, fts_bmax_count_childs(patcher));

  /* Code all the objects */

  i = 0;
  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_patcher(p) &&
	  (! fts_object_is_abstraction(p)) &&
	  (! fts_object_is_template(p)))
	{
	  /* Save the object recursively as a patcher, and then pop it from the stack */


	  fts_bmax_code_new_patcher(f, p, i, 0);
	  fts_bmax_code_pop_objs(f, 1);
	}
      else
	{
	  /* Code a new object and pop it from the object stack */
	  fts_bmax_code_new_object(f, p, i);
	  fts_bmax_code_pop_objs(f, 1);
	}

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

      i++;
    }

  /* Finally, pop the object table */

  fts_bmax_code_pop_obj_table(f);
}



void fts_save_patcher_as_bmax(fts_symbol_t file, fts_object_t *patcher)
{
  fts_bmax_file_t *f;

  f = fts_open_bmax_file_for_writing(file, 1);

  if (f)
    {
      fts_bmax_code_new_patcher(f, patcher, -1, 1);

      /* code the return command */

      fts_bmax_code_return(f);
      fts_close_bmax_file(f);

      /* Recompute the template instances if needed */

      fts_template_file_modified(file);
    }
}


static void
fts_bmax_code_new_selection(fts_bmax_file_t *f, fts_object_t *obj)
{
  int i, objidx;
  fts_selection_t *selection = (fts_selection_t *) obj;

#ifdef SAVER_DEBUG
  fprintf(stderr, "Saving Selection %d\n", obj->id);
#endif

  /* Allocate a new object table frame of the right dimension */

  fts_bmax_code_push_obj_table(f, selection->objects_count);

  /* Code all the objects */

  objidx = 0;
  for (i = 0; i < selection->objects_size; i++)
    {
      if (selection->objects[i])
	{
	  fts_object_t *p;

	  p = selection->objects[i];

	  if (fts_object_is_patcher(p) && fts_patcher_is_standard((fts_patcher_t *) p))
	    {
	      /* Save the object recursively as a patcher, and then pop it from the stack */
	      
	      fts_bmax_code_new_patcher(f, p, objidx, 0);
	      fts_bmax_code_pop_objs(f, 1);
	    }
	  else
	    {
	      /* Code a new object and pop it from the object stack */

	      fts_bmax_code_new_object(f, p, objidx);
	      fts_bmax_code_pop_objs(f, 1);
	    }

	  objidx++;
	}
    }

  /* For each object, for each outlet, code all the connections */

  for (i = 0; i < selection->connections_size; i++)
    {
      if (selection->connections[i])
	{
	  fts_connection_t *c;

	  c = selection->connections[i];

	  fts_bmax_code_new_connection_in_selection(f, c, selection);
	}
    }

  /* Finally, pop the object table */

  fts_bmax_code_pop_obj_table(f);
}

void fts_save_selection_as_bmax(fts_symbol_t file, fts_object_t *selection)
{
  fts_bmax_file_t *f;

  f = fts_open_bmax_file_for_writing(file, 0);

  if (f)
    {
      fts_bmax_code_new_selection(f, selection);

      /* code the return command */

      fts_bmax_code_return(f);
      fts_close_bmax_file(f);
    }
}






