/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#ifndef _MESS_TYPES_H_
#define _MESS_TYPES_H_

/******************************************************************************/
/*                                                                            */
/*                            TYPEDEFS                                        */
/*                                                                            */
/******************************************************************************/

/* Forward declaration of types: all  the typedefs are here to simplify
   circular type definitions.
 */

typedef const struct fts_symbol_descr *fts_symbol_t;
typedef union  fts_word          fts_word_t;
typedef struct fts_atom          fts_atom_t;
typedef struct fts_plist                 fts_plist_t;

/* (fd) 
   For now, fts_type_t is an alias of fts_symbol_t, in order
   to be introduced in the message system API, but not force changing
   the message system implementation and all the objects.
   This will be changed soon to a true typing system (? ;-{)
*/
#define fts_type_t fts_symbol_t
#define fts_type_get_selector(T) (T)

typedef enum fts_daemon_action {
  obj_property_put, obj_property_get, obj_property_remove
} fts_daemon_action_t;


typedef struct fts_metaclass    fts_metaclass_t;
typedef struct fts_class        fts_class_t;
typedef struct fts_object       fts_object_t;


typedef struct fts_mess_type    fts_mess_type_t;
typedef struct fts_class_mess   fts_class_mess_t;
typedef struct fts_connection   fts_connection_t;
typedef struct fts_inlet_decl   fts_inlet_decl_t;
typedef struct fts_outlet_decl  fts_outlet_decl_t;
typedef struct fts_patcher_data fts_patcher_data_t;
typedef struct fts_patcher      fts_patcher_t;
typedef struct fts_inlet        fts_inlet_t;
typedef struct fts_outlet       fts_outlet_t;

typedef struct fts_selection    fts_selection_t;

typedef struct fts_binding_list fts_binding_list_t;
typedef struct fts_object_list  fts_object_list_t;
typedef struct fts_binding	fts_binding_t;
typedef struct fts_env		fts_env_t;

typedef struct fts_data			fts_data_t;
typedef struct fts_data_class		fts_data_class_t;
typedef struct fts_template		fts_template_t;

/*
 *  SYMBOLS
 *
 * symbol are a unique representation for read-only strings (in LISP style);
 * symbol content comparison can be done by pointer comparison.
 */


struct fts_symbol_descr
{
  const char *name;		/* name */
  int cache_index;		/* index in the client cache if any */
  int operator;			/* index in the operator table, for the expression eval */
  struct fts_symbol_descr *next_in_table; /* next in hash table for fts_new_symbol */
};



/*
 * WORDS
 *
 * An fts_word_t is an union of the different basic types used in FTS.
 * See atoms.h for word related macros.
 */


union fts_word
{
  int                 fts_int;
  float               fts_float;
  fts_symbol_t        fts_symbol;
  char                *fts_str;
  fts_object_t        *fts_obj;
  fts_connection_t    *fts_connection;
  fts_data_t          *fts_data;
  void                *fts_ptr;		/*  just a pointer somewhere */
  void                (*fts_fun)(void);     /*  just a pointer to some function */
};

/*
 * ATOMS
 *
 * Atoms are type-tagged words; i.e. an atom include a type tag,
 * and a fts_word_t value.
 * They are used everywhere in FTS as self described datums, like
 * in object messages and so on.
 *
 * The types tag is a symbols; to each basic type correspond 
 * a symbol, but above levels are free to define their types.
 * See atoms.h for atom related macros, and symbols.h for predefined 
 * symbols
 */


struct fts_atom
{	 
  fts_symbol_t type;		/* from the above defs, or others */
  fts_word_t value;
};

/* 
   An atom array is an array of atoms of fixed length.
   For now, used as value in expression, may become more
   widely used later.
   */

/*
 *  PROPERTY LISTS
 *
 * Used to store properties.
 */

struct fts_plist
{
  struct fts_plist_cell *head;
};


/*
 * Data functions
 */

typedef void (*fts_data_fun_t)(fts_data_t *d, int ac, const fts_atom_t *at);
typedef void (*fts_data_export_fun_t)(fts_data_t *d);
typedef fts_data_t *(*fts_data_remote_constructor_t)(int ac, const fts_atom_t *at);
typedef void (*fts_data_remote_destructor_t)(fts_data_t *d);


/*
 * Universal daemon type;
 *
 * For get actions, the value argument is the pointer where the value is returned.
 * For put actions, the value argument is the pointer where the value is passed.
 * For remove actions, the value argument is ignored.
 */


typedef void (* fts_property_daemon_t)(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);


/* generic class/object types */

typedef fts_status_t (*fts_instantiate_fun_t)(fts_class_t *, int, const fts_atom_t *);
typedef int (*fts_equiv_fun_t)(int, const fts_atom_t *, int, const fts_atom_t *);
typedef void (*fts_method_t) (fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);


struct fts_metaclass
{
  fts_instantiate_fun_t instantiate_fun;

  /* Instance data base */

  fts_equiv_fun_t equiv_fun;
  fts_class_t *inst_list;

  fts_symbol_t name;		/* the name of the metaclass, i.e. the first name used to register it */
};

struct fts_class
{
  /* Object management */

  fts_metaclass_t *mcl;
  fts_inlet_decl_t *sysinlet;

  int ninlets;
  fts_inlet_decl_t *inlets;

  int noutlets;
  fts_outlet_decl_t *outlets;

  unsigned int size;

  /* Class Instance Data Base support */

  int ac;
  const fts_atom_t *at;
  fts_class_t *next;

  /* property list handling */

  fts_plist_t *properties;		/* class dynamic properties */

  struct daemon_list *daemons;

  /* User data */

  void *user_data;
};


struct fts_mess_type
{
  fts_symbol_t symb;

  int mandatory_args;
  int nargs;
  fts_symbol_t *arg_types;	/* now atom are symbols */
};


struct fts_class_mess
{
  fts_mess_type_t tmess;
  fts_method_t mth;
};


struct fts_connection
{
  fts_object_t *src;
  int woutlet;

  fts_object_t *dst;
  int winlet;

  int id;                        /* the connection ID, when defined */
 
  fts_symbol_t symb;		/* the message cache: the symbol: if null, means anything ! */
  fts_method_t  mth;		/* the message  cache: the method */

  fts_connection_t *next_same_dst;
  fts_connection_t *next_same_src;
};


struct fts_inlet_decl
{
  unsigned int nmess;
  unsigned int nalloc;
  fts_class_mess_t **messlist;
};


struct fts_outlet_decl
{
  fts_mess_type_t tmess;	
};


struct fts_object
{
  /* Class pointer  */

  fts_class_t *cl;

  /* the object description */

  int argc;
  fts_atom_t *argv;

  /* patcher housekeeping */

  fts_patcher_t *patcher;
  fts_object_t  *next_in_patcher;

  /* ID housekeeping */

  int id;		/*  id for the object */

  /* Variable: If this object is bound to a variable,
     this is the variable  name */

  fts_symbol_t varname;

  /* Variables referred by the object */

  fts_binding_list_t *var_refs; /* handled completely in the variable.c file */

  /* connections */

  fts_connection_t **in_conn;
  fts_connection_t **out_conn;

  /* object dynamic properties */

  fts_plist_t *properties;	
};

/* Commodity structure to keep a list of objects */

struct fts_object_list
{
  fts_object_t *obj;

  struct fts_object_list *next;
};


/* Commodity structure to keep a list of bindings */

struct fts_binding_list
{
  fts_binding_t *var;

  struct fts_binding_list *next;
};


/* Variable structure */

struct fts_binding
{
  fts_symbol_t   name;
  int            suspended;
  fts_atom_t     value;
  fts_object_list_t *users;	/* object that use this variables */
  fts_object_list_t *definitions;/* object that want to redefine this variables locally, if any*/
  struct fts_env  *env;		/* back pointer to the environment where the variable is stored */
  fts_binding_t *next;		/* next in the environent */
};


/* Variable environment */

struct fts_env
{
  fts_binding_t *first;
  fts_object_t *patcher;
};

/* Patcher structure */

struct fts_patcher
{
  fts_object_t o;

  fts_inlet_t  **inlets;	/* the patcher inlet array */
  fts_outlet_t **outlets;	/* the patcher outlet array */
  fts_object_t *objects;	/* the patcher content, organized as a list */

  int open;			/* the open flag */
  int load_init_fired;		/* the multiple load init protection flag*/
  int deleted;			/* set to one during content deleting */

  fts_data_t *args;	/* the arguments used for the "args" variable */

  enum {fts_p_standard, fts_p_abstraction, fts_p_error, fts_p_template} type;

  /* If this patcher is a template, point to the template definition */

  fts_template_t *template;

  /* The pointer to the patcher data  */

  fts_patcher_data_t *data;

  /* Variables */

  fts_env_t env;
};    


struct fts_inlet
{
  fts_object_t o;

  int          position;	/* inlet position */
  struct fts_inlet *next;	/* next inlet in the same position */
};

   
struct fts_outlet
{
  fts_object_t o;		

  int          position;	/* outlet position */
  struct fts_outlet *next;	/* next outlet in the same position */
};


/* Selection */

struct fts_selection
{
  fts_object_t ob;

  fts_object_t **objects;
  int objects_size;
  int objects_count;

  fts_connection_t **connections;
  int connections_size;
  int connections_count;

};

#endif
