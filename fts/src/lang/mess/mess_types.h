/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.13 $ IRCAM $Date: 1998/06/17 15:42:47 $
 *
 *  Eric Viara for Ircam, January 1995
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

typedef enum fts_daemon_action {
  obj_property_put, obj_property_get, obj_property_remove, 
  inlet_property_put, inlet_property_get, inlet_property_remove, 
  outlet_property_put, outlet_property_get, outlet_property_remove
} fts_daemon_action_t;

typedef struct fts_metaclass		 fts_metaclass_t;
typedef struct fts_class		 fts_class_t;
typedef struct fts_object		 fts_object_t;

typedef struct fts_mess_type		 fts_mess_type_t;
typedef struct fts_class_mess		 fts_class_mess_t;
typedef struct fts_connection		 fts_connection_t;
typedef struct fts_inlet_decl		 fts_inlet_decl_t;
typedef struct fts_outlet_decl		 fts_outlet_decl_t;
typedef struct fts_patcher		 fts_patcher_t;
typedef struct fts_inlet		 fts_inlet_t;
typedef struct fts_outlet		 fts_outlet_t;

typedef struct fts_selection		 fts_selection_t;
typedef struct fts_variable		 fts_variable_t;


/*
 *  SYMBOLS
 *
 * symbol are a unique representation for read-only strings (in LISP style);
 * symbol content comparison can be done by pointer comparison.
 */


struct fts_symbol_descr
{
  const char *name;		/* name */
  int index;			/* index in the builtin table, -1 otherwise */
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
  long                fts_long;
  unsigned long       fts_ulong;
  float               fts_float;
  fts_symbol_t        fts_symbol;
  void                *fts_ptr;		/*  just a pointer somewhere */
  char                *fts_str;
  fts_object_t        *fts_obj;
  fts_connection_t    *fts_connection;
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
 *  PROPERTY LISTS
 *
 * Used to store properties.
 */

struct fts_plist {
  struct fts_plist_cell *head;
};


/*
 * Universal daemon type;
 *
 * Depending on the type of action, the idx value may be not significative,
 * or may represent the inlet or outlet for the property.
 *
 * For get actions, the value argument is the pointer where the value is returned.
 * For put actions, the value argument is the pointer where the value is passed.
 * For remove actions, the value argument is ignored.
 */


typedef void (* fts_property_daemon_t)(fts_daemon_action_t action, fts_object_t *obj, int idx, fts_symbol_t property, fts_atom_t *value);


/* generic class/object types */

typedef fts_status_t (*fts_method_instantiate_t)(fts_class_t *, int, const fts_atom_t *);
typedef int (*fts_method_equiv_t)(int, const fts_atom_t *, int, const fts_atom_t *);
typedef void (*fts_method_t) (fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);


struct fts_metaclass
{
  fts_method_instantiate_t mth_instantiate;

  /* Instance data base */

  fts_method_equiv_t mth_equiv;
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
  fts_plist_t *properties;		/* class inlet dynamic properties */

  int nmess;
  int nalloc;
  fts_class_mess_t **messlist;
};


struct fts_outlet_decl
{
  fts_plist_t *properties;		/* class outlet dynamic properties */

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
     this is the variable (and object) name */

  fts_symbol_t varname;

  /* connections */

  fts_connection_t **in_conn;
  fts_connection_t **out_conn;

  /* object dynamic properties */

  fts_plist_t *properties;	
  fts_plist_t **inlets_properties;	
  fts_plist_t **outlets_properties;	
};

/* Variable structure */

struct fts_variable
{
  fts_symbol_t  name;
  fts_atom_t    value;
  fts_object_t  *owner;		/* the object that defined this variable */
  fts_variable_t *next;
};

/* Patcher structure */

struct fts_patcher
{
  fts_object_t o;

  fts_symbol_t   name;		/* the patcher name */
  fts_inlet_t  **inlets;	/* the patcher inlet array */
  fts_outlet_t **outlets;	/* the patcher outlet array */

  fts_object_t *objects;	/* the patcher content, organized as a list */

  int open;			/* the open flag */
  int load_init_fired;		/* the multiple load init protection flag*/

  enum {fts_p_standard, fts_p_abstraction, fts_p_error, fts_p_template} type;

  /* Variables */

  fts_variable_t *env;
};    


struct fts_inlet
{
  fts_object_t o;

  int          position;	/* inlet position */
};

   
struct fts_outlet
{
  fts_object_t o;		

  int          position;	/* outlet position */
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
