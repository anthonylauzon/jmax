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

/******************************************************
 *
 *  dumpfile
 *
 */

static fts_symbol_t sym_comment = 0;

typedef struct 
{
  fts_dumper_t super;
  enum {dumpfile_closed, dumpfile_opened_read, dumpfile_opened_write} status;
  fts_atomfile_t *file;
  int block; /* writing message block */
  fts_symbol_t class; /* restoring class */
} dumpfile_t;

static void
dumpfile_dumper_send(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;
  fts_atom_t a;
  int i;

  if(this->block)
    {
      /* write comma after last dump message while in block */
      fts_set_symbol(&a, fts_s_comma);
      fts_atomfile_write(this->file, &a, '\n');
    }
  
  /* writing block */
  this->block = 1;
      
  /* write selector */
  fts_set_symbol(&a, s);
  fts_atomfile_write(this->file, &a, ' ');

  /* write args */
  for(i=0; i<ac; i++)
    fts_atomfile_write(this->file, at + i, ' ');
}

static fts_symbol_t
dumpfile_read_class_comment(dumpfile_t *this)
{
  fts_atom_t a;
  char c;

  this->class = 0;

  while(fts_atomfile_read(this->file, &a, &c))
    {
      if(fts_is_symbol(&a) && fts_get_symbol(&a) == sym_comment)
	break;
    }
  
  if(fts_atomfile_read(this->file, &a, &c) && fts_is_symbol(&a))
    this->class = fts_get_symbol(&a);

  while(c != '\n')
    {
      if(!fts_atomfile_read(this->file, &a, &c))
	break;
    }

  if(c != '\n')
    return 0;
  
  return this->class;
}

static void
dumpfile_close(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;

  if(this->file)
    {
      fts_atomfile_close(this->file);
      this->file = 0;
      this->class = 0;
      this->status = dumpfile_closed;
    }
}

static void
dumpfile_open(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;
  
  if(this->status != dumpfile_closed)
    dumpfile_close(o, 0, 0, 0, 0);

  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t name = fts_get_symbol(at);
      fts_symbol_t mode = fts_s_read;
      
      if(ac > 1 && fts_is_symbol(at + 1))
	mode = fts_get_symbol(at + 1);
      
      if(mode == fts_s_read)
	{
	  this->file = fts_atomfile_open_read(name);
	  
	  if(dumpfile_read_class_comment(this))
	    this->status = dumpfile_opened_read;
	  else
	    dumpfile_close(o, 0, 0, 0, 0);
	}
      else if(mode == fts_s_write)
	{
	  this->file = fts_atomfile_open_write(name);
	  this->status = dumpfile_opened_write;
	}
      else
	{
	  fts_object_error(o, "unknown mode for open: %s", mode);
	  return;
	}

      if(!this->file)
	{
	  char *str = (mode == fts_s_read)? "reading": "writing";

	  fts_object_error(o, "cannot open file %s for %s", name, str);
	  this->status = dumpfile_closed;
	}
    }
  else
    fts_object_error(o, "bad arguments for message open");
}

static void
dumpfile_dump_object(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;

  if(this->status == dumpfile_opened_write)
  {
    if(ac > 0 && fts_is_object(at))
    {
      fts_object_t *object = fts_get_object(at);
      fts_class_t *class = fts_object_get_class(object);
      fts_symbol_t class_name = fts_class_get_name(class);
      fts_method_t meth_dump_state = fts_class_get_method_varargs(class, fts_s_dump_state);
      
      if(meth_dump_state)
	    {
	      fts_atom_t a;
	      
	      /* write class comment */
	      fts_set_symbol(&a, sym_comment);
	      fts_atomfile_write(this->file, &a, ' ');
        
	      fts_set_symbol(&a, class_name);
	      fts_atomfile_write(this->file, &a, '\n');
        
	      /* set dumpfile as dumper */
	      fts_set_object(&a, (fts_object_t *)o);
	      (*meth_dump_state)(object, fts_s_dump_state, 1, &a, fts_nix);
	      
	      /* write final semicolon */
	      fts_set_symbol(&a, fts_s_semi);
	      fts_atomfile_write(this->file, &a, '\n');	  
        
	      /* reset flag */
	      this->block = 0;
	    }
      else
        fts_object_error(o, "cannot dump object of class %s", class_name);
    }
    else
      fts_object_error(o, "object argument required for message dump");
  }
  else
    fts_object_error(o, "file not opened in write mode (cannot dump)");
}

static void
dumpfile_restore_object(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;

  if(this->status == dumpfile_opened_read)
    {
      if(ac > 0 && fts_is_object(at))
	{
	  fts_object_t *object = fts_get_object(at);
	  fts_message_t *mess = fts_dumper_get_message(&this->super);
	  fts_atom_t a;
	  char c;
	  
	  if(this->class != fts_class_get_name(fts_object_get_class(object)))
	    {
	      /* wrong class */
	      fts_object_error(o, "%s object exspected for restore", this->class);
	      return;
	    }

	  /* clear object */
	  fts_send_message(object, fts_s_clear, 0, 0, fts_nix);
	  
	  /* reset message */
	  fts_message_set(mess, 0, 0, 0);
	  
	  /* read message block */
	  while(fts_atomfile_read(this->file, &a, &c))
	    {
	      if(fts_is_symbol(&a))
		{
		  fts_symbol_t sym = fts_get_symbol(&a);
		  
		  if(sym == fts_s_comma)
		    {
		      if(fts_message_get_selector(mess) == 0)
			{
			  /* empty message */
			  fts_object_error(o, "file format error in restore (empty message)");
			  dumpfile_close((fts_object_t *)this, 0, 0, 0, 0);
			  return;
			}
		      else
			{
			  int mess_ac;
			  const fts_atom_t *mess_at;
			  
			  /* send message */
			  mess_ac = fts_message_get_ac(mess);
			  mess_at = fts_message_get_at(mess);
			  
			  fts_send_message(object, fts_message_get_selector(mess), mess_ac, mess_at, fts_nix);
			  
			  /* reset message */
			  fts_message_set(mess, 0, 0, 0);
			}
		    }
		  else if(sym == fts_s_semi)
		    {
		      /* end of message block */
		      break;
		    }
		  else if(fts_message_get_selector(mess) == 0)
		    /* read selector */
		    fts_message_set(mess, fts_get_symbol(&a), 0, 0);
		  else
		    /* read symbol argument */
		    fts_message_append(mess, 1, &a);
		}
	      else if(fts_message_get_selector(mess) == 0)
		{
		  /* argument without selector */
		  fts_object_error((fts_object_t *)this, "file format error in restore (missing selector)");
		  dumpfile_close((fts_object_t *)this, 0, 0, 0, 0);
		  return;
		}
	      else
		/* read argument */
		fts_message_append(mess, 1, &a);
	    }
	  
	  /* eof is valid terminator (as semicolon)  */
	  if(fts_message_get_selector(mess) != 0)
	    {
	      int mess_ac;
	      const fts_atom_t *mess_at;
	      
	      /* send last message */
	      mess_ac = fts_message_get_ac(mess);
	      mess_at = fts_message_get_at(mess);
	      
	      fts_send_message(object, fts_message_get_selector(mess), mess_ac, mess_at, fts_nix);
	    }

	  /* read next class name */
	  if(!dumpfile_read_class_comment(this))
	    dumpfile_close(o, 0, 0, 0, 0);
	}
      else
	fts_object_error(o, "object argument required for restore");
    }
  else
    fts_object_error(o, "file not opened in read mode (cannot restore)");
}

/******************************************************
 *
 *  class
 *
 */

static void
dumpfile_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;

  /* init super */
  fts_dumper_init((fts_dumper_t *)this, dumpfile_dumper_send);

  this->file = 0;
  this->status = dumpfile_closed;
  this->block = 0;
  this->class = 0;
}

static void
dumpfile_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dumpfile_t *this = (dumpfile_t *)o;

  /* delete super */
  fts_dumper_destroy((fts_dumper_t *)this);
}

static void
dumpfile_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dumpfile_t), dumpfile_init, dumpfile_delete);

  fts_class_message_varargs(cl, fts_new_symbol("dump"), dumpfile_dump_object); 

  fts_class_message_varargs(cl, fts_s_open, dumpfile_open);
  fts_class_message_varargs(cl, fts_s_close, dumpfile_close);

  fts_class_message_varargs(cl, fts_new_symbol("restore"), dumpfile_restore_object);
}

FTS_MODULE_INIT(dumpfile)
{
  sym_comment = fts_new_symbol("#");

  fts_class_install(fts_new_symbol("dumpfile"), dumpfile_instantiate);
}
