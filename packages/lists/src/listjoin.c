#include "fts.h"

typedef struct
{
  fts_atom_t *at;
  int ac;
  int alloc;
} list_t;
  
typedef struct 
{
  fts_object_t o;
  list_t list;
  list_t store;
} listjoin_t;

#define LIST_ALLOC_BLOC 32

/*********************************************
 *
 *  utils
 *
 */

static list_t*
list_realloc_empty(list_t *list, int ac)
{
  int alloc = list->alloc;

  if(list->at) 
    fts_free(list->at);

  while(alloc < ac)
    alloc += LIST_ALLOC_BLOCK;

  list->at = (fts_atom_t *) fts_malloc(alloc * sizeof(fts_atom_t));
  list->ac = ac;
  list->alloc = alloc;

  return list;
}


/*********************************************
 *
 *  methods
 *
 */

static void
listjoin_store_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int i;

  if(this->store.alloc < ac)
    list_realloc_empty(&(this->store), ac);
  else
    this->store.ac = ac;

  memcpy((char *)this->store.at, at, ac * sizeof(fts_atom_t)); /* copy incomming list to store */
}

static void
listjoin_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int total;

  total = this->store.ac + ac;

  if(this->list.alloc < total)
    list_realloc_empty(&(this->list), total);
  else
    this->list.ac = total;

  /* copy incomming list */
  memcpy((char *)this->list.at, at, ac * sizeof(fts_atom_t)); 
  /* append stored list */
  memcpy((char *)(this->list.at + ac), this->store.at, this->store.ac * sizeof(fts_atom_t)); 

  fts_outlet_send(o, 0, fts_s_list, this->list.ac, this->list.at);
}


static void
listjoin_prepend(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int total;

  total = this->store.ac + ac;

  if(this->list.alloc < total)
    list_realloc_empty(&(this->list), total);
  else
    this->list.ac = total;

  /* copy stored list */
  memcpy((char *)this->list.at, this->store.at, this->store.ac * sizeof(fts_atom_t)); 
  /* append incomming list */
  memcpy((char *)(this->list.at + this->store.ac), at, ac * sizeof(fts_atom_t)); 

  fts_outlet_send(o, 0, fts_s_list, this->list.ac, this->list.at);
}

/*********************************************
 *
 *  object
 *
 */

static void
listjoin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  this->list.at = (fts_atom_t *) fts_malloc(LIST_ALLOC_BLOCK * sizeof(fts_atom_t));
  this->list.at = 0;
  this->list.ac = 0;
  this->list.alloc = 0;

  this->store.at = 0;
  this->store.ac = 0;
  this->store.alloc = 0;

  if(ac > 1)
    listjoin_store_list(o, 0, 0, ac-1, at+1);
}

static void
listjoin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  fts_free(this->list.at);
  fts_free(this->store.at);
}

/*********************************************
 *
 *  class
 *
 */

static void
listjoin_common_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  fts_atom_t class_name;

  /* initialization */
  fts_class_init(cl, sizeof(listjoin_t), 2, 1, 0); 

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listjoin_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, listjoin_delete, 0, 0);

  /* user methods */
  fts_method_define_varargs(cl, 1, fts_s_list, listjoin_store_list);

  /* outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);
}

static fts_status_t
listappend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  listjoin_common_instantiate(cl, ac, at);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_append);

  return fts_Success;
}

static fts_status_t
listprepend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  listjoin_common_instantiate(cl, ac, at);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_prepend);

  return fts_Success;
}

void
listjoin_config(void)
{
  fts_metaclass_create(fts_new_symbol("listappend"), listappend_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("listprepend"), listprepend_instantiate, fts_always_equiv);
}







