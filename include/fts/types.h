
#ifndef _FTS_TYPES_H_
#define _FTS_TYPES_H_

typedef const struct fts_symbol_descr *fts_symbol_t;

#define fts_type_t fts_symbol_t
#define fts_type_get_selector(T) (T)
#define fts_type_get_by_name(n) (n)

typedef void (*fts_fun_t)(void);
typedef struct fts_object fts_object_t;
typedef struct fts_connection fts_connection_t;
typedef struct fts_data fts_data_t;

typedef struct fts_metaclass fts_metaclass_t;
typedef struct fts_class fts_class_t;
typedef struct fts_patcher fts_patcher_t;

/* the following lines should not be here.... */
typedef struct fts_inlet fts_inlet_t;
typedef struct fts_outlet fts_outlet_t;
typedef struct fts_template fts_template_t;
typedef struct fts_abstraction fts_abstraction_t;
typedef struct fts_patcher_data fts_patcher_data_t;

typedef union  fts_word fts_word_t;
typedef struct fts_atom fts_atom_t;

typedef struct _fts_status_description_t fts_status_description_t;
typedef fts_status_description_t *fts_status_t;

/*
 * WORDS
 *
 * An fts_word_t is an union of the different basic types used in FTS.
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
 * to a symbol predefined in symbol.h; basic types are supported by
 * means of a group of dedicated get/set macros, but the atoms are not limited
 * to basic types; the symbol can be set freely; the C type of the value part
 * should be considered as the representational type; it make sense for example
 * to have a type like "socket" that use for its representation the int C type.
 *
 * The special  symbol fts_s_void means a void value.
 * 
 * The special symbol fts_s_error means an error value, i.e. a value
 * that should generate an error each time somebody try to use it.
 *
 */

struct fts_atom
{	 
  fts_symbol_t type;		/* from the above defs, or others */
  fts_word_t value;
};



typedef void (*fts_method_t) (fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);


#endif
