/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.11 $ IRCAM $Date: 1998/05/12 10:16:44 $
 *
 * FTS by Miller Puckette
 * 
 * Modified by MDC: "__" behaviour deleted (not documented and shadock).
 * Also, in the old code "drop" did not worked with messages to the outlet
 * now they do.
 *
 */


#include "fts.h"


/* Updates are handled with the atom list update mechanism.
   Easier, cleaner, faster ... */

typedef struct
{
  fts_object_t  ob;

  fts_atom_list_t atom_list;
  fts_atom_list_iterator_t iterator;

} qlist_t;


/* Method for message "rewind" inlet 0 */

static void
qlist_rewind(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_iterator_init(&(this->iterator), &(this->atom_list));
}

#define MILLER_VERSION

#ifdef MILLER_VERSION



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
  fts_symbol_t who_name = 0;
  
  while (! fts_atom_list_iterator_end(&this->iterator))
    {
      int is_comma = 0;

      av[0] = *fts_atom_list_iterator_current(&(this->iterator));
      fts_atom_list_iterator_next(&this->iterator);

      if (!who_name)
	{
	  if (fts_is_float(av) || fts_is_long(av))
	    {
	      fts_atom_t waka[11];
	      fts_atom_t *wp = waka+1;
	      long count = 1;
	      waka[0] = *av;
	      
	      while (1)
		{
		  *wp = *fts_atom_list_iterator_current(&(this->iterator));

		  if ((! fts_is_long(wp)) && (! fts_is_float(wp)))
		    break;

		  fts_atom_list_iterator_next(&this->iterator);
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
	  else if (fts_is_long(av))
	    {
	      fts_outlet_send(o, 0, fts_s_int, 1, av);

	      break;
	    }
	}



      for (ac = 0, ap = av; ! fts_atom_list_iterator_end(&this->iterator);ac++)
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

	  *(++ap) = *(fts_atom_list_iterator_current(&(this->iterator)));

	  fts_atom_list_iterator_next(&this->iterator);
	}

      ap = av;

      if (! who_name)
	{
	  if (fts_is_symbol(av)  && 
	      (fts_get_symbol(av) != fts_s_semi) &&
	      (fts_get_symbol(av) != fts_s_comma))
	    {
	      if (! fts_receive_exists(fts_get_symbol(av)))
		{
		  post("qlist: %s: no such object\n",
		       fts_symbol_name(fts_get_symbol(av)));
		  continue;
		}
	      else
		{
		  who_name = fts_get_symbol(av);
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
	  if (fts_is_long(ap))
	    {
	      if (ac > 1)
		fts_send_message_to_receives(who_name, fts_s_list, ac, ap);
	      else 
		fts_send_message_to_receives(who_name, fts_s_int, ac, ap);
	    }
	  else if (fts_is_float(ap))
	    {
	      if (ac >1) 
		fts_send_message_to_receives(who_name, fts_s_list, ac, ap);
	      else 
		fts_send_message_to_receives(who_name, fts_s_float, ac, ap);
	    }
	  else if (fts_is_symbol(ap)  && 
		   (fts_get_symbol(ap) != fts_s_semi) &&
		   (fts_get_symbol(ap) != fts_s_comma))
	    fts_send_message_to_receives(who_name, fts_get_symbol(ap), ac-1, ap+1);
	}

      if (! is_comma)
	who_name = 0;
    }
}


#else
/* Method for message "next" [<int>] inlet 0 */

  /* SHADOCK WARNING: 

     The qlist have a number of bizzare and ad hoc behaviour
     that are too complex to solve with compatibilty messages, so
     probabily the best thing to do is to ignore it, and substitute
     it in the future.

     So, the following implementation use a finite state automata
     to rebuild the 0.26 shadock semantic
     */



  /* private macros */


#define NATOMS 256

#define ADVANCE_POINTER      fts_atom_list_iterator_next(&this->iterator);

#define GET_DESTINATION      \
     dest = fts_get_receive_by_name(fts_get_symbol(fts_atom_list_iterator_current(&(this->iterator))))

#define GET_SELECTOR         selector = fts_get_symbol(fts_atom_list_iterator_current(&(this->iterator)))

#define GET_ARG              args[args_count++] = *(fts_atom_list_iterator_current(&(this->iterator)))
#define RESET_ARGS           args_count = 0

#define SEND_INT_TO_OUTLET   fts_outlet_send(o, 0, fts_s_int,   args_count, args)
#define SEND_FLOAT_TO_OUTLET fts_outlet_send(o, 0, fts_s_float, args_count, args)
#define SEND_LIST_TO_OUTLET  fts_outlet_send(o, 0, fts_s_list,  args_count, args)

#define SEND_MSG_TO_DEST     if (dest && (! drop)) fts_message_send(dest, 0, selector, args_count, args)
#define SEND_FLOAT_TO_DEST   if (dest && (! drop)) fts_message_send(dest, 0, fts_s_float, args_count, args)
#define SEND_INT_TO_DEST     if (dest && (! drop)) fts_message_send(dest, 0, fts_s_int, args_count, args)
#define SEND_LIST_TO_DEST    if (dest && (! drop)) fts_message_send(dest, 0, fts_s_list, args_count, args)


static void
qlist_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  long drop = fts_get_long_arg(ac, at, 0, 0);
  fts_atom_t args[NATOMS];

  fts_object_t *dest;
  fts_symbol_t selector;
  int args_count;

  enum
    {
      qlist_start,

      qlist_o_got_int,
      qlist_o_got_float,
      qlist_o_got_list,
      
      qlist_d_got_dst,
      qlist_d_got_sel,
      qlist_d_got_int,
      qlist_d_got_float,
      qlist_d_got_list,

      qlist_stop
    } status;

  enum
    {
      qlist_in_end,
      qlist_in_semi,
      qlist_in_symbol,
      qlist_in_float,
      qlist_in_int
    } in_type;

  status = qlist_start;
  args_count = 0;
  
  while (status != qlist_stop)
    {
      /* Get the input, and analize it in in_type */

      if (fts_atom_list_iterator_end(&this->iterator))
	in_type = qlist_in_end;
      else if (fts_is_symbol(fts_atom_list_iterator_current(&(this->iterator))))
	{
	  if ((fts_get_symbol(fts_atom_list_iterator_current(&(this->iterator))) == fts_s_semi) ||
	      (fts_get_symbol(fts_atom_list_iterator_current(&(this->iterator))) == fts_s_comma))
	    in_type = qlist_in_semi;
	  else
	    in_type = qlist_in_symbol;
	}
      else if (fts_is_long(fts_atom_list_iterator_current(&(this->iterator))))
	in_type = qlist_in_int;
      else if (fts_is_float(fts_atom_list_iterator_current(&(this->iterator))))
	in_type = qlist_in_float;

      /* the actual finite state automata; look big, but is simpler than
	 what you can expect 
	 */

      switch (status)
	{
	case qlist_start:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      break;

	    case qlist_in_semi:
	      status = qlist_start;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	      status = qlist_d_got_dst;
	      GET_DESTINATION;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_float:
	      status = qlist_o_got_float;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_int:
	      status = qlist_o_got_int;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_o_got_int:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_INT_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_stop;
	      SEND_INT_TO_OUTLET;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	      status = qlist_stop;
	      SEND_INT_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_o_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_o_got_float:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_FLOAT_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_stop;
	      SEND_FLOAT_TO_OUTLET;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	      status = qlist_stop;
	      SEND_FLOAT_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_o_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_o_got_list:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_LIST_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_stop;
	      SEND_LIST_TO_OUTLET;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	      status = qlist_stop;
	      SEND_LIST_TO_OUTLET;
	      RESET_ARGS;
	      break;

	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_o_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

      
	case qlist_d_got_dst:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      break;

	    case qlist_in_semi:
	      status = qlist_d_got_dst;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	      status = qlist_d_got_sel;
	      GET_SELECTOR;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_float:
	      status = qlist_d_got_float;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_int:
	      status = qlist_d_got_int;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_d_got_sel:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_MSG_TO_DEST;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_start;
	      SEND_MSG_TO_DEST;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_d_got_sel;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_d_got_int:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_INT_TO_DEST;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_start;
	      SEND_INT_TO_DEST;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_d_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_d_got_float:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_FLOAT_TO_DEST;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_start;
	      SEND_FLOAT_TO_DEST;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_d_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_d_got_list:
	  switch (in_type)
	    {
	    case qlist_in_end:
	      status = qlist_stop;
	      SEND_LIST_TO_DEST;
	      RESET_ARGS;
	      break;

	    case qlist_in_semi:
	      status = qlist_start;
	      SEND_LIST_TO_DEST;
	      RESET_ARGS;
	      ADVANCE_POINTER;
	      break;

	    case qlist_in_symbol:
	    case qlist_in_float:
	    case qlist_in_int:
	      status = qlist_d_got_list;
	      GET_ARG;
	      ADVANCE_POINTER;
	      break;
	    }
	  break;

	case qlist_stop:
	  break;
	}
    }
}

#endif


/* Method for message "append" [<arg>*] inlet 0 */

static void
qlist_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_append( &(this->atom_list), ac, at);
  fts_atom_list_iterator_init(&(this->iterator), &(this->atom_list));
}

/* Method for message "set" [<arg>*] inlet 0 */

static void
qlist_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_set( &(this->atom_list), ac, at);
  fts_atom_list_iterator_init(&(this->iterator), &(this->atom_list));
}

/* Method for message "clear" inlet 0 */

static void
qlist_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_destroy(&(this->atom_list));
  fts_atom_list_iterator_init(&(this->iterator), &(this->atom_list));
}

/* Method for message "flush" inlet 0 */
/* code to flush a qlist up from CP to host */

static void
qlist_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Not Yet implemented */
}

static void
qlist_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  fts_atom_list_iterator_t iterator;

  fts_atom_list_iterator_init(&iterator, &(this->atom_list));

  fts_client_mess_start_msg(CLIENTMESS_CODE);
  fts_client_mess_add_object(o);
  fts_client_mess_add_sym(fts_new_symbol("atomList"));

  while (! fts_atom_list_iterator_end(&iterator))
    {
      fts_client_mess_add_atoms(1, fts_atom_list_iterator_current(&iterator));
      fts_atom_list_iterator_next(&iterator);
    }


  fts_client_mess_send_msg();
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

  fts_atom_list_init(&(this->atom_list));
  fts_atom_list_iterator_init(&(this->iterator), &(this->atom_list));
}



static void
qlist_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;

  fts_atom_list_destroy(&(this->atom_list));
}

static void
qlist_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  qlist_t *this = (qlist_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);

  fts_atom_list_save_bmax(&(this->atom_list), f, (fts_object_t *) this);
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

  /* Methods for saving in bmax */

  fts_method_define(cl, fts_SystemInlet, fts_s_clear, qlist_clear, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, qlist_save_bmax, 1, a);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append, qlist_append);

  /* Method for loading from the user interface */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set"), qlist_set);
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("update"), qlist_update, 0, 0);

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
  fts_metaclass_create(fts_new_symbol("qlist"),qlist_instantiate, fts_always_equiv);
}






