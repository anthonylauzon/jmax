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
 */


/*
 * 
 * Modified by MDC: "__" behaviour deleted (not documented and shadock).
 * Also, in the old code "drop" did not worked with messages to the outlet
 * now they do.
 *
 */

#include <fts/fts.h>
#include "atomlist.h"
#include "naming.h"

fts_symbol_t sym_setAtomList = 0;
fts_symbol_t sym_openEditor = 0;
fts_symbol_t sym_destroyEditor = 0;

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
qlist_rewind(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_iterator_init(this->iterator, this->atom_list);
}


/* Qlist NEXT version translated directly one-to-one from the
   original Miller Puckette code; no intention to understand
   or debug it */

#define NATOM 256

static void
qlist_next(fts_object_t *o, int winlet, fts_symbol_t s, int aac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  long drop = fts_get_long_arg(aac, at, 0, 0);
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
	  if (fts_is_float(av) || fts_is_int(av))
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
		  if (count < 11) {count++; wp++;}
		}

	      if (count == 1)
		{
		  if (fts_is_float(&waka[0]))
		    fts_outlet_send(o, 0, fts_s_float, 1, av);
		  else
		    fts_outlet_send(o, 0, fts_s_int, 1, av);
		}
	      else 
		{
		  if (count > 10)
		    count = 10;

		  fts_outlet_send(o, 0, fts_s_list, count, waka);
		}
	      break;
	    }
	  else if (fts_is_int(av))
	    {
	      fts_outlet_send(o, 0, fts_s_int, 1, av);

	      break;
	    }
	}



      for (ac = 0, ap = av; ! fts_atom_list_iterator_end(this->iterator);ac++)
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
		  post("qlist: %s: no such object\n", fts_symbol_name(fts_get_symbol(av)));
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

      if (! drop)
	{
	  if (fts_is_int(ap))
	    {
	      if (ac > 1)
		fts_message_send(target, 0, fts_s_list, ac, ap);
	      else 
		fts_message_send(target, 0, fts_s_int, ac, ap);
	    }
	  else if (fts_is_float(ap))
	    {
	      if (ac >1) 
		fts_message_send(target, 0, fts_s_list, ac, ap);
	      else 
		fts_message_send(target, 0, fts_s_float, ac, ap);
	    }
	  else if (fts_is_symbol(ap) && (fts_get_symbol(ap) != fts_s_semi) && (fts_get_symbol(ap) != fts_s_comma))
	    fts_message_send(target, 0, fts_get_symbol(ap), ac - 1, ap + 1);
	}

      if (!is_comma)
	target = 0;
    }
}



/* Method for message "append" [<arg>*] inlet 0 */

static void
qlist_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_append(this->atom_list, ac, at);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message((fts_object_t *)this->atom_list, fts_SystemInlet, sym_atomlist_update, 0, 0);
}

/* Method for message "set" [<arg>*] inlet 0 */

static void
qlist_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_set( this->atom_list, ac, at);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message((fts_object_t *)this->atom_list, fts_SystemInlet, sym_atomlist_update, 0, 0);
}

/* Method for message "clear" inlet 0 */

static void
qlist_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_clear(this->atom_list);
  fts_atom_list_iterator_init(this->iterator, this->atom_list);

  fts_send_message((fts_object_t *)this->atom_list, fts_SystemInlet, sym_atomlist_update, 0, 0);
}

/* Method for message "flush" inlet 0 */
/* code to flush a qlist up from CP to host */

static void
qlist_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Not Yet implemented (will it ever be ??) */
}

/* Method for message "read" <sym> inlet 0 */

static void
qlist_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post("qlist_read not implemented (yet)\n");
}


/* for the moment, the symbol name (optional second argument) is ignored (what it is, anyway ?) */

static void
qlist_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  fts_atom_t a[1];
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);

  this->open = 0;
 
  if(name)
      fts_set_symbol(a, name);
  else
      fts_set_symbol(a, fts_new_symbol(""));
  this->atom_list = (fts_atom_list_t *)fts_object_create(fts_class_get_by_name(atomlist_symbol), 1, a);
  
  fts_send_message((fts_object_t *)this->atom_list, fts_SystemInlet, fts_s_upload, 0, 0);

  this->iterator  = fts_atom_list_iterator_new(this->atom_list);
}


static void
qlist_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_send_message((fts_object_t *)this->atom_list, fts_SystemInlet, fts_s_delete, 0, 0);
  fts_atom_list_iterator_free(this->iterator);

  fts_client_send_message(o, sym_destroyEditor, 0, 0);
}

static void
qlist_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  fts_atom_t a[1];

  fts_set_object(a, (fts_object_t *)this->atom_list);
  fts_client_send_message((fts_object_t *)this, sym_setAtomList, 1, a);
}

static void
qlist_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);

  fts_atom_list_save_bmax(this->atom_list, f, (fts_object_t *) this);
}

#define MAX_ATOMS_PER_LINE 18

static void qlist_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *) o;
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;
  fts_atom_list_iterator_t *iterator;
  int count;

  file = (FILE *)fts_get_ptr( at);

  if ( o->argc > 1)
    fprintf( file, "#N qlist %s;\n", fts_symbol_name( fts_get_symbol( o->argv + 1)));
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
	    fprintf( file, " \\%s", fts_symbol_name( s));
	  else
	    fprintf( file, " %s", fts_symbol_name( s));
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
qlist_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  qlist_set_editor_open(this);
  fts_client_send_message(o, sym_openEditor, 0, 0);
  /*qlist_upload(o, 0, 0, 0, 0);*/
}

static void
qlist_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  qlist_set_editor_close(this);
}

static fts_status_t
qlist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(qlist_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, qlist_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, qlist_delete, 0, 0);
  fts_method_define(cl, fts_SystemInlet, fts_s_upload, qlist_upload, 0, 0);

  /* Methods for saving in bmax */

  fts_method_define(cl, fts_SystemInlet, fts_s_clear, qlist_clear, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, qlist_save_bmax, 1, a);

  a[0] = fts_s_ptr;
  fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, qlist_save_dotpat, 1, a); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append, qlist_append);

  /* Method for loading from the user interface */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), qlist_open_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), qlist_close_editor);

  a[0] = fts_s_int;

  fts_method_define(cl, 0, fts_new_symbol("rewind"), qlist_rewind, 0, 0);

  fts_method_define(cl, 0, fts_new_symbol("flush"), qlist_flush, 0, 0);

  fts_method_define(cl, 0, fts_s_clear, qlist_clear, 0, 0);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("next"), qlist_next, 1, a, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("read"), qlist_read, 1, a);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), qlist_set);

  fts_method_define_varargs(cl, 0, fts_s_append, qlist_append);

  return fts_Success;
}


void
qlist_config(void)
{
  sym_setAtomList = fts_new_symbol("setAtomList");
  sym_openEditor = fts_new_symbol("openEditor");
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  
  fts_class_install(fts_new_symbol("qlist"), qlist_instantiate);
}






