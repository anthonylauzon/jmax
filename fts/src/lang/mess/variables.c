/* FTS Variable handling */

#include "sys.h"
#include "lang.h"
#include "lang/mess/messP.h"

/* Heaps */

static fts_heap_t variables_heap;
static fts_heap_t var_ref_heap;

/* Variable handling */

static fts_status_description_t fts_redefinedVariable = {"Redefined variable"};

static fts_variable_t *fts_patcher_variable_new(fts_object_t *owner, fts_patcher_t *p, fts_symbol_t name, fts_atom_t *value)
{
  fts_variable_t *v;

  v = (fts_variable_t *) fts_heap_alloc(&variables_heap);

  v->owner = owner;
  v->name = name;
  v->next = p->env;
  v->value = *value;

  p->env = v;

  return v;
}

static void
fts_patcher_variable_remove(fts_patcher_t *patcher, fts_object_t *owner)
{
  /* HERE @@@ ADD propagation of variable removing/unbinding */

  fts_variable_t **p;		/* indirect precursor */

  p = &(patcher->env); 

  while (*p)
    if ((*p)->owner == owner)
      {
	fts_variable_t *p2;

	p2 = *p;
	*p = (*p)->next;
	
	fts_heap_free((char *)p2, &variables_heap);
      }
    else
      p = &((*p)->next);
}


static void
fts_patcher_variable_remove_by_name(fts_patcher_t *patcher, fts_symbol_t name)
{
  /* HERE @@@ ADD propagation of variable removing/unbinding */

  fts_variable_t **p;		/* indirect precursor */

  p = &(patcher->env); 

  while (*p)
    if ((*p)->name == name)
      {
	fts_variable_t *p2;

	p2 = *p;
	*p = (*p)->next;
	
	fts_heap_free((char *)p2, &variables_heap);
      }
    else
      p = &((*p)->next);
}


/* Remove the variables the patcher declared internally */

void
fts_patcher_remove_variables(fts_patcher_t *patcher)
{
  fts_patcher_variable_remove(patcher, (fts_object_t *) patcher);
}


/* Remove the variables owner defined in its patcher */
   
void
fts_variable_remove_all(fts_object_t *owner)
{
  fts_patcher_variable_remove(fts_object_get_patcher(owner), owner);
}

static fts_variable_t *fts_patcher_variable_get(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_variable_t *p;

  for (p = patcher->env; p ; p = p->next)
    if (p->name == name)
      return p;

  return 0;
}


fts_atom_t *fts_variable_get_value(fts_object_t *obj, fts_symbol_t name)
{
  fts_patcher_t *patcher;

  if (fts_object_is_patcher(obj))
    patcher = (fts_patcher_t *) obj;
  else
    patcher = fts_object_get_patcher(obj);

  while (patcher)
    {
      fts_variable_t *v;

      v = fts_patcher_variable_get(patcher, name);

      if (v)
	return &(v->value);
      
      patcher = fts_object_get_patcher((fts_object_t *) patcher);
    }

  return 0;
}

fts_status_t
fts_patcher_variable_define(fts_patcher_t *patcher, fts_symbol_t name, fts_atom_t *value)
{
  fts_variable_t *v;

  v = fts_patcher_variable_get(patcher, name);

  if (v)
    return &fts_redefinedVariable;
  else
    {
      fts_patcher_variable_new((fts_object_t *) patcher, patcher, name, value);

      return fts_Success;
    }
}


fts_status_t
fts_variable_bind_to_object(fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *patcher;
  fts_variable_t *v;

  patcher = fts_object_get_patcher(obj);

  v = fts_patcher_variable_get(patcher, name);

  if (v)
    return &fts_redefinedVariable;
  else
    {
      fts_atom_t a;

      fts_set_object(&a, obj);
      fts_patcher_variable_new(obj, patcher, name, &a);

      obj->varname = name;
      return fts_Success;
    }
}


void fts_variable_unbind_to_object(fts_object_t *obj)
{
  if (obj->varname)
    fts_patcher_variable_remove_by_name(fts_object_get_patcher(obj), obj->varname);
}


void fts_variable_env_clean(fts_variable_t **env)
{
  while (*env)
    {
      fts_variable_t *v;

      v = *env;
      env = &((*env)->next);

      fts_heap_free((char *) v, &variables_heap);
    }
}


/*
 *
 * expression variable dependencies  handling support
 *
 */

#ifdef TODO
static void fts_expression_add_var_ref(fts_expression_state_t *e, fts_symbol_t name)
{
  fts_var_ref_t *p;

  p = (fts_var_ref_t *) fts_heap_alloc(&var_ref_heap);

  p->var  = name; 
  p->next  = e->var_refs;
  e->var_refs = p;
}
#endif


/* Module init function */

void fts_variables_init(void)
{
  fts_heap_init(&variables_heap, sizeof(fts_variable_t), 256);
  fts_heap_init(&var_ref_heap, sizeof(fts_var_ref_t), 16);
}
