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
#include <fts/packages/data/data.h>

fts_symbol_t dict_symbol = 0;
fts_class_t *dict_type = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_coll = 0;
static fts_symbol_t sym_remove_entries = 0;

#define dict_set_editor_open(m) ((m)->opened = 1)
#define dict_set_editor_close(m) ((m)->opened = 0)
#define dict_editor_is_open(m) ((m)->opened)

/* 
 *  data write access functions 
 */

/* store one key-value pair */
void
dict_store(dict_t *dict, const fts_atom_t *key, const fts_atom_t *atom)
{
  fts_atom_t a;

  /* remove old entry for same key */
  if(fts_hashtable_get(&dict->hash, key, &a))
    fts_atom_void(&a);  
  else
    fts_set_void(&a);

  fts_atom_assign(&a, atom);

  /* insert atom to hashtable */
  fts_hashtable_put(&dict->hash, key, &a);
}


/* store tuple of values in at under key */
void
dict_store_atoms(dict_t *dict, const fts_atom_t *key, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    dict_store(dict, key, at);
  else if(ac > 1)
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      dict_store(dict, key, &a);
    }
}


/* store list of key-value pairs */
void
dict_store_list (dict_t *dict, int ac, const fts_atom_t *at)
{
  int i;

  ac &= -2;	/* round down to even number (drop last bit) */

  for (i = 0; i < ac; i += 2)
  {
    if (fts_is_int(at + i)  ||  fts_is_symbol(at + i))
      dict_store(dict, at + i, at + i + 1);
    else
      fts_object_error((fts_object_t *) dict, "set: wrong key type for arg %d", i);
  }
}

void 
dict_get(dict_t *dict, const fts_atom_t *key, fts_atom_t *atom)
{
  if(!fts_hashtable_get(&dict->hash, key, atom))
    fts_set_void(atom);
}

static void
dict_remove(dict_t *dict, const fts_atom_t *key)
{
  fts_atom_t value;
  
  if(fts_hashtable_get(&dict->hash, key, &value))
    {
      fts_atom_void(&value);      
      fts_hashtable_remove(&dict->hash, key);
    }
}

static void
dict_remove_all(dict_t *dict)
{
  fts_iterator_t iterator;

  fts_hashtable_get_values(&dict->hash, &iterator);

  while(fts_iterator_has_more(&iterator))
  {
    fts_atom_t value;

    fts_iterator_next(&iterator, &value);
    fts_atom_void(&value);
  }

  fts_hashtable_clear(&dict->hash);
}

static void
dict_copy(dict_t *org, dict_t *copy)
{
  fts_iterator_t key_iterator;
  fts_iterator_t value_iterator;

  dict_remove_all(copy);
  
  /* iterate on org hash table */
  fts_hashtable_get_keys(&org->hash, &key_iterator);
  fts_hashtable_get_values(&org->hash, &value_iterator);

  while(fts_iterator_has_more(&key_iterator))
  {
    fts_atom_t key;
    fts_atom_t value;
    fts_atom_t value_copy;

    /* get key */
    fts_iterator_next(&key_iterator, &key);
    fts_iterator_next(&value_iterator, &value);

    /* copy entry */
    fts_atom_copy(&value, &value_copy);
    fts_atom_refer(&value_copy);

    /* store entry to copy hash table */
    fts_hashtable_put(&copy->hash, &key, &value_copy);
  }
}

static void
dict_copy_function(const fts_object_t *from, fts_object_t *to)
{
  dict_copy((dict_t *)from, (dict_t *)to);
}

static void
dict_post_function(fts_object_t *o, fts_bytestream_t *stream)
{
  fts_spost(stream, "<dict>");
}

static void
dict_array_function(fts_object_t *o, fts_array_t *array)
{
  dict_t *self = (dict_t *)o;
  fts_iterator_t keys_iterator;
  fts_iterator_t values_iterator;
  
  fts_hashtable_get_keys(&self->hash, &keys_iterator);
  fts_hashtable_get_values(&self->hash, &values_iterator);
  
  while(fts_iterator_has_more(&keys_iterator))
  {
    fts_atom_t key, value;
    
    fts_iterator_next(&keys_iterator, &key);
    fts_iterator_next(&values_iterator, &value);
    
    fts_array_append(array, 1, &key);
    fts_array_append(array, 1, &value);
  }
}
/********************************************************************
*
*   upload methods
*
*/
#define DICT_CLIENT_BLOCK_SIZE 128

static fts_memorystream_t *dict_memory_stream ;

static fts_memorystream_t * dict_get_memory_stream()
{
  if(!dict_memory_stream)
    dict_memory_stream = (fts_memorystream_t *)fts_object_create(fts_memorystream_class, 0, 0);
  
  return dict_memory_stream;
}

static void 
dict_upload_size(dict_t *self)
{
  fts_atom_t a[2];
  int m = fts_hashtable_get_size(&self->hash);
  int n = 2;
  
  fts_set_int(a, m);
  fts_set_int(a+1, n);
  fts_client_send_message((fts_object_t *)self, fts_s_size, 2, a);
}

static void 
dict_upload_data(dict_t *self)
{  
  fts_array_t array;
  fts_atom_t *atoms;
  fts_atom_t *d;
  fts_atom_t a[DICT_CLIENT_BLOCK_SIZE];
  int size, data_size, ms, ns, sent;
  fts_memorystream_t *stream = dict_get_memory_stream();
  
  fts_array_init(&array, 0, 0);
  dict_array_function((fts_object_t *)self, &array);
  atoms = fts_array_get_atoms(&array);
  size = fts_array_get_size(&array);
  
  data_size = size;
  ms = 0;
  ns = 0;
  sent = 0;
  while( data_size > 0)
  {
    int i = 0;
    int n = (data_size > DICT_CLIENT_BLOCK_SIZE-2)? DICT_CLIENT_BLOCK_SIZE-2: data_size;
    
    if( sent)
    {
      ms = sent/2;
      ns = sent - ms*2;
    }
    fts_set_int(a, ms);
    fts_set_int(a+1, ns);
    
    for(i=0; i<n; i++)
    {
      d = atoms + sent + i;
      
      if(fts_is_object(d))
      {
        fts_atom_t b[3];
        fts_object_t *dobj = fts_get_object(d);
        
        if(fts_object_has_client(dobj) == 0)
          fts_client_register_object(dobj, fts_object_get_client_id((fts_object_t *)self));	
        
        fts_set_int(b, fts_object_get_id(dobj));
        fts_set_symbol(b+1, fts_object_get_class_name(dobj));
        
        fts_memorystream_reset(stream);
        fts_spost_object((fts_bytestream_t *)stream, dobj);
        fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
        fts_set_symbol(b+2,  fts_new_symbol((char *)fts_memorystream_get_bytes( stream)));
        
        fts_client_send_message((fts_object_t *)self, fts_s_register_object, 3, b);
        
        fts_send_message(dobj, fts_s_update_gui, 0, 0);
        
        fts_set_object(&a[2+i], dobj);
      }
      else
        fts_atom_copy(d, &a[2+i]);
    }
    fts_client_send_message((fts_object_t *)self, fts_s_set, n+2, a);
    
    sent += n;
    data_size -= n;
  }
}

static void
dict_upload(dict_t *self)
{
  dict_upload_size(self);
  dict_upload_data(self);
}

/**********************************************************
 *
 *  user methods
 *
 */

static void
dict_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  
  dict_remove_all(self);
  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */

  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
dict_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  
  dict_store_list(self, ac, at);
  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */

  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
dict_remove_entry(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  
  dict_remove(self, at);
  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */

  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
dict_remove_entries(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  int i = 0;
  
  for(i = 0; i<ac; i++)
    dict_remove(self, at+i);
  
  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */
  
  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
_dict_get_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  fts_atom_t a;
  
  if(fts_hashtable_get(&self->hash, at, &a))
    fts_return(&a);
  else if(fts_is_symbol(at))
    fts_object_error(o, "no entry for %s", fts_symbol_name(fts_get_symbol(at)));
  else if(fts_is_int(at))
    fts_object_error(o, "no entry for %d", fts_get_int(at));
}

static void
dict_set_from_dict(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;

  dict_copy((dict_t *)fts_get_object(at), self);

  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */

  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
dict_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_iterator_t key_iterator, value_iterator;

  fts_hashtable_get_keys(&self->hash, &key_iterator);
  fts_hashtable_get_values(&self->hash, &value_iterator);

  while(fts_iterator_has_more(&key_iterator))
  {
    fts_atom_t key, value;

    fts_iterator_next(&key_iterator, &key);
    fts_iterator_next(&value_iterator, &value);

    if(fts_is_tuple(&value))
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
dict_import_from_coll(dict_t *self, fts_symbol_t file_name)
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

  dict_remove_all(self);
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
			  dict_store_atoms(self, &key, n - 1, atoms + 1);
		      }
		    else
		      dict_store_atoms(self, &key, n, atoms);

		    i++;
		    n = 0;

		    state = read_key;
		  }
		else
		  fts_post("dict: empty message found in coll file %s (ignored)\n", fts_symbol_name(file_name));
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
    fts_post("dict: error reading coll file %s (%s)\n", fts_symbol_name(file_name), error);
  else if(state != read_key)
    {
      if(n > 0)
	{
	  dict_store_atoms(self, &key, n, atoms);
	  i++;
	}
      
      fts_post("dict: found unexpected ending in coll file %s\n", fts_symbol_name(file_name));
    }
  
  dict_atom_buf_free(atoms, atoms_alloc);
  fts_atom_file_close(file);

  return i;
}

static int 
dict_export_to_coll(dict_t *self, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "w");
  fts_iterator_t key_iterator;
  fts_iterator_t value_iterator;
  int size = 0;
  int i;

  if(!file)
    return 0;

  fts_hashtable_get_keys(&self->hash, &key_iterator);
  fts_hashtable_get_values(&self->hash, &value_iterator);

  while(fts_iterator_has_more(&key_iterator))
  {
    fts_atom_t key, value;
    fts_symbol_t s = NULL;
    int ac = 0;
    const fts_atom_t *at = NULL;
    fts_atom_t a;

    fts_iterator_next(&key_iterator, &key);
    fts_iterator_next(&value_iterator, &value);

    if(fts_is_tuple(&value))
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

  fts_atom_file_close(file);

  return size;
}

static void
dict_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = dict_import_from_coll(self, file_name);    
  else
    {
      fts_post("dict: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
      return;
    }

  if(size <= 0)
    fts_post("dict: can't import from file \"%s\"\n", fts_symbol_name(file_name));

  fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */

  if(dict_editor_is_open(self))
    dict_upload(self);
}

static void
dict_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_coll);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_coll)
    size = dict_export_to_coll(self, file_name);    
  else
    {
      fts_post("dict: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
      return;
    }

  if(size <= 0)
    fts_post("dict: can't export to file \"%s\"\n", fts_symbol_name(file_name));  
}

static void
dict_get_keys(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  fts_iterator_t iterator;

  fts_hashtable_get_keys(&self->hash, &iterator);

  while(fts_iterator_has_more(&iterator))
  {
    fts_atom_t key;

    fts_iterator_next(&iterator, &key);
    fts_tuple_append(tuple, 1, &key);
  }

  fts_return_object((fts_object_t *)tuple);
}

static void
dict_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  int size = fts_hashtable_get_size(&self->hash);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(size == 0)
    fts_spost(stream, "<empty dictionary>\n");
  else
  {
    fts_iterator_t key_iterator;
    fts_iterator_t value_iterator;

    if(size == 1)
      fts_spost(stream, "<dictionary of 1 entry>\n");
    else
      fts_spost(stream, "<dictionary of %d entries>\n", size);

    fts_spost(stream, "{\n");

    fts_hashtable_get_keys(&self->hash, &key_iterator);
    fts_hashtable_get_values(&self->hash, &value_iterator);

    while(fts_iterator_has_more(&key_iterator))
    {
      fts_atom_t key, value;

      fts_iterator_next(&key_iterator, &key);
      fts_iterator_next(&value_iterator, &value);

      if(fts_is_int(&key))
        fts_spost(stream, "  %d: ", fts_get_int(&key));
      else
        fts_spost(stream, "  %s: ", fts_symbol_name(fts_get_symbol(&key)));

      fts_spost_atoms(stream, 1, &value);
      fts_spost(stream, "\n");
    }

    fts_spost(stream, "}\n");
  }
}

static void
dict_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  
  dict_set_editor_open(self);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  dict_upload(self);
}

static void 
dict_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *) o;
  
  if(dict_editor_is_open(self))
  {
    dict_set_editor_close(self);
    fts_client_send_message(o, fts_s_closeEditor, 0, 0);  
  }
}

static void
dict_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  
  dict_set_editor_close(self);
}

/**********************************************************
 *
 *  class
 *
 */

static void
dict_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;
  int i;
  
  ac &= -2;	/* round down to even number (drop last bit) */
  
  fts_hashtable_init(&self->hash, FTS_HASHTABLE_SMALL);
  
  for(i=0; i<ac; i+=2)
  {
    if(fts_is_int(at + i) || fts_is_symbol(at + i))
      dict_store(self, at + i, at + i + 1);
    else
    {
      dict_remove_all(self);
      fts_object_error(o, "wrong key type in initialization");
    }
    
  }
}


static void
dict_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dict_t *self = (dict_t *)o;

  dict_remove_all(self);
  fts_hashtable_destroy(&self->hash);
}


static void
dict_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dict_t), dict_init, dict_delete);
  
  fts_class_set_copy_function(cl, dict_copy_function);
  fts_class_set_post_function(cl, dict_post_function);
  fts_class_set_array_function(cl, dict_array_function);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, dict_dump_state);

  fts_class_message_varargs(cl, fts_s_set_from_instance, dict_set_from_dict);
  fts_class_message_varargs(cl, fts_new_symbol("keys"), dict_get_keys);

  fts_class_message_varargs(cl, fts_s_print, dict_print);
  
  fts_class_message_varargs(cl, fts_s_import, dict_import);
  fts_class_message_varargs(cl, fts_s_export, dict_export);
  
  fts_class_message_void(cl, fts_s_clear, dict_clear);

  fts_class_message_varargs(cl, fts_s_set, dict_set);
  fts_class_message_number(cl, fts_s_set, dict_remove_entry);
  fts_class_message_symbol(cl, fts_s_set, dict_remove_entry);
  
  fts_class_message_number(cl, fts_s_remove, dict_remove_entry);
  fts_class_message_symbol(cl, fts_s_remove, dict_remove_entry);
  fts_class_message_varargs(cl, sym_remove_entries, dict_remove_entries);

  fts_class_message_number(cl, fts_s_get_element, _dict_get_element);
  fts_class_message_symbol(cl, fts_s_get_element, _dict_get_element);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_message_varargs(cl, fts_s_openEditor, dict_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, dict_close_editor); 
  fts_class_message_varargs(cl, fts_s_destroyEditor, dict_destroy_editor);  
  
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);

  fts_class_doc(cl, dict_symbol, "[<sym|int: key> <any: value> ...]", "dictionary");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all entries");
  fts_class_doc(cl, fts_s_set, "<sym|int: key> <any: value> ...", 
			       "set list of key-value pairs");
  fts_class_doc(cl, fts_s_remove, "<sym|int: key>", "remove entry");
  fts_class_doc(cl, fts_s_print, NULL, "print list of entries");
}


void
dict_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_coll = fts_new_symbol("coll");
  sym_remove_entries = fts_new_symbol("remove_entries");
  dict_symbol = fts_new_symbol("dict");

  dict_type = fts_class_install(dict_symbol, dict_instantiate);
}
