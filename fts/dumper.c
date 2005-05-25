/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>

/*****************************************************************************
 *
 *  generic message dumper
 *
 */
void
fts_dumper_init(fts_dumper_t *dumper, fts_method_t send)
{
  dumper->send = send;
  dumper->message = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);

  fts_object_refer(dumper->message);
}

void
fts_dumper_destroy(fts_dumper_t *dumper)
{
  fts_object_release(dumper->message);
}

fts_message_t *
fts_dumper_message_get(fts_dumper_t *dumper, fts_symbol_t selector)
{
  fts_message_set(dumper->message, selector, 0, 0);

  return dumper->message;
}

void
fts_dumper_message_send(fts_dumper_t *dumper, fts_message_t *message)
{
  fts_symbol_t s = fts_message_get_selector(message);
  int ac = fts_message_get_ac(message);
  const fts_atom_t *at = fts_message_get_at(message);

  dumper->send((fts_object_t *)dumper, s, ac, at, fts_nix);
}

void
fts_dumper_send(fts_dumper_t *dumper, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dumper->send((fts_object_t *)dumper, s, ac, at, fts_nix);  
}

/*****************************************************************************
 *
 *  generic message loader
 *
 */
void
fts_loader_init(fts_loader_t *loader, fts_method_t load)
{
  loader->load = load;
  loader->target = NULL;
  loader->message = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);
  loader->n_mess = 0;
  
  fts_object_refer(loader->message);
}

void
fts_loader_destroy(fts_loader_t *loader)
{
  fts_object_release(loader->message);
}

fts_message_t *
fts_loader_message_get(fts_loader_t *loader, fts_symbol_t selector)
{
  fts_message_set(loader->message, selector, 0, 0);
  
  return loader->message;
}

void 
fts_loader_message_send(fts_loader_t *loader, fts_message_t *message)
{
  fts_symbol_t s = fts_message_get_selector(message);
  int ac = fts_message_get_ac(message);
  fts_atom_t *at = fts_message_get_at(message);
  
  if(loader->target != NULL)
    fts_send_message(loader->target, s, ac, at, fts_nix);
  
  loader->n_mess++;
}

void
fts_loader_send(fts_loader_t *loader, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  if(loader->target != NULL)
    fts_send_message(loader->target, s, ac, at, fts_nix);

  loader->n_mess++;
}

void
fts_loader_load(fts_loader_t *loader)
{
  loader->load((fts_object_t *)loader, NULL, 0, NULL, fts_nix);
}

/*****************************************************************************
 *
 *  recursive object dumper and loader
 *
 */
struct fts_object_dumper
{
  fts_dumper_t head;
  fts_hashtable_t hash; /* hash table of objects already saved (by pointer) and loaded (by id) */  
  fts_stack_t stack; /* stack of currently saved objects (by id) */
  int current_id; /* cache stack top */  
  int index; /* id counter */
  fts_dumper_t *line_dumper;
};

struct fts_object_loader
{
  fts_loader_t head;
  fts_hashtable_t hash; /* hash table of objects already saved (by pointer) and loaded (by id) */
  fts_loader_t *line_loader;
};

static fts_class_t *object_dumper_class = NULL;
static fts_class_t *object_loader_class = NULL;
static fts_symbol_t sym_dash_obj = NULL;
static fts_symbol_t sym_dash_mess = NULL;

static fts_symbol_t
id_to_symbol(int id)
{
  char str[64];
  
  if(id > 0)
    sprintf(str, "#%d", id);
  else
    sprintf(str, "#NULL");
  
  return fts_new_symbol(str);
}

static int
string_to_id(char *str)
{
  if(str[1]=='#' && str[2] > '0' && str[2] <= '9')
  {
    int id = 0;
    int i;
    
    for(i=2; str[i] != '\0'; i++)
      id = id * 10 + (str[i] - '0');
    
    return id;
  }
  
  return 0;
}

static int
object_dumper_put_object(fts_object_dumper_t *dumper, fts_object_t *obj)
{
  int id = ++dumper->index;
  fts_atom_t a_id, a_obj;
  
  fts_set_pointer(&a_obj, obj);
  fts_set_int(&a_id, id);
  fts_hashtable_put(&dumper->hash, &a_obj, &a_id);
  
  return id;
}

static int
object_dumper_get_id(fts_object_dumper_t *dumper, fts_object_t *obj)
{
  fts_atom_t a_id, a_obj;
  
  fts_set_pointer(&a_obj, (void *)obj);
  
  if(fts_hashtable_get(&dumper->hash, &a_obj, &a_id))
    return fts_get_int(&a_id);
  else
    return 0;  
}

static int object_dumper_dump_reference(fts_object_dumper_t *dumper, fts_object_t *obj);

static void
object_dumper_dump_object(fts_object_dumper_t *dumper, fts_object_t *obj, int id)
{
  fts_class_t *cl = fts_object_get_class(obj);
  fts_array_t array;
  fts_atom_t a;  
  fts_atom_t *at;
  int ac;
  int i;
  
  /* push id on save stack */
  fts_stack_push_int(&dumper->stack, id);
  dumper->current_id = id;
  
  /* save message: _#obj <id> <object description> */
  fts_array_init(&array, 0, NULL);
  fts_array_append_int(&array, id);
  (*fts_class_get_description_function(cl))(obj, &array);
  
  ac = fts_array_get_size(&array);
  at = fts_array_get_atoms(&array);
  
  /* dump object referenced in description first */
  for(i=1; i<ac; i++)
  {
    if(fts_is_object(at + i))
    {
      fts_object_t *argobj = fts_get_object(at + i);
      int argid = object_dumper_dump_reference(dumper, argobj);
      
      /* replace object argument by id symbol #<id> */
      fts_set_symbol(at + i, id_to_symbol(argid));
      fts_object_release(argobj); /* object was referenced by array */
    }
  }
  
  /* write dump message #obj <id> <class name> [<args> ...] */
  if(dumper->line_dumper != NULL)
    fts_dumper_send(dumper->line_dumper, sym_dash_obj, ac, at);
  else
  {
    fts_post("#obj ");
    fts_post_atoms(ac, at);
    fts_post("\n");      
  }
  
  fts_array_destroy(&array);
  
  fts_set_object(&a, dumper);
  fts_send_message(obj, fts_s_dump_state, 1, &a, fts_nix);
  
  /* pop id from stack */
  fts_stack_pop(&dumper->stack, 1);
  dumper->current_id = fts_stack_top_int(&dumper->stack);
}

static int
object_dumper_dump_reference(fts_object_dumper_t *dumper, fts_object_t *obj)
{
  int id = object_dumper_get_id(dumper, obj);
  
  /* dump object if not saved yet */
  if(id == 0)
  {
    id = object_dumper_put_object(dumper, obj);    
    object_dumper_dump_object(dumper, obj, id);
  }
  
  return id;
}

static fts_method_status_t
object_dumper_send(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_dumper_t *self = (fts_object_dumper_t *)o;
  
  if(self->current_id > 0)
  {
    /* write object state */
    fts_atom_t *args = alloca(sizeof(fts_atom_t) * (ac + 2));
    int i;
    
    fts_set_int(args, self->current_id);
    fts_set_symbol(args + 1, s);
    
    /* save unsaved objects first */
    for(i=0; i<ac; i++)
    {
      if(fts_is_object(at + i))
      {
        fts_object_t *obj = fts_get_object(at + i);
        int id = object_dumper_dump_reference(self, obj);
        
        /* replace object argument by id symbol #<id> */
        fts_set_symbol(args + i + 2, id_to_symbol(id));
      }
      else
        args[i + 2] = at[i];
    }
    
    if(self->line_dumper != NULL)
      fts_dumper_send(self->line_dumper, sym_dash_mess, ac + 2, args);
    else
    {
      fts_post("#mess ");
      fts_post_atoms(ac + 2, args);
      fts_post("\n");      
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
object_dumper_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_dumper_t *self = (fts_object_dumper_t *)o;
  
  fts_dumper_init((fts_dumper_t *)self, object_dumper_send);
  
  fts_hashtable_init(&self->hash, FTS_HASHTABLE_MEDIUM);
  fts_stack_init_int(&self->stack);
  
  self->current_id = 0;
  self->index = 0;
  self->line_dumper = NULL;
  
  if(ac > 0 && fts_is_object(at))
  {
    self->line_dumper = (fts_dumper_t *)fts_get_object(at);
    fts_object_refer((fts_object_t *)self->line_dumper);
  }
  
  return fts_ok;
}

static fts_method_status_t
object_dumper_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_dumper_t *self = (fts_object_dumper_t *)o;
  
  fts_hashtable_destroy(&self->hash);
  fts_stack_destroy(&self->stack);
  fts_dumper_destroy((fts_dumper_t *)self);
  
  if(self->line_dumper != NULL)
    fts_object_release((fts_object_t *)self->line_dumper);
  
  return fts_ok;
}

static void
object_dumper_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_dumper_t), object_dumper_init, object_dumper_delete);
}

static int
object_loader_put_object(fts_object_loader_t *loader, int id, fts_object_t *obj)
{
  fts_atom_t a_id, a_obj;
  
  fts_set_int(&a_id, id);
  fts_set_pointer(&a_obj, obj);
  return fts_hashtable_put(&loader->hash, &a_id, &a_obj);
}

static fts_object_t *
object_loader_get_object(fts_object_loader_t *loader, int id)
{
  fts_atom_t a_id, a_obj;
  
  fts_set_int(&a_id, id);
  
  if(fts_hashtable_get(&loader->hash, &a_id, &a_obj))
    return (fts_object_t *)fts_get_pointer(&a_obj);
  else
    return NULL;
}

static void
object_loader_replace_symbols(fts_object_loader_t *loader, int ac, const fts_atom_t *at, fts_atom_t *args)
{
  int i;
  
  /* resolve id symbols */
  for(i=0; i<ac; i++)
  {
    if(fts_is_symbol(at + i))
    {
      fts_symbol_t sym = fts_get_symbol(at + i);
      char *str = fts_symbol_name(sym);      
      int id = string_to_id(str);
      
      if(id > 0)
      {
        fts_object_t *obj = object_loader_get_object(loader, id);
        
        /* replace id symbol argument by object */
        if(obj != NULL)
          fts_set_object(args + i, obj);
        else
        {
          /* ERROR: invalid id */
          fts_set_int(args + i, 0);
          fts_post("error in object loader: invalid object id %ld\n", id);
        }
      }
      else
        args[i] = at[i];
    }
    else
      args[i] = at[i];
  }
}

static fts_method_status_t
object_loader_restore_object(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_loader_t *loader = (fts_object_loader_t *)o;
  
  if(ac > 0 && fts_is_int(at))
  {
    int id = fts_get_int(at);
    
    if(ac > 1 && fts_is_symbol(at + 1))
    {
      fts_symbol_t cl_name = fts_get_symbol(at + 1);
      fts_class_t *cl = fts_get_class_by_name(cl_name);
      
      /* skip id and class name */
      ac -= 2;
      at += 2;
      
      if(cl != NULL)
      {
        if(id > 0)
        {
          fts_object_t *obj = object_loader_get_object(loader, id);
          
          if(obj == NULL)
          {
            fts_atom_t *args = (fts_atom_t *)at;
            
            if(ac > 0)
            {
              args = alloca(sizeof(fts_atom_t) * ac);
              object_loader_replace_symbols(loader, ac, at, args);      
            }
            
            /* create new object and register */
            obj = fts_object_create(cl, ac, args);
            object_loader_put_object(loader, id, obj);
            
            if(obj != NULL)
              object_loader_put_object(loader, id, obj);
            else
            {
              fts_post("error in object loader: invalid object arguments\n");
            }
          }
        }
      }
      else
      {
        fts_post("error in object loader: invalid class %s\n", fts_symbol_name(cl_name));
      }
    }
    else
    {
      fts_post("error in object loader: missing class name\n");
    }
  }
  else
  {
    fts_post("error in object loader: missing object id\n");
  }
  
  return fts_ok;
}

static fts_method_status_t
object_loader_restore_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_loader_t *loader = (fts_object_loader_t *)o;
  
  if(ac > 0 && fts_is_int(at))
  {
    int id = fts_get_int(at);
    
    if(ac > 1 && fts_is_symbol(at + 1))
    {
      fts_symbol_t selector = fts_get_symbol(at + 1);
      fts_object_t *obj = object_loader_get_object(loader, id);
      fts_atom_t *args = (fts_atom_t *)at;
      
      /* skip id and selector */
      ac -= 2;
      at += 2;
      
      if(ac > 0)
      {
        args = alloca(sizeof(fts_atom_t) * ac);
        object_loader_replace_symbols(loader, ac, at, args);
      }
      
      fts_send_message(obj, selector, ac, args, fts_nix);
    }
    else
    {
      fts_post("error in object loader: missing message selector\n");
    }
  }
  else
  {
    fts_post("error in object loader: missing object id\n");
  }
  
  return fts_ok;
}

static fts_method_status_t
object_loader_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_loader_t *self = (fts_object_loader_t *)o;
  
  fts_loader_init((fts_loader_t *)self, NULL);
  
  fts_hashtable_init(&self->hash, FTS_HASHTABLE_MEDIUM);
  
  if(ac > 0 && fts_is_object(at))
  {
    self->line_loader = (fts_loader_t *)fts_get_object(at);
    fts_object_refer((fts_object_t *)self->line_loader);
    
    fts_loader_set_target(self->line_loader, o);
  }
  
  return fts_ok;
}

static fts_method_status_t
object_loader_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_loader_t *self = (fts_object_loader_t *)o;
  
  fts_hashtable_destroy(&self->hash);
  
  if(self->line_loader != NULL)
    fts_object_release((fts_object_t *)self->line_loader);
  
  return fts_ok;
}

static void
object_loader_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_loader_t), object_loader_init, object_loader_delete);
  
  fts_class_message_varargs(cl, sym_dash_obj, object_loader_restore_object);
  fts_class_message_varargs(cl, sym_dash_mess, object_loader_restore_message);
}

static void
object_loader_load_object(fts_object_loader_t *loader, fts_object_t *obj)
{
  fts_loader_t *line_loader = loader->line_loader;
  fts_object_t *loaded = NULL;
  
  if(line_loader != NULL)
    fts_loader_load(line_loader);
  
  loaded = object_loader_get_object(loader, 1);
  
  if(loaded != NULL)
  {
    fts_class_t *obj_class = fts_object_get_class(obj);
    fts_class_t *loaded_class = fts_object_get_class(loaded);
    
    if(loaded_class == obj_class)
      fts_object_snatch(obj, loaded);
    else
      fts_post("error in object loader: found %s object while expecting %s\n", fts_symbol_name(fts_class_get_name(loaded_class)), fts_symbol_name(fts_class_get_name(obj_class)));
  }
  else
    fts_post("error in object loader: didn't find root object\n");
}

/****************************************************************************
 *
 *  dump and load object
 *
 */
void
fts_object_dump(fts_object_t *obj, fts_dumper_t *line_dumper)
{
  fts_object_dumper_t *dumper = NULL;
  fts_atom_t a;
  
  if(line_dumper != NULL)
  {
    fts_set_object(&a, (fts_object_t *)line_dumper);
    dumper = (fts_object_dumper_t *)fts_object_create(object_dumper_class, 1, &a);
  }
  else
    dumper = (fts_object_dumper_t *)fts_object_create(object_dumper_class, 0, NULL);
    
  fts_object_refer((fts_object_t *)dumper);
  
  /* dump object with or without state */
  object_dumper_dump_object(dumper, obj, object_dumper_put_object(dumper, obj));

  fts_object_release((fts_object_t *)dumper);
}

void
fts_object_load(fts_object_t *obj, fts_loader_t *line_loader)
{
  fts_object_loader_t *loader = NULL;
  fts_atom_t a;
  
  fts_set_object(&a, (fts_object_t *)line_loader);
  loader = (fts_object_loader_t *)fts_object_create(object_loader_class, 1, &a);
  fts_object_refer((fts_object_t *)loader);
  
  object_loader_load_object(loader, obj);
  
  fts_object_release((fts_object_t *)loader);
}

/***********************************************************************
 *
 * initialization
 *
 */

void
fts_kernel_dumper_init(void)
{
  object_dumper_class = fts_class_install(NULL, object_dumper_instantiate);
  object_loader_class = fts_class_install(NULL, object_loader_instantiate);
  sym_dash_obj = fts_new_symbol("#obj");
  sym_dash_mess = fts_new_symbol("#mess");;
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
