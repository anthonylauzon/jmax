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
 */

#ifndef _FTS_PRIVATE_BMAXFILE_H_
#define _FTS_PRIVATE_BMAXFILE_H_

/*
 * A FTS binary file is made of the following:
 *  . a header:
 *  . magic number: 'bMax'
 *  . code size
 *  . number of symbols
 * . code
 * . symbol table
 */

/*
 *
 * bmax file header
 *
 */
typedef struct fts_binary_file_header_t {
  unsigned long magic_number;
  unsigned long code_size;
  unsigned long n_symbols;
} fts_binary_file_header_t;

/* magic word is 'bMax' */
#define FTS_BINARY_FILE_MAGIC 0x624D6178

/*
 * Opcode of the bmax stack machine
 * Opcodes are built like this: 2 bits of argument size, and 6 of real opcode.
 * The two bits are zero in the case where they are not needed.
 */

#define FVM_N_ARG               0x00
#define FVM_B_ARG               0x40
#define FVM_S_ARG               0x80
#define FVM_L_ARG               0xc0
  
#define FVM_RETURN             0

#define FVM_PUSH_INT           1
#define FVM_PUSH_INT_B         (FVM_PUSH_INT | FVM_B_ARG)
#define FVM_PUSH_INT_S         (FVM_PUSH_INT | FVM_S_ARG)
#define FVM_PUSH_INT_L         (FVM_PUSH_INT | FVM_L_ARG)

#define FVM_PUSH_FLOAT         2
#define FVM_PUSH_SYM           3

#define FVM_PUSH_SYM_B         (FVM_PUSH_SYM | FVM_B_ARG)
#define FVM_PUSH_SYM_S         (FVM_PUSH_SYM | FVM_S_ARG)
#define FVM_PUSH_SYM_L         (FVM_PUSH_SYM | FVM_L_ARG)

#define FVM_PUSH_BUILTIN_SYM   4

#define FVM_SET_INT            5
#define FVM_SET_INT_B          (FVM_SET_INT | FVM_B_ARG)
#define FVM_SET_INT_S          (FVM_SET_INT | FVM_S_ARG)
#define FVM_SET_INT_L          (FVM_SET_INT | FVM_L_ARG)

#define FVM_SET_FLOAT          6
#define FVM_SET_SYM            7

#define FVM_SET_SYM_B          (FVM_SET_SYM | FVM_B_ARG)
#define FVM_SET_SYM_S          (FVM_SET_SYM | FVM_S_ARG)
#define FVM_SET_SYM_L          (FVM_SET_SYM | FVM_L_ARG)

#define FVM_SET_BUILTIN_SYM    8

#define FVM_POP_ARGS           9
#define FVM_POP_ARGS_B         (FVM_POP_ARGS | FVM_B_ARG)
#define FVM_POP_ARGS_S         (FVM_POP_ARGS | FVM_S_ARG)
#define FVM_POP_ARGS_L         (FVM_POP_ARGS | FVM_L_ARG)


#define FVM_PUSH_OBJ          10
#define FVM_PUSH_OBJ_B        (FVM_PUSH_OBJ | FVM_B_ARG)
#define FVM_PUSH_OBJ_S        (FVM_PUSH_OBJ | FVM_S_ARG)
#define FVM_PUSH_OBJ_L        (FVM_PUSH_OBJ | FVM_L_ARG)

#define FVM_MV_OBJ            11
#define FVM_MV_OBJ_B          (FVM_MV_OBJ | FVM_B_ARG)
#define FVM_MV_OBJ_S          (FVM_MV_OBJ | FVM_S_ARG)
#define FVM_MV_OBJ_L          (FVM_MV_OBJ | FVM_L_ARG)

#define FVM_POP_OBJS          12
#define FVM_POP_OBJS_B        (FVM_POP_OBJS | FVM_B_ARG)
#define FVM_POP_OBJS_S        (FVM_POP_OBJS | FVM_S_ARG)
#define FVM_POP_OBJS_L        (FVM_POP_OBJS | FVM_L_ARG)


#define FVM_MAKE_OBJ          13
#define FVM_MAKE_OBJ_B        (FVM_MAKE_OBJ | FVM_B_ARG)
#define FVM_MAKE_OBJ_S        (FVM_MAKE_OBJ | FVM_S_ARG)
#define FVM_MAKE_OBJ_L        (FVM_MAKE_OBJ | FVM_L_ARG)


#define FVM_PUT_PROP          14
#define FVM_PUT_PROP_B        (FVM_PUT_PROP | FVM_B_ARG)
#define FVM_PUT_PROP_S        (FVM_PUT_PROP | FVM_S_ARG)
#define FVM_PUT_PROP_L        (FVM_PUT_PROP | FVM_L_ARG)


#define FVM_PUT_BUILTIN_PROP  15
#define FVM_OBJ_MESS          16
#define FVM_OBJ_BUILTIN_MESS  17


#define FVM_PUSH_OBJ_TABLE    18
#define FVM_PUSH_OBJ_TABLE_B  (FVM_PUSH_OBJ_TABLE | FVM_B_ARG)
#define FVM_PUSH_OBJ_TABLE_S  (FVM_PUSH_OBJ_TABLE | FVM_S_ARG)
#define FVM_PUSH_OBJ_TABLE_L  (FVM_PUSH_OBJ_TABLE | FVM_L_ARG)


#define FVM_POP_OBJ_TABLE     19
#define FVM_CONNECT           20

#define FVM_MAKE_TOP_OBJ      21
#define FVM_MAKE_TOP_OBJ_B    (FVM_MAKE_TOP_OBJ | FVM_B_ARG)
#define FVM_MAKE_TOP_OBJ_S    (FVM_MAKE_TOP_OBJ | FVM_S_ARG)
#define FVM_MAKE_TOP_OBJ_L    (FVM_MAKE_TOP_OBJ | FVM_L_ARG)


/*
 *
 * bmax file loading
 *
 */

/* Loads a FTS binary file. Return value is < 0 if an error occured */
extern fts_object_t *fts_binary_file_load( const char *name, fts_object_t *parent, int ac, const fts_atom_t *at);

/* Idem but with a FILE * argument */
extern fts_object_t *fts_binary_filedesc_load( FILE *f, fts_object_t *parent, int ac, const fts_atom_t *at);

/*
 *
 * bmax file saving
 *
 */

typedef struct fts_bmax_file fts_bmax_file_t;
struct _saver_dumper;

struct fts_bmax_file {
  FILE *file;
  fts_binary_file_header_t header; 
  fts_symbol_t *symbol_table;
  unsigned int symbol_table_size;
  int symbol_table_static;
  struct _saver_dumper *dumper;
};

/* used by package class only!!! */
extern void fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at);
extern void fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
extern void fts_bmax_code_pop_args(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs);
extern void fts_bmax_code_return(fts_bmax_file_t *f);
extern void fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx);

extern int fts_bmax_file_open( fts_bmax_file_t *f, const char *name, int dobackup, fts_symbol_t *symbol_table, int symbol_table_size);
extern void fts_bmax_file_sync( fts_bmax_file_t *f);
extern void fts_bmax_file_close( fts_bmax_file_t *f);

extern void fts_save_patcher_as_bmax( fts_symbol_t file, fts_object_t *patcher);
extern void fts_save_selection_as_bmax( FILE *file, fts_object_t *selection);
extern void fts_save_simple_as_bmax( const char *filename, fts_object_t *patcher);


/*
 *
 * misc functions
 *
 */

/* bizarre function for objects redefinition...*/
extern void fts_vm_substitute_object(fts_object_t *old, fts_object_t *new);

#endif
