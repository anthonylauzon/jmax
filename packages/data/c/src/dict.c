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

#include <fts/fts.h>
#include "dict.h"

fts_symbol_t dict_symbol = 0;
fts_metaclass_t *dict_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_coll = 0;

static fts_hashtable_t *
dict_get_hash(dict_t *dict, const fts_atom_t *key)
{
  return (fts_is_int(key))? &dict->table_int: &dict->table_symbol;
}

void
dict_store(dict_t *dict, const fts_atom_t *key, const fts_atom_t *atom)
{
  fts_hashtable_t *hash = dict_get_hash(dict, key);
  fts_atom_t a;

  /* remove old entry for same key */
  if(fts_hashtable_get(hash, key, &a))
    fts_atom_void(&a);  

  fts_atom_assign(&a, atom);

  /* insert atom to hashtable */
  fts_hashtable_put(hash, key, &a);
}

void
dict_store_atoms(dict_t *dict, const fts_atom_t *key, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    dict_store(dict, key, at);
  else if(ac > 1)
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      dict_store(dict, key, &a);
    }
}

void
dict_recall(dict_t *dict, const fts_atom_t *key, fts_atom_t *atom)
{
  fts_hashtable_t *hash = dict_get_hash(dict, key);
  
  fts_set_void(atom);
  fts_hashtable_get(hash, key, atom);
}

static void
dict_remove(dict_t *dict, const fts_atom_t *key)
{
  fts_hashtable_t *hash = (fts_is_int(key))? &dict->table_int: &dict->table_symbol;
  fts_atom_t value;
  
  if(fts_hashtable_get(hash, key, &value))
    {
      fts_atom_void(&value);      
      fts_hashtable_remove(hash, key);
    }
}

static void
dict_remove_all(dict_t *dict)
{
  fts_hashtable_t *hashtabs[2];
  int tab;
	  
  hashtabs[0] = &dict->table_int;
  hashtabs[1] = &dict->table_symbol;

  for(tab=0; tab<2; tab++)
    {
      fts_hashtable_t *hash = hashtabs[tab];
      fts_iterator_t iterator;
      
      /* get int table */
      fts_hashtable_get_values(hash, &iterator);
	      
      while(fts_iterator_has_more(&iterator))
	{
	  fts_atom_t value;
		      
	  fts_iterator_next(&iterator, &value);
	  fts_set_void(&value);
	}
	      
      fts_hashtable_clear(hash);
    }
}

void
dict_get_keys(dict_t *dict, fts_array_t *array)
{
  fts_iterator_t iterators[2];
  int tab;

  fts_hashtable_get_keys(&dict->table_int, iterators + 0);
  fts_hashtable_get_keys(&dict->table_symbol, iterators + 1);

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *iterator = iterators + tab;

      while(fts_iterator_has_more(iterator))
	{
	  fts_atom_t key;

	  fts_iterator_next(iterator, &key);
	  fts_array_append(array, 1, &key);
	}
    }
}

void
dict_copy(dict_t *org, dict_t *copy)
{
  fts_hashtable_t *org_hash[2];
  fts_hashtable_t *copy_hash[2];
  int tab;

  /* clear copy */
  dict_remove_all(copy);

  /* init org hash tables */
  org_hash[0] = &org->table_int;
  org_hash[1] = &org->table_symbol;

  /* init copy hash tables */
  copy_hash[0] = &copy->table_int;
  copy_hash[1] = &copy->table_symbol;

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t key_iterator;
      fts_iterator_t value_iterator;
      
      /* iterate on org hash table */
      fts_hashtable_get_keys(org_hash[tab], &key_iterator);
      fts_hashtable_get_values(org_hash[tab], &value_iterator);
	      
      while(fts_iterator_has_more(&key_iterator))
	{
	  fts_atom_t key;
	  fts_atom_t value;
		      
	  /* get key */
	  fts_iterator_next(&key_iterator, &key);
	  fts_iterator_next(&value_iterator, &value);

	  /* claim entry */
	  fts_atom_refer(&value);

	  /* store entry to copy hash table */
	  fts_hashtable_put(copy_hash[tab], &key, &value);
	}
    }
}

/**********************************************************
 *
 *  user methods
 *
 */

static void
dict_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  
  if(ac > 0 && (fts_is_symbol(at) || fts_is_int(at)))
    {
      if(ac > 1)
	dict_store_atoms(this, at, ac - 1, at + 1);
      else
	dict_remove(this, at);	
    }
}

static void
dict_get(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  
  if(ac > 0 && (fts_is_symbol(at) || fts_is_int(at)))
    {
      fts_atom_t a;

      dict_recall(this, at, &a);
      fts_outlet_atom(o, 0, &a);
    }
}

static void
dict_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;

  if(ac > 0)
    { 
      if(fts_is_int(at) || fts_is_symbol(at))
	dict_remove(this, at);
    }
  else
    dict_remove_all(this);
}

static void
dict_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  
  if(ac > 1 && (fts_is_symbol(at) || fts_is_int(at)))
    {
      fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_metaclass, ac - 1, at + 1);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)mess);
      dict_store(this, at, &a);
    }
}

static void
dict_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  dict_t *in = dict_atom_get(at);
  
  dict_copy(in, this);
}

static void
dict_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_hashtable_t *hash[2] = {&this->table_int, &this->table_symbol};
  int tab;

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *key_iterator, *value_iterator;

      fts_hashtable_get_keys(hash[tab], key_iterator);
      fts_hashtable_get_values(hash[tab], value_iterator);

      while(fts_iterator_has_more(key_iterator))
	{
	  fts_atom_t key, value;
	  
	  fts_iterator_next(key_iterator, &key);
	  fts_iterator_next(value_iterator, &value);

	  if(fts_is_message(&value))
	    {
	      fts_message_t *message = (fts_message_t *)fts_get_object(&value);
	      fts_symbol_t mess_s = fts_message_get_selector(message);
	      int mess_ac = fts_message_get_ac(message);
	      const fts_atom_t *mess_at = fts_message_get_at(message);
	      fts_message_t *dump_mess = fts_dumper_message_new(dumper, fts_s_message);

	      /* dump key */
	      fts_message_append(dump_mess, 1, &key);
	      
	      /* dump message */
	      fts_message_append_symbol(dump_mess, mess_s);
	      fts_message_append(dump_mess, mess_ac, mess_at);
	      fts_dumper_message_send(dumper, dump_mess);
	    }
	  else if(fts_is_tuple(&value))
	    {
	      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&value);
	      int size = fts_tuple_get_size(tuple);
	      const fts_atom_t *atoms = fts_tuple_get_atoms(tuple);
	      fts_message_t *dump_mess = fts_dumper_message_new(dumper, fts_s_set);
	      
	      /* dump key */
	      fts_message_append(dump_mess, 1, &key);	      

	      /* dump tuple */
	      fts_message_append(dump_mess, size, atoms);
	      fts_dumper_message_send(dumper, dump_mess);
	    }
	  else if(fts_is_object(&value))
	    {
	      /* don't now how to do yet */
	    }
	  else
	    {
	      fts_message_t *dump_mess = fts_dumper_message_new(dumper, fts_s_set);

	      /* dump key */
	      fts_message_append(dump_mess, 1, &key);
	      
	      /* dump value */
	      fts_message_append(dump_mess, 1, &value);
	      fts_dumper_message_send(dumper, dump_mess);
	    }
	}
    }
}

/**********************************************************
 *
 *  files
 *
 */
#define DICT_ATOM_BUF_BLOCK_SIZE 64
#define DICT_BLOCK_SIZE 64

static fts_atom_t *
dict_atom_buf_realloc(fts_atom_t *buf, int size)
{
  fts_atom_t *new_buf = (fts_atom_t *)fts_realloc(buf, sizeof(fts_atom_t) * size); /* double size */

  return new_buf;
}

static void
dict_atom_buf_free(fts_atom_t *buf, int size)
{
  if(buf)
    fts_free(buf);
}

static int 
dict_import_from_coll(dict_t *this, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
  int atoms_alloc = DICT_ATOM_BUF_BLOCK_SIZE;
  fts_atom_t *atoms = 0;
  enum {read_key, read_comma, read_argument} state = read_key;
  char *error = 0;
  int i = 0;
  int n = 0;
  fts_atom_t key;
  fts_atom_t a;
  char c;

  if(!file)
    return 0;

  atoms = dict_atom_buf_realloc(atoms, atoms_alloc);

  dict_clear((fts_object_t *)this, 0, 0, 0, 0);
  fts_set_void(&key);

  while(error == 0 && fts_atom_file_read(file, &a, &c))
    {
      switch(state)
	{
	case read_key:
	  {
	    if(fts_is_symbol(&a) || fts_is_int(&a))
	      {
		key = a;
		state = read_comma;
	      }
	    else
	      error = "wrong key type";	      
	  }
	  
	  break;
	  
	case read_comma:
	  {
	    if(fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
	      state = read_argument;
	    else
	      error = "comma expected";
	  }
	  
	  break;

	case read_argument:
	  {
	    if(fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(";")))
	      {
		if(n > 0)
		  {
		    if(fts_is_symbol(atoms + 0))
		      {
			fts_symbol_t selector = fts_get_symbol(atoms + 0);
			
			if(selector == fts_s_int || selector == fts_s_float || selector == fts_s_symbol || fts_s_list)
			  dict_store_atoms(this, &key, n - 1, atoms + 1);
		      }
		    else
		      dict_store_atoms(this, &key, n, atoms);

		    i++;
		    n = 0;

		    state = read_key;
		  }
		else
		  post("dict: empty message found in coll file %s (ignored)\n", file_name);
	      }
	    else
	      {
		/* read argument */
		if(n >= atoms_alloc)
		  {
		    atoms_alloc += DICT_ATOM_BUF_BLOCK_SIZE;
		    atoms = dict_atom_buf_realloc(atoms, atoms_alloc);
		  }
		
		atoms[n] = a;
		n++;
	      }
	  }
	  
	  break;
	}
    }
  
  if(error != 0)
    post("dict: error reading coll file %s (%s)\n", file_name, error);
  else if(state != read_key)
    {
      if(n > 0)
	{
	  dict_store_atoms(this, &key, n, atoms);
	  i++;
	}
      
      post("dict: found unexpected ending in coll file %s\n", file_name);
    }
  
  dict_atom_buf_free(atoms, atoms_alloc);
  fts_atom_file_close(file);

  return i;
}

static int 
dict_export_to_coll(dict_t *this, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "w");
  fts_hashtable_t *hash[2] = {&this->table_int, &this->table_symbol};
  fts_atom_t a;
  int size = 0;
  int tab;
  int i;

  if(!file)
    return 0;

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *key_iterator, *value_iterator;

      fts_hashtable_get_keys(hash[tab], key_iterator);
      fts_hashtable_get_values(hash[tab], value_iterator);

      while(fts_iterator_has_more(key_iterator))
	{
	  fts_atom_t key, value;
	  fts_symbol_t s = NULL;
	  int ac = 0;
	  const fts_atom_t *at = NULL;
	  fts_atom_t a;
	  
	  fts_iterator_next(key_iterator, &key);
	  fts_iterator_next(value_iterator, &value);

	  if(fts_is_message(&value))
	    {
	      fts_message_t *message = (fts_message_t *)fts_get_object(&value);

	      s = fts_message_get_selector(message);
	      ac = fts_message_get_ac(message);
	      at = fts_message_get_at(message);
	    }
	  else if(fts_is_tuple(&value))
	    {
	      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&value);

	      s = fts_s_list;
	      ac = fts_tuple_get_size(tuple);
	      at = fts_tuple_get_atoms(tuple);
	    }
	  else if(fts_is_symbol(&value))
	    {
	      s = fts_s_symbol;
	      ac = 1;
	      at = &value;
	    }
	  
	  else if(!fts_is_object(&value))
	    {
	      ac = 1;
	      at = &value;
	    }
	  
	  /* write key */
	  fts_atom_file_write(file, &key, ' ');
	  
	  /* write comma */
	  fts_set_symbol(&a, fts_s_comma);
	  fts_atom_file_write(file, &a, ' ');
	  
	  /* write selector (if any) */
	  if(s)
	    {
	      fts_set_symbol(&a, s);
	      fts_atom_file_write(file, &a, ' ');
	    }
	  
	  /* write arguments */
	  for(i=0; i<ac; i++)
	    fts_atom_file_write(file, at + i, ' ');

	  /* write semicolon and new line */
	  fts_set_symbol(&a, fts_s_semi);
	  fts_atom_file_write(file, &a, '\n');
	  
	  size++;
	}
    }

  fts_atom_file_close(file);
  
  return size;
}

static void
dict_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = dict_import_from_coll(this, file_name);    
  else
    {
      post("dict: unknown import file format \"%s\"\n", file_format);
      return;
    }

  if(size <= 0)
    post("dict: can't import from file \"%s\"\n", file_name);  
}

static void
dict_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = dict_export_to_coll(this, file_name);    
  else
    {
      post("dict: unknown export file format \"%s\"\n", file_format);
      return;
    }

  if(size <= 0)
    post("dict: can't export to file \"%s\"\n", file_name);  
}

static void
dict_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  dict_get_keys(this, array);
}

static void
dict_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static void
dict_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  fts_hashtable_t *hash[2] = {&this->table_int, &this->table_symbol};
  int size = fts_hashtable_get_size(&this->table_int) + fts_hashtable_get_size(&this->table_symbol);
  int tab;

  if(size > 0)
    {
      for(tab=0; tab<2; tab++)
	{
	  fts_iterator_t *key_iterator, *value_iterator;
	  
	  fts_hashtable_get_keys(hash[tab], key_iterator);
	  fts_hashtable_get_values(hash[tab], value_iterator);
	  
	  while(fts_iterator_has_more(key_iterator))
	    {
	      fts_atom_t key, value;
	      
	      fts_iterator_next(key_iterator, &key);
	      fts_iterator_next(value_iterator, &value);
	      
	      if(fts_is_int(&key))
		post("  %d: ", fts_get_int(&key));
	      else
		post("  %s: ", fts_get_symbol(&key));
	      
	      if(fts_is_message(&value))
		{
		  fts_message_t *message = (fts_message_t *)fts_get_object(&value);
		  fts_symbol_t mess_s = fts_message_get_selector(message);
		  int mess_ac = fts_message_get_ac(message);
		  const fts_atom_t *mess_at = fts_message_get_at(message);
		  
		  post("%s ", mess_s);
		  
		  if(mess_ac)
		    post_atoms(mess_ac, mess_at);
		  
		  post("\n");
		}
	      else if(fts_is_tuple(&value))
		{
		  fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&value);
		  int size = fts_tuple_get_size(tuple);
		  const fts_atom_t *atoms = fts_tuple_get_atoms(tuple);
		  
		  post("(");
		  
		  if(size)
		    post_atoms(size, atoms);
		  
		  post(")\n");
		}
	      else if(fts_is_object(&value))
		post("<%s>\n", fts_object_get_class_name(fts_get_object(&value)));
	      else
		{
		  post_atoms(1, &value);
		  post("\n");
		}
	    }
	  
	  post("}\n");
	}
    }
  else
    post("(empty dict)\n");
}

/**********************************************************
 *
 *  class
 *
 */

static void
dict_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;
  int i;

  fts_hashtable_init(&this->table_int, FTS_HASHTABLE_INT, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->table_symbol, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);

  data_object_set_keep((data_object_t *)o, fts_s_no);

  for(i=0; i<ac; i++)
    {
      if(fts_is_tuple(at + i))
	{
	  fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at + i);
	  int tac = fts_tuple_get_size(tup);
	  fts_atom_t *tat = fts_tuple_get_atoms(tup);
	  
	  if(tac > 1 && (fts_is_int(tat) || fts_is_symbol(tat)))
	    dict_store_atoms(this, tat, tac - 1, tat + 1);
	  else
	    {
	      dict_clear(o, 0, 0, 0, 0);
	      fts_object_set_error(o, "Wrong key type in initialization");
	    }
	}
      else
	{
	  dict_clear(o, 0, 0, 0, 0);
	  fts_object_set_error(o, "Wrong arguments");
	}

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
}

static void
dict_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *this = (dict_t *)o;

  dict_clear(o, 0, 0, 0, 0);
}

static fts_status_t
dict_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(dict_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dict_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, dict_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, dict_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, dict_set); /* from file */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_message, dict_message); /* from file */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, dict_get_array);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_clear, dict_clear);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, dict_dump);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, dict_print);

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, dict_get_state);
  
  fts_method_define_varargs(cl, 0, fts_s_import, dict_import);
  fts_method_define_varargs(cl, 0, fts_s_export, dict_export);
  
  fts_method_define_varargs(cl, 0, fts_s_put, dict_set);
  fts_method_define_varargs(cl, 0, fts_s_set, dict_set);
  fts_method_define_varargs(cl, 0, fts_s_get, dict_get);
  fts_method_define_varargs(cl, 0, fts_s_clear, dict_clear);
  
  return fts_Success;
}

void
dict_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_coll = fts_new_symbol("coll");
  dict_symbol = fts_new_symbol("dict");

  dict_type = fts_class_install(dict_symbol, dict_instantiate);
  fts_alias_install(fts_new_symbol("messtab"), dict_symbol);
}
