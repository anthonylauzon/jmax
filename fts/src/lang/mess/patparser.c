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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/patlex.h"
/* (fd) For post */
#include "runtime/files.h"

/* Local symbols */


static fts_symbol_t fts_s_max;
static fts_symbol_t fts_s_v2;
static fts_symbol_t fts_s_diesN;
static fts_symbol_t fts_s_diesn;
static fts_symbol_t fts_s_diesP;
static fts_symbol_t fts_s_diesp;
static fts_symbol_t fts_s_diesT;
static fts_symbol_t fts_s_diest;
static fts_symbol_t fts_s_diesX;
static fts_symbol_t fts_s_diesx;
static fts_symbol_t fts_s_newex;
static fts_symbol_t fts_s_newobj;
static fts_symbol_t fts_s_vpatcher;
static fts_symbol_t fts_s_vtable;
static fts_symbol_t fts_s_slider;
static fts_symbol_t fts_s_number;
static fts_symbol_t fts_s_flonum;
static fts_symbol_t fts_s_intbox;
static fts_symbol_t fts_s_floatbox;
static fts_symbol_t fts_s_messbox;
static fts_symbol_t fts_s_button;
static fts_symbol_t fts_s_toggle;
static fts_symbol_t fts_s_message;
static fts_symbol_t fts_s_connect;
static fts_symbol_t fts_s_pop;
static fts_symbol_t fts_s_load_init;

/* Forward declarations */

/* auxiliary structures: store the graphic information for
 *  an object 
 */

typedef struct fts_graphic_description
{
  fts_atom_t x;
  fts_atom_t y;
  fts_atom_t width;
  fts_atom_t range;
  fts_atom_t fontSize;
} fts_graphic_description_t;



static void fts_patparse_graphic_description_init(fts_graphic_description_t *this);
static void fts_patparse_parse_patcher(fts_object_t *parent, fts_patlex_t *in);
static fts_object_t *fts_get_child(fts_object_t *obj, int idx);
static void fts_patparse_parse_connection(fts_object_t *parent, fts_patlex_t *in);
static int fts_patparse_read_object_arguments(fts_atom_t *args, fts_patlex_t *in);
static void fts_patparse_parse_object(fts_object_t *parent, fts_patlex_t *in,
				 fts_object_t *lastNObject, fts_symbol_t  lastNObjectType);
static fts_graphic_description_t *fts_patparse_parse_graphic_description(fts_patlex_t *in, fts_graphic_description_t *g);
static void fts_patparse_parse_window_properties(fts_object_t *parent, fts_patlex_t *in);
static void pat_error(const char *description);
static void pat_warning(const char *description);

void fts_patparser_init()
{
  fts_s_max = fts_new_symbol("max");
  fts_s_v2 = fts_new_symbol("v2");
  fts_s_diesN = fts_new_symbol("#N");
  fts_s_diesn = fts_new_symbol("#n");
  fts_s_diesP = fts_new_symbol("#P");
  fts_s_diesp = fts_new_symbol("#p");
  fts_s_diesT = fts_new_symbol("#T");
  fts_s_diest = fts_new_symbol("#t");
  fts_s_diesX = fts_new_symbol("#X");
  fts_s_diesx = fts_new_symbol("#x");
  fts_s_newex = fts_new_symbol("newex");
  fts_s_newobj = fts_new_symbol("newobj");
  fts_s_vpatcher = fts_new_symbol("vpatcher");
  fts_s_vtable = fts_new_symbol("vtable");
  fts_s_slider = fts_new_symbol("slider");
  fts_s_number = fts_new_symbol("number");
  fts_s_flonum = fts_new_symbol("flonum");
  fts_s_intbox = fts_new_symbol("intbox");
  fts_s_floatbox = fts_new_symbol("floatbox");
  fts_s_messbox = fts_new_symbol("messbox");
  fts_s_button = fts_new_symbol("button");
  fts_s_toggle = fts_new_symbol("toggle");
  fts_s_message = fts_new_symbol("message");
  fts_s_connect = fts_new_symbol("connect");
  fts_s_pop = fts_new_symbol("pop");
  fts_s_load_init = fts_new_symbol("load_init");
}

/*
 * Parse a .pat file.
 * A module providing  a parser for old Max .pat format files.
 */


static void
fts_patparse_graphic_description_init(fts_graphic_description_t *this)
{
  fts_set_int(&(this->x), 0);
  fts_set_int(&(this->y), 0);
  fts_set_int(&(this->width), 0);
  fts_set_int(&(this->range), 0);
  fts_set_int(&(this->fontSize), 10);
}

/* Called only if it make sense */

static void fts_patparse_set_range(fts_graphic_description_t *this, fts_patlex_t *in)
{
  fts_patlex_next_token(in);
  this->range = in->val;
}


int fontIndexTable[8] = {9, 10, 12, 14, 16, 18, 20, 24};
#define FONTINDEXTABLE_SIZE (sizeof(fontIndexTable)/sizeof(int))

void fts_patparse_set_font_size_table(int ac, const fts_atom_t *at)
{
  int i;

  for (i = 0; (i < ac) && (i < 8); i++)
    {
      if (fts_is_int(&at[i]))
	fontIndexTable[i] = fts_get_int(&at[i]);
    }
}

#define SIZE_BITS 6
#define SIZE_RANGE (1 << 6)

/*
In the Max ISPW .pat format the font size is saved like an index (from 0 to 7) of a vector containing sizes from 9 to 24.
In the actual Max .pat format the Mac font Id and the font size are saved together like a long integer. There is only a 
little set of Mac Fonts with a fixed Id, the others Id are platform dependents and the conversion
Id -->font_name is possible only with a systen_call on Mac Systems. So for the moment we are using font size only.    
 */
static void fts_patparse_set_font_index(fts_graphic_description_t *this, fts_patlex_t *in)
{
  int i;
  int font_id, font_size;
 
  fts_patlex_next_token(in);

  i = fts_get_int(&(in->val));

  if(i >= FONTINDEXTABLE_SIZE)
    {
      font_id = i >> SIZE_BITS;
      font_size = i & (SIZE_RANGE - 1);
      fts_set_int(&(this->fontSize), font_size);
    }
  else
    {
      if ( i < 0 )
	i = 2;
    
      fts_set_int(&(this->fontSize), fontIndexTable[i]);
    }
}

static void fts_patparse_set_text_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));

  if (fts_get_int(&(this->fontSize)) != 10)
    fts_object_put_prop(obj, fts_s_fontSize, &(this->fontSize));
}


static void fts_patparse_set_slider_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_atom_t zero;
  fts_atom_t max_value;
  fts_set_int(&zero, 0);

  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));
  fts_object_put_prop(obj, fts_s_height, &(this->range));
  fts_object_put_prop(obj, fts_s_min_value, &zero);

  fts_set_int(&max_value, fts_get_int(&(this->range)) - 1);
  fts_object_put_prop(obj, fts_s_max_value, &max_value);
}


static void fts_patparse_set_square_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));
  fts_object_put_prop(obj, fts_s_height, &(this->width));
}


/*
 * Create a new patcher from a .pat file.
 * The patcher is always a top level patcher.
 */
    
fts_object_t *fts_load_dotpat_patcher(fts_object_t *parent, fts_symbol_t filename) 
{
  fts_patlex_t *in; 

  in = fts_patlex_open(fts_symbol_name(filename), 0, 0);

  if (in != 0)
    {
      fts_atom_t description[1];
      fts_object_t *patcher;

      fts_set_symbol(&description[0], fts_s_patcher);
      patcher = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
      fts_patparse_parse_patlex(patcher, in);
      fts_patlex_close(in);
      fts_patcher_reassign_inlets_outlets((fts_patcher_t *) patcher);

      /* activate the post-load init, like loadbangs */

      fts_send_message(patcher, fts_SystemInlet, fts_s_load_init, 0, 0);

      return patcher;
    }
  else
    return 0;
}

  
/*
 * Method implementing the actual reading and parsing.
 *
 */
  
void fts_patparse_parse_patlex(fts_object_t *parent, fts_patlex_t *in)
{
  /* skip the header from the file, */

  fts_patlex_next_token(in);

  if (! token_sym_equals(in, fts_s_max))
    {
      pat_error("file not in .pat format (header error)");
      return;
    }

  fts_patlex_next_token(in); 

  if (! token_sym_equals(in, fts_s_v2))
    {
      pat_error("file not in .pat format (header error)");
      return;
    }

  /* Skip possible declarations */

  while (in->ttype != FTS_LEX_EOC)
    fts_patlex_next_token(in);

  /*     fts_patlex_next_token(in);  Skip the ';' */

  fts_patlex_next_token(in); /* Skip the '#N' */

  /*
   * call the parser for a patcher, implemented
   * as a patcher constructor, with the stream pointing
   * to the "vpatcher" word.
   */

  fts_patparse_parse_patcher(parent, in);
}


/*
 * Parse a patcher.
 *
 */

static void fts_patparse_parse_patcher(fts_object_t *parent, fts_patlex_t *in) 
{
  fts_object_t *lastNObject = 0;
  fts_symbol_t  lastNObjectType = 0;

  /* We don't try to do many checks, for the moment */

  fts_patlex_next_token(in); /* Skip the 'vpatcher' */

  fts_patparse_parse_window_properties(parent, in); 

  /* skip the optional processor number */

  fts_patlex_next_token(in);

  if (in->ttype !=  FTS_LEX_EOC)
    fts_patlex_next_token(in);	/* skip ';' */

  while (in->ttype != FTS_LEX_EOF)
    {
      fts_patlex_next_token(in); 

      if (token_sym_equals(in, fts_s_diesN))
	{
	  fts_patlex_next_token(in);

	  if (token_sym_equals(in, fts_s_vpatcher))
	    {
	      fts_atom_t a;
	      fts_atom_t description[1];

	      fts_patlex_push_back(in);

	      fts_set_symbol(&description[0], fts_s_patcher);

	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

	      fts_patparse_parse_patcher(lastNObject, in);

	      lastNObjectType = fts_s_patcher;
	    }
	  else if (token_sym_equals(in, fts_s_qlist))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], fts_s_qlist);
	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
	      lastNObjectType = fts_s_qlist;

	      /* skip the rest of the command: #N qlist argument are ignored */

	      while (in->ttype != FTS_LEX_EOC)
		fts_patlex_next_token(in);
	    }
	  else if (token_sym_equals(in, fts_s_vtable))
	    {
	      int haveName = 0;
	      int argc;
	      fts_atom_t vargs[10];
	      fts_atom_t description[10];

	      /* 
	       * Get the size and the name,
	       * ignore everything else (data
	       * that regard the editor)
	       */

	      argc = fts_patparse_read_object_arguments(vargs, in);
	      
	      /* get the name */

	      fts_set_symbol(&description[0], fts_s_table);

	      if (argc >= 8)
		{
		  description[1] = vargs[7];
		  haveName = 1;
		}


	      /* Make the table */

	      if (haveName)
		{
		  /* get the size */

		  description[2] = vargs[0];
		  lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 3, description);
		}
	      else
		{
		  /* get the size */

		  description[1] = vargs[0];
		  lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 2, description);
		}

	      lastNObjectType = fts_s_table;

	      /* skip the ';' */

	      fts_patlex_next_token(in);
	    }
	  else if (token_sym_equals(in, fts_s_explode))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], fts_s_explode);

	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
	      lastNObjectType = fts_s_explode;

	      /* skip the rest of the command */

	      while (in->ttype != FTS_LEX_EOC)
		fts_patlex_next_token(in);
	    }
	}
      else if (token_sym_equals(in, fts_s_diesP))
	{
	  /* get the line name */

	  fts_patlex_next_token(in); 

	  if (token_sym_equals(in, fts_s_connect))
	    {
	      fts_patparse_parse_connection(parent, in);
	    }
	  else if (token_sym_equals(in, fts_s_pop))
	    {
	      fts_patlex_next_token(in);	/* skip ';' */
	      
	      if  (in->ttype != FTS_LEX_EOC)
		{
		  /* the open patcher flag has been specified */

		  fts_patlex_next_token(in);	/* skip ';' */
		}
		    
	      return ;
	    }
	  else
	    {
	      /* Otherwise, we are parsing an object */
	      fts_patparse_parse_object(parent, in, lastNObject, lastNObjectType);
	      lastNObject = 0;
	      lastNObjectType = 0;
	    }

	  fts_patlex_next_token(in);/* skip ';' */
	}
      else if (token_sym_equals(in, fts_s_diesT))
	{
	  fts_patlex_next_token(in); /* get the command */

	  if (token_sym_equals(in, fts_s_set))
	    {
	      int argc;
	      fts_atom_t args[512];

	      argc = fts_patparse_read_object_arguments(args, in);

	      fts_send_message(lastNObject, 0, fts_s_set, argc, args);

	      fts_patlex_next_token(in);/* skip ';' ?? */
	    }
	  else
	    {
	      pat_error("Syntax error in a #T (table content)");
	      return;
	    }
	}
      else if (token_sym_equals(in, fts_s_diesX))
	{

	  if (lastNObjectType == fts_s_qlist)
	    {
	      int argc;
	      fts_atom_t args[512];
	      fts_symbol_t selector;

	      fts_patlex_next_token(in); /*  get the command */

	      selector = fts_get_symbol(&(in->val));

	      argc = fts_patparse_read_object_arguments(args, in);

	      fts_send_message(lastNObject, 0, selector, argc, args);
	      
	      fts_patlex_next_token(in);/* skip ';' ?? */
	    }
	  else if (lastNObjectType == fts_s_explode)
	    {
	      fts_patlex_next_token(in); /*  get the command */

	      if (in->ttype == FTS_LEX_SYMBOL)
		{
		  /* restore or stop commands */

		  fts_symbol_t selector;

		  selector = fts_get_symbol(&(in->val));

		  fts_send_message(lastNObject, 0, selector, 0, 0);
		}
	      else
		{
		  /* append command (without the keyword, sic. !!) */

		  int argc;
		  fts_atom_t args[512];

		  fts_patlex_push_back(in);

		  argc = fts_patparse_read_object_arguments(args, in);
		  fts_send_message(lastNObject, 0, fts_s_append, argc, args);
		}

	      fts_patlex_next_token(in); /* skip ';' ?? */
	    }
	  else
	    {
	      pat_warning("Syntax error: #X in a .pat file, after something different from qlist or table");

	      while ((in->ttype != FTS_LEX_EOC) && (in->ttype != FTS_LEX_EOF))
		fts_patlex_next_token(in);/* skip ';' */
	    }
	}
      else
	{
	  if ( in->ttype != FTS_LEX_SYMBOL)
	    post( "[%d] Expecting symbol, got %d\n", in->line_number, in->ttype);
	  else
	    post("[%d] Format not supported (%s)\n", in->line_number, fts_symbol_name( fts_get_symbol( &in->val)));
	  
	  /* skip until the next ';' */

	  while ((in->ttype != FTS_LEX_EOC) && (in->ttype != FTS_LEX_EOF))
	    fts_patlex_next_token(in);/* skip ';' */
	}
    }
}


/*
 * Load an abstraction, substituing the arguments.
 * 
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found in the file,
 * or the file contains an unimplemented construct.
 */


/*
 * Parse a connection. 
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */

static fts_object_t *
fts_get_child(fts_object_t *obj, int idx)
{
  fts_patcher_t *patcher = (fts_patcher_t *) obj;
  fts_object_t *p;
  
  idx = fts_patcher_get_objects_count( (fts_patcher_t *)obj ) - idx - 1;

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (idx == 0)
	return p;
      else
	idx--;
    }

  return 0;
}

static void fts_patparse_parse_connection(fts_object_t *parent, fts_patlex_t *in)
{
  int fromIdx;
  int toIdx;
  int inlet;
  int outlet;
  fts_object_t *from;
  fts_object_t *to;

  fts_patlex_next_token(in); 

  fromIdx = fts_get_int(&(in->val));

  fts_patlex_next_token(in); 
  outlet = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  toIdx = fts_get_int(&(in->val));

  fts_patlex_next_token(in); 
  inlet  = fts_get_int(&(in->val));

  from = fts_get_child(parent, fromIdx);
  to   = fts_get_child(parent, toIdx);

  fts_connection_new(FTS_NO_ID, from, outlet, to, inlet);
}


/*
 * Read object description arguments in a vect.
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */


/* Arrivato fino a qui  */

static int fts_patparse_read_object_arguments(fts_atom_t *args, fts_patlex_t *in) 
{
  int count = 0;

  fts_patlex_next_token(in);

  while (in->ttype != FTS_LEX_EOC)
    {
      args[count++] = in->val;

      fts_patlex_next_token(in);
    }

  fts_patlex_push_back(in);

  return count;
}


/*
 * Parse an object from the a tokenized stream.
 *
 */


static void fts_patparse_parse_object(fts_object_t *parent, fts_patlex_t *in,
				 fts_object_t *lastNObject, fts_symbol_t lastNObjectType)
{
  fts_object_t *obj;
  fts_symbol_t objclass = fts_get_symbol(&(in->val));
  fts_graphic_description_t gd;
  fts_graphic_description_t *graphicDescr = &gd;

  /* get the graphic information */

  fts_patparse_parse_graphic_description(in, graphicDescr);

  if (objclass == fts_s_slider)
    {
      fts_atom_t description[1];

      fts_patparse_set_range(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_slider);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
      fts_patparse_set_slider_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_newex)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_patlex_next_token(in);/* get the object name */

      /* Abstraction are handled directly by the fts_eval_object_description function */

      if (in->ttype == FTS_LEX_EOC)
	{
	  /*
	   * Empty object, built and return an
	   * object with an empty description , that will be an error object.
	   */

	  fts_patlex_push_back(in);

	  obj = fts_eval_object_description((fts_patcher_t *) parent, 0, 0);

	  fts_patparse_set_text_graphic_properties(graphicDescr, obj);

	  return;
	}

      if (in->ttype == FTS_LEX_SYMBOL)
	description[0] = in->val;
      else if (in->ttype == FTS_LEX_NUMBER)
	{
	  fts_set_symbol(&description[0], fts_s_int);
	  fts_patlex_push_back(in);
	}
      else if (in->ttype == FTS_LEX_FLOAT)
	{
	  fts_set_symbol(&description[0], fts_s_float);
	  fts_patlex_push_back(in);
	}

      argc = fts_patparse_read_object_arguments(description + 1, in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, argc + 1 , description);

      if (obj)
	fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_newobj)
    {
      /* Special handling for patchers, qlist, explode and table */

      int argc;
      fts_atom_t args[512];

      fts_patparse_set_font_index(graphicDescr,in);

      argc = fts_patparse_read_object_arguments(args, in);

      /* Check if it is the patcher, otherwise just skip it */

      if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_old_patcher ) &&
	  (lastNObjectType == fts_s_patcher))
	{
	  /* add the two ninlet and noutlet and the arguments to description */

	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);

	  if (argc > 1)
	    {
	      fts_atom_t new_args[512];
	      int i;

	      fts_patcher_reassign_inlets_outlets((fts_patcher_t *)lastNObject);

	      fts_set_symbol(&new_args[0], fts_s_patcher);

	      for (i = 1; (i < argc) && (i < 512); i++)
		new_args[i] = args[i];

	      fts_patcher_redefine((fts_patcher_t *)lastNObject, argc, new_args);
	    }
	  else
	    {
	      fts_patcher_reassign_inlets_outlets((fts_patcher_t *)lastNObject);
	    }
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_qlist) &&
	       (lastNObjectType == fts_s_qlist))
	{
	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_table) &&
	       (lastNObjectType == fts_s_table))
	{
	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_explode) &&
	       (lastNObjectType == fts_s_explode))
	{
	  /* First, look if we have a name there */

	  if (argc >= 2)
	    fts_object_put_prop(lastNObject, fts_s_name, &args[1]);

	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else
	{
	  if (fts_is_symbol(&args[0]))
	    post("Object %s not yet Supported in .pat files",
		 fts_symbol_name(fts_get_symbol(&(args[0]))));
	}
    }
  else if (objclass == fts_s_inlet)
    {
      fts_atom_t description[2];

      /* skip optional dsp flag  */
      fts_patlex_next_token(in);

      if (in->ttype == FTS_LEX_EOC)
	fts_patlex_push_back(in);

      /* Create a non assigned inlet */


      fts_set_symbol(&description[0], fts_s_inlet);
      fts_set_int(&description[1], -2); 
      obj = fts_eval_object_description((fts_patcher_t *)parent, 2, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_outlet)
    {
      fts_atom_t description[2];

      /* skip optional dsp flag  */

      fts_patlex_next_token(in);

      if (in->ttype == FTS_LEX_EOC)
	fts_patlex_push_back(in);

      /* Create a non assigned outlet */

      fts_set_symbol(&description[0], fts_s_outlet);
      fts_set_int(&description[1], -2); 
      obj = fts_eval_object_description((fts_patcher_t *)parent, 2, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_number)
    {
      fts_atom_t description[1];

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_intbox);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_flonum)
    {
      fts_atom_t description[1];

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_floatbox);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_button)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], fts_s_button);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_toggle)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], fts_s_toggle);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_message)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_messbox);

      fts_patparse_set_messbox_mode(in);
      argc = fts_patparse_read_object_arguments(description + 1, in);
      fts_patparse_set_normal_mode(in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
      fts_send_message(obj, fts_SystemInlet, fts_s_clear, 0, 0);
      fts_send_message(obj, fts_SystemInlet, fts_s_append, argc, description + 1);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_comment)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_comment);

      argc = fts_patparse_read_object_arguments(description + 1, in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, argc + 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else
    pat_error("unknown object type error");
}

	    
/*
 * Parse a fts Graphic description.
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */




static fts_graphic_description_t *fts_patparse_parse_graphic_description(fts_patlex_t *in, fts_graphic_description_t *g)
{
  fts_patparse_graphic_description_init(g);

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->x), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->y), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->width), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  return g;
}


/*
 * Parse a window description.
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */

static void fts_patparse_parse_window_properties(fts_object_t *parent, fts_patlex_t *in)
{
  int x_left, y_top, x_right, y_bottom;
  fts_atom_t x, y, height, width;

  fts_patlex_next_token(in);
  x_left = fts_get_int(&(in->val));
    
  fts_patlex_next_token(in);
  y_top = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  x_right = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  y_bottom = fts_get_int(&(in->val));

  /* If patcher window has big negative coordinates */
  if (x_left < -10000 || y_top < -10000)
    {
      fts_atom_t a;

      fts_set_int( &a, 1);
      fts_object_put_prop( parent, fts_new_symbol( "no_upload"), &a);

      x_left = -x_left;
      y_top = -y_top;

      return;
    }

  fts_set_int( &x, x_left);
  fts_object_put_prop(parent, fts_s_wx, &x);

  fts_set_int( &y, y_top);
  fts_object_put_prop(parent, fts_s_wy, &y);

  fts_set_int( &width, (x_right - x_left) );
  fts_object_put_prop(parent, fts_s_ww, &width);

  fts_set_int( &height, (y_bottom - y_top) );
  fts_object_put_prop(parent, fts_s_wh, &height);
}

static void pat_error(const char *description) 
{
  post("Error loading .pat file: %s\n", description);
}

static void pat_warning(const char *description) 
{
  post("Warning loading .pat file: %s\n", description);
}
