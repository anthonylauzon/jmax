/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and ditributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "mt.h"

static fts_symbol_t sym_list = 0;
static fts_symbol_t sym_null = 0;
static fts_symbol_t sym_comma = 0;

fts_type_t message_table_type = 0;

/**********************************************************
 *
 *  dynamic messages
 *
 */

void
message_init(message_t *mess, fts_symbol_t s, int ac, fts_atom_t *at)
{
  mess->s = s;

  if(ac > 0)
    {
      int i;

      mess->at = (fts_atom_t *)fts_block_alloc(sizeof(fts_atom_t) * ac);
      mess->ac = ac;

      /* copy atoms */
      for(i=0; i<ac; i++)
	mess->at[i] = at[i];
    }
  else
    {
      mess->at = 0;
      mess->ac = 0;
    }
}

void
message_clear(message_t *mess)
{
  mess->s = 0;

  if(mess->ac)
    {
      fts_block_free(mess->at, sizeof(fts_atom_t) * mess->ac);
  
      mess->ac = 0;
      mess->at = 0;
    }
}

/* clear and init */
void
message_set(message_t *mess, fts_symbol_t s, int ac, fts_atom_t *at)
{
  int i;

  mess->s = s;

  if(mess->ac)
    fts_block_free(mess->at, sizeof(fts_atom_t) * mess->ac);
  
  if(ac > 0)
    {
      mess->at = (fts_atom_t *)fts_block_alloc(sizeof(fts_atom_t) * ac);
      mess->ac = ac;

      /* copy atoms */
      for(i=0; i<ac; i++)
	mess->at[i] = at[i];
    }
  else
    {
      mess->at = 0;
      mess->ac = 0;
    }
}

void
message_move(message_t *from, message_t *to)
{
  if(to->ac)
    fts_block_free(to->at, sizeof(fts_atom_t) * to->ac);

  to->s = from->s;
  to->ac = from->ac;
  to->at = from->at;
  
  from->s = 0;
  from->ac = 0;
  from->at = 0;
}

/**********************************************************
 *
 *  message table
 *
 */

static void
message_table_init(message_table_t *mt, int size)
{
  if(size)
    {
      mt->mess = fts_block_zalloc(sizeof(message_t) * size); /* alloc and init! */
      mt->size = size;
      mt->alloc = size;
    }
  else
    {
      mt->mess = 0;
      mt->size = 0;
      mt->alloc = 0;
    }

  mt->refcnt = 0;
}

message_table_t *
message_table_new(int size)
{
  message_table_t *mt = fts_malloc(sizeof(message_table_t));

  message_table_init(mt, size);

  return mt;
}

void
message_table_clear(message_table_t *mt)
{
  int i;
  
  for(i=0; i<mt->size; i++)
    message_clear(mt->mess + i);
  
  mt->size = 0;
}

void
message_table_delete(message_table_t *mt)
{
  message_table_clear(mt);

  if(mt->alloc)
    fts_block_free(mt->mess, sizeof(message_t) * mt->alloc);
}

void
message_table_set_size(message_table_t *mt, int size)
{
  if(size > mt->alloc)
    {
      int i;
      message_t *old_tab = mt->mess;

      mt->mess = fts_block_zalloc(sizeof(message_t) * size);

      /* copy the old content */
      for(i=0; i<mt->size; i++)
	mt->mess[i] = old_tab[i];
      
      fts_block_free(old_tab, sizeof(message_t) * mt->alloc);

      mt->size = size;
      mt->alloc = size;
    }
  else
    {
      int i;
      
      for(i=size; i<mt->size; i++)
	message_clear(mt->mess + i);
	  
      mt->size = size;
    }
}

void
message_table_refer(message_table_t *mt)
{
  mt->refcnt++;  
}

void
message_table_release(message_table_t *mt)
{
  mt->refcnt--;

  if(mt->refcnt == 0)
    message_table_delete(mt);
}

/**********************************************************
 *
 *  files
 *
 */

#define MESSAGE_TABLE_ATOM_BUF_BLOCK_SIZE 64
#define MESSAGE_TABLE_BLOCK_SIZE 64

static fts_atom_t *
message_table_atom_buf_new(int size)
{
  fts_atom_t *buf = (fts_atom_t *)fts_block_zalloc(sizeof(fts_atom_t) * size);
  return buf;
}

static void
message_table_atom_buf_free(fts_atom_t *buf, int size)
{
  if(buf)
    fts_block_free(buf, sizeof(fts_atom_t) * size);
}

static fts_atom_t *
message_table_atom_buf_grow(fts_atom_t *buf, int size, int more)
{
  int new_size = size + more;
  fts_atom_t *new_buf = (fts_atom_t *)fts_block_zalloc(sizeof(fts_atom_t) * size); /* double size */
  int i;
  
  for(i=0; i<size; i++)
    new_buf[i] = buf[i];

  fts_block_free(buf, sizeof(fts_atom_t) * size);

  return new_buf;
}

int 
message_table_file_import_ascii(message_table_t *mt, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  fts_atom_t a;
  char c;
  int i, j;
  fts_symbol_t s = 0;
  int ac = 0;
  int at_alloc = MESSAGE_TABLE_ATOM_BUF_BLOCK_SIZE;
  fts_atom_t *at;

  file = fts_atom_file_open(fts_symbol_name(file_name), "r");

  if(!file)
    {
      post("message_table: can't open file to read: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  message_table_clear(mt);
  at = message_table_atom_buf_new(at_alloc);

  i = 0;
  j = 0;
  while (fts_atom_file_read(file, &a, &c))
    {
      if (fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
	{
	  /* next row */

	  if(i >= mt->alloc)
	    message_table_set_size(mt, mt->alloc + MESSAGE_TABLE_BLOCK_SIZE);
	  
	  message_set(mt->mess + i, s, ac, at);
	  
	  i++;
	  j = 0;
	  ac = 0;
	}
      else
	{
	  if(j >= at_alloc)
	    at = message_table_atom_buf_grow(at, at_alloc, MESSAGE_TABLE_ATOM_BUF_BLOCK_SIZE);
	  
	  if(j == 0)
	    {
	      if(fts_is_symbol(&a))
		s = fts_get_symbol(&a);
	      else
		{
		  s = sym_list;
		  at[0] = a;
		  ac++;
		}
	    }
	  else
	    {
	      at[j] = a;
	      ac++;
	      j++;
	    }
	}
    }

  message_table_atom_buf_free(at, at_alloc);
  fts_atom_file_close(file);

  mt->size = i;

  return i;
}

int 
message_table_file_export_ascii(message_table_t *mt, fts_symbol_t file_name)
{
  fts_atom_t  a;
  fts_atom_file_t *file;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    {
      post("message_table: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  for(i=0; i<mt->size; i++)
    {
      message_t *mess = mt->mess + i;

      /* write selector if needed */
      if(mess->s && (mess->s != sym_list || fts_is_symbol(mess->at)))
	{
	  fts_set_symbol(&a, mess->s);
	  fts_atom_file_write(file, &a, ' ');
	}

      /* write atoms with space */
      for(j=0; j<mess->ac; j++)	
	fts_atom_file_write(file, mess->at + j, ' ');

      /* comma and new line */
      fts_set_symbol(&a, sym_comma);
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return i;
}

void
message_table_config(void)
{
  sym_list = fts_new_symbol("list");
  sym_null = fts_new_symbol("");
  sym_comma = fts_new_symbol(",");

  message_table_type = fts_new_symbol("message_table");
}
