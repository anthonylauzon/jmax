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
#include "message.h"

typedef struct messtab
{
  fts_object_t o;
  fts_hashtable_t table_symbol;
  fts_hashtable_t table_int;
  int locked;
} messtab_t;

typedef struct getmess
{
  fts_object_t o;
  messtab_t *messtab;
  fts_atom_t key;
} getmess_t;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_coll = 0;

static fts_symbol_t messtab_symbol = 0;

static fts_symbol_t
messtab_store(messtab_t *messtab, const fts_atom_t *key, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_hashtable_t *hash = (fts_is_int(key))? &messtab->table_int: &messtab->table_symbol;
  fts_atom_t value;
  message_t *mess;

  if(s)
    {
      mess = (message_t *)fts_object_create(message_class, 0, 0);
      message_set(mess, s, ac, at);
    }
  else
    {
      fts_symbol_t error = 0;

      mess = (message_t *)fts_object_create(message_class, ac, at);
      error = fts_object_get_error((fts_object_t *)mess);

      if(error)
	{
	  fts_object_destroy((fts_object_t *)mess);
	  return error;
	}
    }

  fts_object_refer((fts_object_t *)mess);  

  /* remove old entry for same key */
  if(fts_hashtable_get(hash, key, &value))
    fts_atom_release(&value);

  /* insert message to hashtable */
  fts_set_object(&value, (void *)mess);
  fts_hashtable_put(hash, key, &value);

  return 0;
}

static message_t *
messtab_recall(messtab_t *messtab, const fts_atom_t *key)
{
  fts_hashtable_t *hash = (fts_is_int(key))? &messtab->table_int: &messtab->table_symbol;
  fts_atom_t value;
  
  if(fts_hashtable_get(hash, key, &value))
    return (message_t *)fts_get_object(&value);
  else
    return 0;
}

static void
messtab_remove(messtab_t *messtab, const fts_atom_t *key)
{
  fts_hashtable_t *hash = (fts_is_int(key))? &messtab->table_int: &messtab->table_symbol;
  fts_atom_t value;
  
  if(fts_hashtable_get(hash, key, &value))
    {
      fts_atom_release(&value);
      
      fts_hashtable_remove(hash, key);
    }
}

static void
messtab_get_entry(messtab_t *messtab, fts_atom_t *key, fts_symbol_t *s, int *ac, const fts_atom_t **at)
{
  message_t *mess = messtab_recall(messtab, key);
  fts_symbol_t mess_s = message_get_selector(mess);
  int mess_ac = message_get_ac(mess);
  const fts_atom_t *mess_at = message_get_at(mess);
  
  if(mess_ac == 1 && (fts_get_selector(mess_at) == mess_s))
    *s = 0;
  else if(mess_s == fts_s_list)
    *s = 0;
  else
    *s = mess_s;

  *ac = mess_ac;
  *at = mess_at;
}

/**********************************************************
 *
 *  user methods
 *
 */

static void
messtab_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  
  if(ac > 1 && (fts_is_symbol(at) || fts_is_int(at)))
    {
      const fts_atom_t *key = at;

      /* skip key */
      ac--;
      at++;

      if(ac > 0)
	messtab_store(this, key, 0, ac, at);
      else
	messtab_store(this, key, fts_s_bang, 0, 0);
    }
}

static void
messtab_get(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  
  if(ac > 0 && (fts_is_symbol(at) || fts_is_int(at)))
    {
      message_t *mess = messtab_recall(this, at);

      if(mess)
	{
	  this->locked = 1; /* lock messtab */
	  fts_outlet_send(o, 1, fts_get_selector(at), 1, at);
	  message_output(o, 0, mess);
	  this->locked = 0;
	}
    }
}

static void
messtab_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  if(!this->locked)
    {
      if(ac > 0)
	{ 
	  if(fts_is_int(at) || fts_is_symbol(at))
	    messtab_remove(this, at);
	}
      else
	{
	  fts_hashtable_t *hashtabs[2];
	  int tab;
	  
	  hashtabs[0] = &this->table_int;
	  hashtabs[1] = &this->table_symbol;

	  for(tab=0; tab<2; tab++)
	    {
	      fts_hashtable_t *hash = hashtabs[tab];
	      fts_iterator_t iterator;
      
	      /* get int table */
	      fts_hashtable_get_values(hash, &iterator);
	      
	      while ( fts_iterator_has_more( &iterator) )
		{
		  fts_atom_t value;
		      
		  fts_iterator_next( &iterator, &value);
		  fts_atom_void(&value);
		}
	      
	      fts_hashtable_clear(hash);
	    }
	}
    }
}

static void
messtab_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_iterator_t iterators[2];
  int tab;

  this->locked = 1;

  fts_hashtable_get_keys(&this->table_int, iterators + 0);
  fts_hashtable_get_keys(&this->table_symbol, iterators + 1);

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *iterator = iterators + tab;

      while ( fts_iterator_has_more( iterator) )
	{
	  fts_atom_t key;
	  message_t *mess;
	  fts_symbol_t mess_s;
	  int mess_ac;
	  const fts_atom_t *mess_at;
	  fts_array_t *aa;
	  fts_atom_t a;
	  
	  fts_iterator_next( iterator, &key);
	  mess = messtab_recall(this, &key);
	  
	  fts_outlet_send(o, 1, fts_get_selector(&key), 1, &key);
	  message_output(o, 0, mess);
	}
    }

  this->locked = 0;
}

static void
messtab_get_state_as_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_array_t *array = fts_get_list(at);
  int size = 0;
  fts_iterator_t iterators[2];
  int tab;

  fts_hashtable_get_keys(&this->table_int, iterators + 0);
  fts_hashtable_get_keys(&this->table_symbol, iterators + 1);

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *iterator = iterators + tab;

      while ( fts_iterator_has_more( iterator) )
	{
	  fts_atom_t key;
	  message_t *mess;
	  fts_symbol_t mess_s;
	  int mess_ac;
	  const fts_atom_t *mess_at;
	  fts_array_t *aa;
	  fts_atom_t a;
	  
	  size++;
	  fts_array_set_size(array, size);

	  fts_iterator_next( iterator, &key);
	  messtab_get_entry(this, &key, &mess_s, &mess_ac, &mess_at);

	  aa = (fts_array_t *)fts_malloc(sizeof(fts_array_t));
	  fts_array_init(aa, 1, &key);
	  
	  if(mess_s)
	    {
	      fts_set_symbol(&a, mess_s);
	      fts_array_append(aa, 1, &a);
	    }
	  
	  fts_array_append(aa, mess_ac, mess_at);

	  fts_set_list(&a, aa);
	  fts_array_set_element(array, size - 1, &a);
	}
    }
}

static void
messtab_restore_state_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  int i;

  messtab_clear(o, 0, 0, 0, 0);

  for(i=0; i<ac; i++)
    {
      fts_array_t *aa = fts_get_list(at + i);
      int aac = fts_array_get_size(aa);
      fts_atom_t *aat = fts_array_get_atoms(aa);

      messtab_store(this, aat, 0, aac - 1, aat + 1);
    }
}

static void
getmess_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  if(this->messtab)
    fts_object_release((fts_object_t *)this->messtab);
  
  this->messtab = (messtab_t *)fts_get_object(at);
  fts_object_refer((fts_object_t *)this->messtab);
}

static void
getmess_set_key(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  this->key = at[0];
}

static void
getmess_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;
  message_t *mess = messtab_recall(this->messtab, &this->key);

  if(mess)
    {
      this->messtab->locked = 1;
      message_output(o, 0, mess);
      this->messtab->locked = 0;
    }
}

static void
getmess_set_key_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_set_key(o, 0, 0, 1, at);
  getmess_output(o, 0, 0, 0, 0);
}

static void
putmess_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  messtab_store(this->messtab, &this->key, s, ac, at);
}

/**********************************************************
 *
 *  files
 *
 */
#define MESSTAB_ATOM_BUF_BLOCK_SIZE 64
#define MESSTAB_BLOCK_SIZE 64

static fts_atom_t *
messtab_atom_buf_realloc(fts_atom_t *buf, int size)
{
  fts_atom_t *new_buf = (fts_atom_t *)fts_realloc(buf, sizeof(fts_atom_t) * size); /* double size */

  return new_buf;
}

static void
messtab_atom_buf_free(fts_atom_t *buf, int size)
{
  if(buf)
    fts_free(buf);
}

static int 
messtab_import_from_coll(messtab_t *this, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int atoms_alloc = MESSTAB_ATOM_BUF_BLOCK_SIZE;
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

  atoms = messtab_atom_buf_realloc(atoms, atoms_alloc);

  messtab_clear((fts_object_t *)this, 0, 0, 0, 0);
  fts_set_void(&key);

  while (error == 0 && fts_atom_file_read(file, &a, &c))
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
		if(n != 0)
		  {
		    /* store message */
		    messtab_store(this, &key, 0, n, atoms);
		    
		    i++;
		    n = 0;

		    state = read_key;
		  }
		else
		  post("messtab: empty message found in coll file %s (ignored)\n", fts_symbol_name(file_name));
	      }
	    else
	      {
		/* read argument */
		if(n >= atoms_alloc)
		  {
		    atoms_alloc += MESSTAB_ATOM_BUF_BLOCK_SIZE;
		    atoms = messtab_atom_buf_realloc(atoms, atoms_alloc);
		  }
		
		atoms[n] = a;
		n++;
	      }
	  }
	  
	  break;
	}
    }
  
  if(error != 0)
    post("messtab: error reading coll file %s (%s)\n", fts_symbol_name(file_name), error);
  else if(state != read_key)
    {
      if(n > 0)
	{
	  messtab_store(this, &key, 0, n, atoms);
	  i++;
	}
      
      post("messtab: found unexpected ending in coll file %s\n", fts_symbol_name(file_name));
    }
  
  messtab_atom_buf_free(atoms, atoms_alloc);
  fts_atom_file_close(file);

  return i;
}

static int 
messtab_export_to_coll(messtab_t *this, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "w");
  fts_iterator_t iterators[2];
  fts_atom_t  a;
  int size = 0;
  int tab;
  int i;

  if(!file)
    return 0;

  fts_hashtable_get_keys(&this->table_int, iterators + 0);
  fts_hashtable_get_keys(&this->table_symbol, iterators + 1);

  for(tab=0; tab<2; tab++)
    {
      fts_iterator_t *iterator = iterators + tab;

      while ( fts_iterator_has_more( iterator) )
	{
	  fts_atom_t key;
	  message_t *mess;
	  fts_symbol_t mess_s;
	  int mess_ac;
	  const fts_atom_t *mess_at;
	  fts_atom_t a;
	  
	  fts_iterator_next( iterator, &key);
	  messtab_get_entry(this, &key, &mess_s, &mess_ac, &mess_at);
	  
	  /* write key */
	  fts_atom_file_write(file, &key, ' ');
	  
	  /* write comma */
	  fts_set_symbol(&a, fts_s_comma);
	  fts_atom_file_write(file, &a, ' ');
	  
	  /* write selector (if any) */
	  if(mess_s)
	    {
	      fts_set_symbol(&a, mess_s);
	      fts_atom_file_write(file, &a, ' ');
	    }
	  
	  /* write arguments */
	  for(i=0; i<mess_ac; i++)
	    fts_atom_file_write(file, mess_at + i, ' ');

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
messtab_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = messtab_import_from_coll(this, file_name);    
  else
    {
      post("messtab: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
      return;
    }

  if(size <= 0)
    post("messtab: can't import from file \"%s\"\n", fts_symbol_name(file_name));  
}

static void
messtab_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = messtab_export_to_coll(this, file_name);    
  else
    {
      post("messtab: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
      return;
    }

  if(size <= 0)
    post("messtab: can't export to file \"%s\"\n", fts_symbol_name(file_name));  
}

/**********************************************************
 *
 *  classes
 *
 */

static void
getmess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  ac--;
  at++;

  this->messtab = 0;
  fts_set_int(&this->key, 0);
  
  if(ac > 1)
    {
      if(fts_is_int(at) || fts_is_symbol(at))
	{
	  this->key = at[0];

	  /* skip key */
	  ac--;
	  at++;
	}
      else
	fts_object_set_error(o, "Wrong arguments");
    }

  if(ac > 0 && fts_is_a((at), messtab_symbol))
    {
      this->messtab = (messtab_t *)fts_get_object(at);
      fts_object_refer((fts_object_t *)this->messtab);
    }
  else
    fts_object_set_error(o, "First argument of messtab required");
}

static void
getmess_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  fts_object_release((fts_object_t *)this->messtab);
}

static void
messtab_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;
  int i;

  ac--;
  at++;

  fts_hashtable_init(&this->table_int, FTS_HASHTABLE_INT, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->table_int, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);

  this->locked = 0;

  for(i=0; i<ac; i++)
    {
      if(fts_is_list(at + i))
	{
	  fts_array_t *list = fts_get_list(at + i);
	  int lac = fts_array_get_size(list);
	  fts_atom_t *lat = fts_array_get_atoms(list);
	  
	  if(fts_is_int(lat) || fts_is_symbol(lat))
	    {
	      const fts_atom_t *key = lat;
	      
	      /* skip key */
	      lac--;
	      lat++;
	      
	      if(lac > 0)
		{
		  fts_symbol_t error = messtab_store(this, key, 0, lac, lat);

		  if(error)
		    {
		      messtab_clear(o, 0, 0, 0, 0);
		      fts_object_set_error(o, fts_symbol_name(error));
		    }
		}
	      else
		messtab_store(this, key, fts_s_bang, 0, 0);
	    }
	  else
	    {
	      messtab_clear(o, 0, 0, 0, 0);
	      fts_object_set_error(o, "Wrong key type in initialization");
	    }
	}
      else
	{
	  messtab_clear(o, 0, 0, 0, 0);
	  fts_object_set_error(o, "Wrong arguments");
	}
    }
}

static void
messtab_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messtab_t *this = (messtab_t *)o;

  messtab_clear(o, 0, 0, 0, 0);
}

/* when defining a variable: daemon for getting the property "state". */
static void
messtab_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messtab_t *this = (messtab_t *) obj;
  
  fts_set_object_with_type(value, (fts_object_t *)this, messtab_symbol);
}

static fts_status_t
messtab_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(messtab_t), 1, 2, 0);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, messtab_get_state);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messtab_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messtab_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("get_state_as_array"), messtab_get_state_as_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("restore_state_from_array"), messtab_restore_state_from_array);

  fts_method_define_varargs(cl, 0, fts_new_symbol("import"), messtab_import);
  fts_method_define_varargs(cl, 0, fts_new_symbol("export"), messtab_export);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("put"), messtab_put);
  fts_method_define_varargs(cl, 0, fts_new_symbol("get"), messtab_get);
  fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), messtab_clear);
  fts_method_define_varargs(cl, 0, fts_new_symbol("dump"), messtab_dump);
  
  return fts_Success;
}

static fts_status_t
getmess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getmess_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getmess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getmess_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, getmess_output);
  fts_method_define_varargs(cl, 0, fts_s_int, getmess_set_key_and_output);
  fts_method_define_varargs(cl, 0, fts_s_symbol, getmess_set_key_and_output);
  
  fts_method_define_varargs(cl, 1, messtab_symbol, getmess_set_reference);

  return fts_Success;
}

static fts_status_t
putmess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getmess_t), 3, 0, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getmess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getmess_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_anything, putmess_message);
  
  fts_method_define_varargs(cl, 1, fts_s_int, getmess_set_key);
  fts_method_define_varargs(cl, 1, fts_s_symbol, getmess_set_key);

  fts_method_define_varargs(cl, 2, messtab_symbol, getmess_set_reference);

  return fts_Success;
}

void
messtab_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_coll = fts_new_symbol("coll");

  messtab_symbol = fts_new_symbol("messtab");

  fts_class_install(messtab_symbol, messtab_instantiate);
  fts_class_install(fts_new_symbol("getmess"), getmess_instantiate);
  fts_class_install(fts_new_symbol("putmess"), putmess_instantiate);
}
