/*
 * Parameter service for all FTS.
 * 
 * A parameter is a named atom value, that can be changed thru
 * an API and thru ucs/script commands; parameters can change dynamically
 * and the API implement the parameter listener concept, i.e. a callback
 * that will be called when the value is set of is changed.
 *
 * The implementation is oriented toward simplicity against run time
 * performance; these parameters should be set by the UI, not changed
 * very fast :->
 *
 * Should be integrated with patch variables ? Don't know really,
 * this stuff is more dynamic ..
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

typedef struct fts_param
{
  fts_symbol_t name;
  fts_atom_t   value;
  struct fts_param *next;
} fts_param_t;


typedef struct fts_param_listener
{
  fts_symbol_t name;
  fts_param_listener_fun_t listener_fun;
  void *listener;
  struct fts_param_listener *next;
} fts_param_listener_t;


static fts_heap_t *param_heap;
static fts_heap_t *param_listener_heap;

static fts_param_t *param_list = 0;
static fts_param_listener_t *param_listener_list = 0;

static void fts_param_run_listeners(fts_symbol_t name, const fts_atom_t *value);

/*
 * Note that a Parameter can't be removed.
 */

void fts_param_set(fts_symbol_t name, const fts_atom_t *value)
{
  fts_param_t *p;

  /* First, check if there, otherwise add it */

  for (p = param_list; p ; p = p->next)
    if (p->name == name)
      {
	p->value = *value;
	break;
      }

  if (! p)
    {
      /* Not found, allocate a new one */

      p = (fts_param_t *) fts_heap_alloc(param_heap);
      p->name  = name;
      p->value = *value;
      p->next  = param_list;

      param_list = p;
    }

  /* Second, run the listeners */

  fts_param_run_listeners(name, value);
}


const fts_atom_t *fts_param_get(fts_symbol_t name)
{
  fts_param_t *p;

  for (p = param_list; p ; p = p->next)
    if (p->name == name)
      return &(p->value);

  return 0;
}


/* Note that is a parameter is already set, the listener is called
   at "adding" time to tell him the current value; note also that a listener
   can be added multiple times, also for the same couple value.
   */


void fts_param_add_listener(fts_symbol_t name, void *listener,
			    fts_param_listener_fun_t listener_fun)
{
  fts_param_listener_t *p;
  fts_param_t *pp;

  /* Add the listener to the list */

  p = (fts_param_listener_t *) fts_heap_alloc(param_listener_heap);
  p->name         = name;
  p->listener_fun = listener_fun;
  p->listener     = listener;
  p->next         = param_listener_list;

  param_listener_list = p;

  /* Run the listener if the value is already defined */

  for (pp = param_list; pp ; pp = pp->next)
    if (pp->name == name)
      {
	(* p->listener_fun)(p->listener, p->name, &(pp->value));
	break;
      }
}


static void fts_param_run_listeners(fts_symbol_t name, const fts_atom_t *value)
{
  fts_param_listener_t *p;

  for (p = param_listener_list; p ; p = p->next)
    if (p->name == name)
      (* p->listener_fun)(p->listener, name, value);
}



void fts_param_remove_listener(void *listener)
{
  fts_param_listener_t **pp;	/* indirect precursor */

  pp = &(param_listener_list);

  while (*pp)
    {
      if (listener == (*pp)->listener)
	{
	  fts_param_listener_t *p;

	  p = (*pp);
	  (*pp) = (*pp)->next;
	  fts_heap_free((char *) p, param_listener_heap);
	}
      else
	pp = &((*pp)->next);
    }
}


float fts_param_get_float(fts_symbol_t name, float default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_number(v))
    return (float) fts_get_number(v);
  else
    return default_value;
}


int fts_param_get_int(fts_symbol_t name, int default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_int(v))
    return fts_get_int(v);
  else
    return default_value;
}

fts_symbol_t fts_param_get_symbol(fts_symbol_t name, fts_symbol_t default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_symbol(v))
    return fts_get_symbol(v);
  else
    return default_value;
}

void fts_params_init()
{
  param_heap = fts_heap_new(sizeof(fts_param_t));
  param_listener_heap = fts_heap_new(sizeof(fts_param_listener_t));
}
