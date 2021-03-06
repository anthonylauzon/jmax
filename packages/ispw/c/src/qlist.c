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


/*
 * 
 * Modified by MDC: "__" behaviour deleted (not documented and shadock).
 * Also, in the old code "drop" did not worked with messages to the outlet
 * now they do.
 *
 */

#include <fts/fts.h>
#include <ftsprivate/client.h>
#include <ftsprivate/patcher.h>
#include "qlist.h"
#include "atomlist.h"
#include "naming.h"

fts_symbol_t sym_setAtomList = 0;

typedef struct _qlist_
{
  fts_object_t  ob;

  int open; /* flag: is 1 if sequence editor is open */

  fts_atom_list_t *atom_list;
  fts_atom_list_iterator_t *iterator;

} qlist_t;


#define qlist_set_editor_open(q) ((q)->open = 1)
#define qlist_set_editor_close(q) ((q)->open = 0)
#define qlist_editor_is_open(q) ((q)->open != 0)

/* Method for message "rewind" inlet 0 */

static void
qlist_rewind(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_iterator_init(this->iterator, this->atom_list);
}


/* Qlist NEXT version translated directly one-to-one from the
   original Miller Puckette code; no intention to understand
   or debug it */

#define NATOM 256

static void
qlist_next(fts_object_t *o, fts_symbol_t s, int aac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;
  long drop = fts_get_int_arg(aac, at, 0, 0);
  fts_atom_t av[NATOM];
  fts_atom_t *ap;
  int ac;
  fts_object_t *target = 0;
  
  while (! fts_atom_list_iterator_end(this->iterator))
    {
      int is_comma = 0;

      av[0] = *fts_atom_list_iterator_current(this->iterator);
      fts_atom_list_iterator_next(this->iterator);

      if (!target)
	{
	  if (fts_is_number(av))
	    {
	      fts_atom_t waka[11];
	      fts_atom_t *wp = waka+1;
	      long count = 1;
	      waka[0] = *av;
	      
	      while (1)
		{
		  *wp = *fts_atom_list_iterator_current(this->iterator);

		  if ((! fts_is_int(wp)) && (! fts_is_float(wp)))
		    break;

		  fts_atom_list_iterator_next(this->iterator);
                  
		  if (count < 11)
                  {
                    count++;
                    wp++;
                  }
		}

              if (count > 10)
                count = 10;

              fts_outlet_varargs(o, 0, count, waka);

              break;
	    }
	}


      for (ac = 0, ap = av; !fts_atom_list_iterator_end(this->iterator); ac++)
	{
	  if (fts_is_symbol(ap))
	    {
	      if (fts_get_symbol(ap) == fts_s_semi)
		break;

	      if (fts_get_symbol(ap) == fts_s_comma)
		{
		  is_comma = 1;
		  break;
		}
	    }

	  *(++ap) = *(fts_atom_list_iterator_current(this->iterator));

	  fts_atom_list_iterator_next(this->iterator);
	}

      ap = av;

      if (!target)
	{
	  if (fts_is_symbol(av)  && 
	      (fts_get_symbol(av) != fts_s_semi) &&
	      (fts_get_symbol(av) != fts_s_comma))
	    {
	      fts_symbol_t target_name = fts_get_symbol(av);

	      target = ispw_get_target(fts_object_get_patcher(o), target_name);

	      if (!target)
		{
		  fts_post("qlist: %s: no such object\n", fts_get_symbol(av));
		  continue;
		}

	      ap++;
	      ac--;

	      if (! ac)
		continue;
	    }
	  else
	    continue;
	}

      if (!drop)
	{
	  if (fts_is_number(ap))
	    ispw_target_send(target, NULL, ac, ap);
	  else if (fts_is_symbol(ap) && (fts_get_symbol(ap) != fts_s_semi) && (fts_get_symbol(ap) != fts_s_comma))
          {
            fts_symbol_t selector = fts_get_symbol(ap);
            
            if(selector == fts_s_bang)
              ispw_target_send(target, NULL, 0, NULL);
            else
              ispw_target_send(target, selector, ac - 1, ap + 1);              
          }
	}

      if (!is_comma)
	target = 0;
    }
}



/* Method for message "append" [<arg>*] inlet 0 */

static void
qlist_append(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_append(this->atom_list, ac, at);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message_varargs((fts_object_t *)this->atom_list, sym_atomlist_update, 0, 0);

  fts_object_set_dirty(o);
}

/* Method for message "set" [<arg>*] inlet 0 */

static void
qlist_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_set( this->atom_list, ac, at);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message_varargs((fts_object_t *)this->atom_list, sym_atomlist_update, 0, 0);

  fts_object_set_dirty(o);
}

/* Method for message "clear" inlet 0 */

static void
qlist_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_clear(this->atom_list);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message_varargs((fts_object_t *)this->atom_list, sym_atomlist_update, 0, 0);

  fts_object_set_dirty(o);
}

/* Method for message "flush" inlet 0 */
/* code to flush a qlist up from CP to host */

static void
qlist_flush(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  /* Not Yet implemented (will it ever be ??) */
}

/* Method for message "read" <sym> inlet 0 */

static void
qlist_read(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_post("qlist_read not implemented (yet)\n");
}


/* for the moment, the symbol name (optional second argument) is ignored (what it is, anyway ?) */

static void
qlist_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;
  fts_atom_t a[1];
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);

  this->open = 0;
 
  if(name)
    {
      fts_set_symbol(a, name);
      this->atom_list = (fts_atom_list_t *)fts_object_create(atomlist_type, 1, a);
    }
  else
    this->atom_list = (fts_atom_list_t *)fts_object_create(atomlist_type, 0, 0);

  this->iterator  = fts_atom_list_iterator_new(this->atom_list);
  
  fts_object_set_persistence(o, 1);
}


static void
qlist_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  fts_send_message_varargs((fts_object_t *)this->atom_list, fts_s_delete, 0, 0);
  fts_atom_list_iterator_free(this->iterator);

  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
}

static void
qlist_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;
  fts_atom_t a[1];

  if(!fts_object_has_client((fts_object_t *)this->atom_list))
    fts_client_register_object((fts_object_t *)this->atom_list, fts_object_get_client_id((fts_object_t *)this));
  
  fts_set_int(a, fts_object_get_id((fts_object_t *)this->atom_list));
  fts_client_send_message((fts_object_t *)this, sym_setAtomList, 1, a);

  fts_send_message_varargs((fts_object_t *)this->atom_list, fts_s_upload, 0, 0);
}

static void
qlist_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);

  fts_atom_list_dump(this->atom_list, dumper, (fts_object_t *)this);
}

#define MAX_ATOMS_PER_LINE 18

static void qlist_save_dotpat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *) o;
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;
  fts_atom_list_iterator_t *iterator;
  int count;

  file = (FILE *)fts_get_pointer( at);

  if (fts_object_get_description_size(o) > 1)
    fprintf( file, "#N qlist %s;\n", fts_get_symbol( fts_object_get_description_atoms(o) + 1));
  else
    fprintf( file, "#N qlist;\n");

  count = 0;
  iterator = fts_atom_list_iterator_new( this->atom_list);

  while (! fts_atom_list_iterator_end( iterator))
    {
      fts_atom_t *p = fts_atom_list_iterator_current( iterator);

      if (count == 0)
	fprintf( file, "#X append");

      if ( fts_is_int( p))
	fprintf( file, " %d", fts_get_int( p));
      else if ( fts_is_float( p) )
	fprintf( file, " %f", fts_get_float( p));
      else if ( fts_is_symbol( p) )
	{
	  fts_symbol_t s = fts_get_symbol( p);

	  if (s == fts_s_semi || s == fts_s_comma)
	    fprintf( file, " \\%s", s);
	  else
	    fprintf( file, " %s", s);
	}

      count++;
      if ( count >= MAX_ATOMS_PER_LINE)
	{
	  fprintf( file, ";\n");
	  count = 0;
	}

      fts_atom_list_iterator_next( iterator);
    }

  if (count != MAX_ATOMS_PER_LINE)
    fprintf( file, ";\n");
    
  fts_atom_list_iterator_free( iterator);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P newobj %d %d %d %d qlist;\n", x, y, w, font_index);
}

static void
qlist_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  qlist_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  qlist_upload(o, 0, 0, 0, 0);
}

static void
qlist_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *)o;

  qlist_set_editor_close(this);
}

static void 
qlist_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  qlist_t *this = (qlist_t *) o;

  if(qlist_editor_is_open(this))
  {
    qlist_set_editor_close(this);
    fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
  }
}

static void
qlist_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(qlist_t), qlist_init, qlist_delete);

  fts_class_message_varargs(cl, fts_s_upload, qlist_upload);
  fts_class_message_varargs(cl, fts_s_dump_state, qlist_dump_state);
  fts_class_message_varargs(cl, fts_s_save_dotpat, qlist_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_openEditor, qlist_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, qlist_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, qlist_close_editor); 

  fts_class_message_varargs(cl, fts_new_symbol("rewind"), qlist_rewind);
  fts_class_message_varargs(cl, fts_new_symbol("flush"), qlist_flush);
  fts_class_message_varargs(cl, fts_s_clear, qlist_clear);

  fts_class_message_varargs(cl, fts_new_symbol("next"), qlist_next);
  fts_class_message_varargs(cl, fts_new_symbol("read"), qlist_read);

  fts_class_message_varargs(cl, fts_s_set, qlist_set);

  fts_class_message_varargs(cl, fts_s_append, qlist_append);

  fts_class_inlet_thru(cl, 0);

  fts_class_outlet_thru(cl, 0);
  fts_class_outlet_varargs(cl, 0);
}


void
qlist_config(void)
{
  sym_setAtomList = fts_new_symbol("setAtomList");
  
  fts_class_install(fts_new_symbol("qlist"), qlist_instantiate);
}
