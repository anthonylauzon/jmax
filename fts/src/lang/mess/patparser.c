#include <stdio.h>		/* for error reporting, temp. */
#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/patlex.h"

/* TODO: assigninletoutlets, setObjectName, better integration, of course */

/* Local symbols */

static fts_symbol_t fts_s_off;
static fts_symbol_t fts_s_unnamed;
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
static fts_symbol_t fts_s_patcher;
static fts_symbol_t fts_s_inlet;
static fts_symbol_t fts_s_outlet;
static fts_symbol_t fts_s_qlist;
static fts_symbol_t fts_s_vtable;
static fts_symbol_t fts_s_table;
static fts_symbol_t fts_s_slider;
static fts_symbol_t fts_s_append;
static fts_symbol_t fts_s__set;
static fts_symbol_t fts_s_explode;
static fts_symbol_t fts_s_number;
static fts_symbol_t fts_s_flonum;
static fts_symbol_t fts_s_intbox;
static fts_symbol_t fts_s_floatbox;
static fts_symbol_t fts_s_messbox;
static fts_symbol_t fts_s_button;
static fts_symbol_t fts_s_toggle;
static fts_symbol_t fts_s_message;
static fts_symbol_t fts_s_comment;
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



static void fts_graphic_description_init(fts_graphic_description_t *this);
static void readFromPatLexer(fts_object_t *parent, fts_pat_lexer_t *in);
static void parsePatcher(fts_object_t *parent, fts_pat_lexer_t *in);
static fts_object_t *fts_get_child(fts_object_t *obj, int idx);
static void parseConnection(fts_object_t *parent, fts_pat_lexer_t *in);
static int readObjectArguments(fts_atom_t *args, fts_pat_lexer_t *in);
static void parseObject(fts_object_t *parent, fts_pat_lexer_t *in,
				 fts_object_t *lastNObject, fts_symbol_t  lastNObjectType);
static fts_graphic_description_t *parseGraphic(fts_pat_lexer_t *in, fts_graphic_description_t *g);
static void parseWindowProperties(fts_object_t *parent, fts_pat_lexer_t *in);
static void pat_error(const char *description);

void fts_patparser_init()
{
  fts_s_unnamed = fts_new_symbol("unnamed");
  fts_s_off = fts_new_symbol("off");
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
  fts_s_patcher = fts_new_symbol("patcher");
  fts_s_inlet = fts_new_symbol("inlet");
  fts_s_outlet = fts_new_symbol("outlet");
  fts_s_qlist = fts_new_symbol("qlist");
  fts_s_vtable = fts_new_symbol("vtable");
  fts_s_table = fts_new_symbol("table");
  fts_s_slider = fts_new_symbol("slider");
  fts_s_append = fts_new_symbol("append");
  fts_s__set = fts_new_symbol("_set");
  fts_s_explode = fts_new_symbol("explode");
  fts_s_number = fts_new_symbol("number");
  fts_s_flonum = fts_new_symbol("flonum");
  fts_s_intbox = fts_new_symbol("intbox");
  fts_s_floatbox = fts_new_symbol("floatbox");
  fts_s_messbox = fts_new_symbol("messbox");
  fts_s_button = fts_new_symbol("button");
  fts_s_toggle = fts_new_symbol("toggle");
  fts_s_message = fts_new_symbol("message");
  fts_s_comment = fts_new_symbol("comment");
  fts_s_connect = fts_new_symbol("connect");
  fts_s_pop = fts_new_symbol("pop");
  fts_s_load_init = fts_new_symbol("load_init");
}

/*
 * Parse a .pat file.
 * A module providing  a parser for old Max .pat format files.
 */


static void
fts_graphic_description_init(fts_graphic_description_t *this)
{
  fts_set_int(&(this->x), 0);
  fts_set_int(&(this->y), 0);
  fts_set_int(&(this->width), 0);
  fts_set_int(&(this->range), 0);
  fts_set_int(&(this->fontSize), 10);
}

/* Called only if it make sense */

void setRange(fts_graphic_description_t *this, fts_pat_lexer_t *in)
{
  nextToken(in);
  this->range = in->val;
}


void setFontIndex(fts_graphic_description_t *this, fts_pat_lexer_t *in)
{
  int n;

  nextToken(in);
  n = fts_get_int(&(in->val));

  switch (n)
    {
    case 0:
      fts_set_int(&(this->fontSize), 9);
      break;
    case 1:
      fts_set_int(&(this->fontSize), 10);
      break;
    case 2:
      fts_set_int(&(this->fontSize), 12);
      break;
    case 3:
      fts_set_int(&(this->fontSize), 14);
      break;
    case 4:
      fts_set_int(&(this->fontSize), 16);
      break;
    case 5:
      fts_set_int(&(this->fontSize), 18);
      break;
    case 6:
      fts_set_int(&(this->fontSize), 20);
      break;
    case 7:
      fts_set_int(&(this->fontSize), 24);
      break;
    }
}



void setTextGraphicProperties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));

  if (fts_get_int(&(this->fontSize)) != 10)
    fts_object_put_prop(obj, fts_s_fontSize, &(this->fontSize));
}


void setSliderGraphicProperties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_atom_t zero;

  fts_set_int(&zero, 0);

  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));
  fts_object_put_prop(obj, fts_s_height, &(this->range));
  fts_object_put_prop(obj, fts_s_min_value, &zero);
  fts_object_put_prop(obj, fts_s_max_value, &(this->range));
}

void setSquareGraphicProperties(fts_graphic_description_t *this, fts_object_t *obj)
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
    
fts_object_t *fts_load_dotpat_patcher(fts_object_t *patcher, const char *inputFile) 
{
  fts_pat_lexer_t *in; 

  in = fts_open_pat_lexer(inputFile, 0, 0);
  
  if (in != 0)
    {
      fts_atom_t a;

      readFromPatLexer(patcher, in);

      fts_close_pat_lexer(in);

      fts_patcher_reassign_inlets_outlets_name((fts_patcher_t *) patcher, fts_s_unnamed);

      fts_set_symbol(&a, fts_s_unnamed);
      fts_object_put_prop(patcher, fts_s_name, &a);

      fts_set_symbol(&a, fts_s_off);
      fts_object_put_prop(patcher, fts_s_autorouting, &a);

      fts_object_send_properties(patcher);

      /* activate the post-load init, like loadbangs */

      fts_message_send(patcher, fts_SystemInlet, fts_s_load_init, 0, 0);

      return patcher;
    }
  else
    return 0;
}

  
/*
 * Method implementing the actual reading and parsing.
 *
 */
  
static void readFromPatLexer(fts_object_t *parent, fts_pat_lexer_t *in)
{
  /* skip the header from the file, */

  nextToken(in);

  if (! token_sym_equals(in, fts_s_max))
    {
      pat_error("file not in .pat format (header error)");
      return;
    }

  nextToken(in); 

  if (! token_sym_equals(in, fts_s_v2))
    {
      pat_error("file not in .pat format (header error)");
      return;
    }

  /* Skip possible declarations */

  while (in->ttype != FTS_LEX_EOC)
    nextToken(in);

  /*     nextToken(in);  Skip the ';' */

  nextToken(in); /* Skip the '#N' */

  /*
   * call the parser for a patcher, implemented
   * as a patcher constructor, with the stream pointing
   * to the "vpatcher" word.
   */

  parsePatcher(parent, in);
}


/*
 * Parse a patcher.
 *
 */

static void parsePatcher(fts_object_t *parent, fts_pat_lexer_t *in) 
{
  fts_object_t *lastNObject = 0;
  fts_symbol_t  lastNObjectType = 0;

  /* We don't try to do many checks, for the moment */

  nextToken(in); /* Skip the 'vpatcher' */

  parseWindowProperties(parent, in); 

  /* skip the optional processor number */

  nextToken(in);

  if (in->ttype !=  FTS_LEX_EOC)
    nextToken(in);	/* skip ';' */

  while (in->ttype != FTS_LEX_EOF)
    {
      nextToken(in); 

      if (token_sym_equals(in, fts_s_diesN))
	{
	  nextToken(in);

	  if (token_sym_equals(in, fts_s_vpatcher))
	    {
	      fts_atom_t a;
	      fts_atom_t description[4];

	      pushBack(in);

	      fts_set_symbol(&description[0], fts_s_patcher);
	      fts_set_symbol(&description[1], fts_new_symbol("unnamed"));
	      fts_set_int(&description[2], 0);
	      fts_set_int(&description[3], 0);

	      lastNObject = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 4, description);

	      fts_set_symbol(&a, fts_s_off);
	      fts_object_put_prop(lastNObject, fts_s_autorouting, &a);

	      parsePatcher(lastNObject, in);

	      lastNObjectType = fts_s_patcher;
	    }
	  else if (token_sym_equals(in, fts_s_qlist))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], fts_s_qlist);
	      lastNObject = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);
	      lastNObjectType = fts_s_qlist;

	      /* skip the rest of the command: #N qlist argument are ignored */

	      while (in->ttype != FTS_LEX_EOC)
		nextToken(in);
	    }
	  else if (token_sym_equals(in, fts_s_vtable))
	    {
	      int argc;
	      fts_atom_t vargs[10];
	      fts_atom_t description[10];

	      /* 
	       * Get the size and the name,
	       * ignore everything else (data
	       * that regard the editor)
	       */

	      argc = readObjectArguments(vargs, in);
	      
	      /* get the name */

	      fts_set_symbol(&description[0], fts_s_table);

	      if (argc >= 8)
		description[1] = vargs[7];
	      else
		fts_set_symbol(&description[1], fts_s_table);

	      /* get the size */

	      description[2] = vargs[0];

	      /* Make the table */

	      lastNObject = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 3, description);
	      lastNObjectType = fts_s_table;

	      /* skip the ';' */

	      nextToken(in);
	    }
	  else if (token_sym_equals(in, fts_s_explode))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], fts_s_explode);

	      lastNObject = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);
	      lastNObjectType = fts_s_explode;

	      /* skip the rest of the command */

	      while (in->ttype != FTS_LEX_EOC)
		nextToken(in);
	    }
	}
      else if (token_sym_equals(in, fts_s_diesP))
	{
	  /* get the line name */

	  nextToken(in); 

	  if (token_sym_equals(in, fts_s_connect))
	    {
	      parseConnection(parent, in);
	    }
	  else if (token_sym_equals(in, fts_s_pop))
	    {
	      nextToken(in);	/* skip ';' */
		    
	      if  (in->ttype != FTS_LEX_EOC)
		{
		  /* the open patcher flag has been specified */

		  nextToken(in);	/* skip ';' */
		}
		    
	      return ;
	    }
	  else
	    {
	      /* Otherwise, we are parsing an object */

	      parseObject(parent, in, lastNObject, lastNObjectType);
	      lastNObject = 0;
	      lastNObjectType = 0;
	    }

	  nextToken(in);/* skip ';' */
	}
      else if (token_sym_equals(in, fts_s_diesT))
	{
	  nextToken(in); /* get the command */

	  if (token_sym_equals(in, fts_s_set))
	    {
	      int argc;
	      fts_atom_t args[512];

	      argc = readObjectArguments(args, in);

	      fts_message_send(lastNObject, 0, fts_s__set, argc, args);

	      nextToken(in);/* skip ';' ?? */
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

	      nextToken(in); /*  get the command */

	      selector = fts_get_symbol(&(in->val));

	      argc = readObjectArguments(args, in);

	      fts_message_send(lastNObject, 0, selector, argc, args);
	      
	      nextToken(in);/* skip ';' ?? */
	    }
	  else if (lastNObjectType == fts_s_explode)
	    {
	      nextToken(in); /*  get the command */

	      if (in->ttype == FTS_LEX_SYMBOL)
		{
		  /* restore or stop commands */

		  fts_symbol_t selector;

		  selector = fts_get_symbol(&(in->val));

		  fts_message_send(lastNObject, 0, selector, 0, 0);
		}
	      else
		{
		  /* append command (without the keyword, sic. !!) */

		  int argc;
		  fts_atom_t args[512];

		  pushBack(in);

		  argc = readObjectArguments(args, in);
		  fts_message_send(lastNObject, 0, fts_s_append, argc, args);
		}

	      nextToken(in); /* skip ';' ?? */
	    }
	  else
	    {
	      pat_error("Syntax error: #X in a .pat file, after something different from qlist or table");
	      return;
	    }
	}
      else
	{
	  fprintf(stderr, "Format not supported (#%c)\n", (char) in->ttype);
	  
	  /* skip until the next ';' */

	  while ((in->ttype != FTS_LEX_EOC) && (in->ttype != FTS_LEX_EOF))
	    nextToken(in);/* skip ';' */
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

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      if (idx == 0)
	return p;
      else
	idx--;
    }

  return 0;
}

static void parseConnection(fts_object_t *parent, fts_pat_lexer_t *in)
{
  int fromIdx;
  int toIdx;
  int inlet;
  int outlet;
  fts_object_t *from;
  fts_object_t *to;

  nextToken(in); 

  fromIdx = fts_get_int(&(in->val));

  nextToken(in); 
  outlet = fts_get_int(&(in->val));

  nextToken(in);
  toIdx = fts_get_int(&(in->val));

  nextToken(in); 
  inlet  = fts_get_int(&(in->val));

  from = fts_get_child(parent, fromIdx);
  to   = fts_get_child(parent, toIdx);

  fts_object_connect(from, outlet, to, inlet);
}


/*
 * Read object description arguments in a vect.
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */


/* Arrivato fino a qui  */

static int readObjectArguments(fts_atom_t *args, fts_pat_lexer_t *in) 
{
  int count = 0;

  nextToken(in);

  while (in->ttype != FTS_LEX_EOC)
    {
      args[count++] = in->val;

      nextToken(in);
    }

  pushBack(in);

  return count;
}


/*
 * Parse an object from the a tokenized stream.
 *
 */


static void parseObject(fts_object_t *parent, fts_pat_lexer_t *in,
				 fts_object_t *lastNObject, fts_symbol_t lastNObjectType)
{
  fts_object_t *obj;
  fts_symbol_t objclass = fts_get_symbol(&(in->val));
  fts_graphic_description_t gd;
  fts_graphic_description_t *graphicDescr = &gd;

  /* get the graphic information */

  parseGraphic(in, graphicDescr);

  if (objclass == fts_s_slider)
    {
      fts_atom_t description[1];

      setRange(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_slider);

      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);
      setSliderGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_newex)
    {
      fts_atom_t description[512];







      int argc;

      setFontIndex(graphicDescr, in);

      nextToken(in);/* get the object name */

      /* Abstraction are handled directly by the fts_object_new function */

      if (in->ttype == FTS_LEX_EOC)
	{
	  /*
	   * Empty object, built and return a 
	   * arbitrary choosen "comment" object
	   */

	  fts_atom_t description[1];

	  pushBack(in);

	  fts_set_symbol(&description[0], fts_s_comment);

	  obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

	  setTextGraphicProperties(graphicDescr, obj);

	  return;
	}

      if (in->ttype == FTS_LEX_SYMBOL)
	description[0] = in->val;
      else if (in->ttype == FTS_LEX_NUMBER)
	{
	  fts_set_symbol(&description[0], fts_s_int);
	  pushBack(in);
	}
      else if (in->ttype == FTS_LEX_FLOAT)
	{
	  fts_set_symbol(&description[0], fts_s_float);
	  pushBack(in);
	}

      argc = readObjectArguments(description + 1, in);

      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, argc + 1 , description);

      setTextGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_newobj)
    {
      /* Special handling for patchers, qlist, explode and table */

      int argc;
      fts_atom_t args[512];

      setFontIndex(graphicDescr,in);

      argc = readObjectArguments(args, in);

      /* Check if it is the patcher, otherwise just skip it */

      if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_patcher ) &&
	  (lastNObjectType == fts_s_patcher))
	{
	  /* add the two ninlet and noutlet arguments to description */

	  setTextGraphicProperties(graphicDescr, lastNObject);

	  if (argc > 1)
	    fts_patcher_reassign_inlets_outlets_name((fts_patcher_t *)lastNObject, fts_get_symbol(&args[1]));
	  else
	    fts_patcher_reassign_inlets_outlets_name((fts_patcher_t *)lastNObject, fts_s_unnamed);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_qlist) &&
	       (lastNObjectType == fts_s_qlist))
	{
	  setTextGraphicProperties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_table) &&
	       (lastNObjectType == fts_s_table))
	{
	  setTextGraphicProperties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_explode) &&
	       (lastNObjectType == fts_s_explode))
	{
	  setTextGraphicProperties(graphicDescr, lastNObject);
	}
      else
	{
	  if (fts_is_symbol(&args[0]))
	    fprintf(stderr, "Object type %s not yet Supported in .pat files",
		    fts_symbol_name(fts_get_symbol(&(args[0]))));
	}
    }
  else if (objclass == fts_s_inlet)
    {
      fts_atom_t description[1];

      /* skip optional dsp flag  */
      nextToken(in);

      if (in->ttype == FTS_LEX_EOC)
	pushBack(in);

      fts_set_symbol(&description[0], fts_s_inlet);
      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setSquareGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_outlet)
    {
      fts_atom_t description[1];

      /* skip optional dsp flag  */

      nextToken(in);

      if (in->ttype == FTS_LEX_EOC)
	pushBack(in);

      fts_set_symbol(&description[0], fts_s_outlet);
      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setSquareGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_number)
    {
      fts_atom_t description[1];

      setFontIndex(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_intbox);
      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setTextGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_flonum)
    {
      fts_atom_t description[1];

      setFontIndex(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_floatbox);
      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setTextGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_button)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], fts_s_button);
      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setSquareGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_toggle)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], fts_s_toggle);

      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, 1, description);

      setSquareGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_message)
    {
      fts_atom_t description[512];
      int argc;

      setFontIndex(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_messbox);

      argc = readObjectArguments(description + 1, in);

      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, argc + 1, description);

      setTextGraphicProperties(graphicDescr, obj);
    }
  else if (objclass == fts_s_comment)
    {
      fts_atom_t description[512];
      int argc;

      setFontIndex(graphicDescr, in);

      fts_set_symbol(&description[0], fts_s_comment);

      argc = readObjectArguments(description + 1, in);

      obj = fts_object_new((fts_patcher_t *)parent, FTS_NO_ID, argc + 1, description);

      setTextGraphicProperties(graphicDescr, obj);
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




static fts_graphic_description_t *parseGraphic(fts_pat_lexer_t *in, fts_graphic_description_t *g)
{
  fts_graphic_description_init(g);

  nextToken(in);

  if (in->ttype == FTS_LEX_NUMBER)
    g->x = in->val;
  else
    {
      pushBack(in);
      return g;
    }

  nextToken(in);

  if (in->ttype == FTS_LEX_NUMBER)
    g->y = in->val;
  else
    {
      pushBack(in);
      return g;
    }

  nextToken(in);

  if (in->ttype == FTS_LEX_NUMBER)
    g->width = in->val;
  else
    {
      pushBack(in);
      return g;
    }

  return g;
}


/*
 * Parse a window description.
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */

static void parseWindowProperties(fts_object_t *parent, fts_pat_lexer_t *in)
{
  int x2, y2;
  fts_atom_t x, y, height, width;

  /* We don't try to do many checks, for the moment */

  nextToken(in);
  x = in->val;
    
  nextToken(in);
  y = in->val;

  nextToken(in);
  x2 = fts_get_int(&(in->val));

  nextToken(in);
  y2 = fts_get_int(&(in->val));

  fts_set_int(&width,  x2 - fts_get_int(&x));
  fts_set_int(&height, y2 - fts_get_int(&y));

  fts_object_put_prop(parent, fts_s_wx, &x);
  fts_object_put_prop(parent, fts_s_wy, &y);
  fts_object_put_prop(parent, fts_s_ww, &width);
  fts_object_put_prop(parent, fts_s_wh, &height);
}

static void pat_error(const char *description) 
{
  fprintf(stderr, "Error loading .pat file: %s\n", description);
}


/* The real abstraction loader */

fts_object_t *fts_abstraction_new(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  int extension;
  char buf[1024];
  char name[1024];
  char *p;
  FILE *file;

  strcpy(name, fts_symbol_name(fts_get_symbol(&at[0])));

  p = strrchr(name, '.');

  if (! p)
    {
      /* No extension given in the name, ok */

	extension = 0;
    }
  else
    {
      /* Extension given in the name, check for .pat or .abs */

      if ((! strcmp(p, ".pat")) || 
	  (! strcmp(p, ".abs")))

	extension = 1;
      else
	extension = 0;
    }

  /* Find the file (HACK, to be substituted with the real thing) */

  if (extension)
    {
      sprintf(buf, "/u/worksta/dececco/tmp/abs/%s", name);

      file = fopen(buf, "r");
    }
  else
    {
      /* Try Nature */

      sprintf(buf, "/u/worksta/dececco/tmp/abs/%s", name);

      file = fopen(buf, "r");

      if (! file)
	{
	  /* Try .abs */

	  sprintf(buf, "/u/worksta/dececco/tmp/abs/%s.abs", name);

	  file = fopen(buf, "r");

	  if (! file)
	    {
	      /* Try .pat */

	      sprintf(buf, "/u/worksta/dececco/tmp/abs/%s.pat", name);
	      file = fopen(buf, "r");
	    }
	}
    }

  if (file)
    {
      int i;
      fts_pat_lexer_t *in; 
      fts_atom_t a;
      fts_atom_t description[4];
      fts_object_t *obj;

      fts_set_symbol(&description[0], fts_s_patcher);
      fts_set_symbol(&description[1], fts_new_symbol("unnamed"));
      fts_set_int(&description[2], 0);
      fts_set_int(&description[3], 0);

      obj = fts_object_new((fts_patcher_t *)patcher, FTS_NO_ID, 4, description);

      fts_set_symbol(&a, fts_s_off);
      fts_object_put_prop(obj, fts_s_autorouting, &a);

      /* Change the description in the object */

      fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

      obj->argc = ac;
      obj->argv = (fts_atom_t *) fts_block_zalloc(ac * sizeof(fts_atom_t));

      for (i = 0; i < ac; i++)
	obj->argv[i] = at[i];

      /* flag the patcher as abstraction */

      fts_patcher_set_abstraction((fts_patcher_t *)obj);

      /* get the lexer */

      in = fts_open_pat_lexer_file(file, ac - 1, at + 1);

      readFromPatLexer(obj, in);

      fts_patcher_reassign_inlets_outlets_name((fts_patcher_t *) obj, fts_get_symbol(&at[0]));

      fts_close_pat_lexer(in);

      return obj;
    }


  /* not found, return null */

  return 0;
}

