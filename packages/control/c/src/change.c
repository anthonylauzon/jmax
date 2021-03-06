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

#include <limits.h>
#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_atom_t state;
} change_t;


static void 
change_get(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  change_t* self = (change_t*)o;
  
  fts_outlet_atom(o, 0, &self->state);
}

static void
change_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  change_t *self = (change_t *)o;
  fts_tuple_t* atup;
  fts_atom_t a;
  
  switch(ac)
  {
  case 0:
    /* the bang case */
    if (!fts_is_void(&self->state))
    {
      fts_atom_void(&self->state);
    }
    break;
  case 1:
    /* a value case */
    fts_atom_assign(&self->state, at);
    break;
  default:
    /* the tuple case */
    atup = (fts_tuple_t*)fts_object_create(fts_tuple_class, ac, at);
    fts_object_refer(atup);
    fts_set_object(&a, atup);
    /* another refer is done in fts_atom_assign */
    fts_atom_assign(&self->state, &a);
    fts_object_release(atup);
  }
}


static void 
change_anything(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  change_t* self = (change_t*)o;
  fts_tuple_t* atup;
  fts_atom_t a;
  int winlet = fts_object_get_message_inlet(o);

  /* special case if we receive symbol set at inlet 1*/
  if (1 == winlet)
  {
    if (fts_s_set == s)
    {
      change_set(o, s, ac, at, fts_nix);
    }
    if (fts_s_get == s)
    {
      change_get(o, s, ac, at, fts_nix);
    }
  }
  else
  {
    if (NULL == s)
    {
      /* no selector */
      switch (ac)
      {
      case 0:
	if (!fts_is_void(&self->state))
	{
	  fts_atom_void(&self->state);
	  fts_outlet_atom(o, 0, &self->state);
	}
	break;
      case 1:
	if (!fts_atom_same_type(&self->state, at))
	{
	  fts_atom_assign(&self->state, at);
	  fts_outlet_atom(o, 0, at);
	}
	else
	{
	  if (!fts_is_object(at))
	  {
	    if (!fts_atom_identical(&self->state, at))
	    {
	      fts_atom_assign(&self->state, at);
	      fts_outlet_atom(o, 0, at);
	    }
	  }
	  else
	  {
	    fts_object_t* obj1 = fts_get_object(&self->state);
	    fts_object_t* obj2 = fts_get_object(at);
      
	    if (obj1 != obj2)
	    {
	      fts_class_t* class = fts_object_get_class(obj1);
	      fts_class_equals_function_t equals = fts_class_get_equals_function(class);
        
	      if ((NULL == equals) || (0 == (*equals)(obj1, obj2)))
	      {
          fts_atom_assign(&self->state, at);
          fts_outlet_atom(o, 0, at);
	      }
	    }
	  }
	}
	break;
      default:
	atup = (fts_tuple_t*)fts_object_create(fts_tuple_class, ac, at);
	fts_object_refer(atup);
	fts_set_object(&a, atup);
	if (!fts_atom_equals(&self->state, &a))
	{
	  fts_atom_assign(&self->state, &a);
	  fts_outlet_atom(o, 0, &a);
	}
	fts_object_release(atup);
	break;
      }
    }
    else
    {
      if (ac > 0)
      {
	/* create a tuple with selector and arguments ... */
	atup = (fts_tuple_t*)fts_object_create(fts_tuple_class, ac, at);
	fts_object_refer(atup);
	fts_tuple_prepend_symbol(atup, s);
	fts_set_object(&a, atup);
	if (!fts_atom_equals(&self->state, &a))
	{
	  fts_atom_assign(&self->state, &a);
	  fts_outlet_atom(o, 0, &a);
	}
	fts_object_release(atup);
      }
      else
      {
	/* we use the selector as current state */
	if (s != fts_get_symbol(&self->state))
	{
	  fts_atom_t a;
	  fts_set_symbol(&a, s);
	  fts_atom_assign(&self->state, &a);
	  fts_outlet_atom(o, 0, &a);
	}
      }
    }
  }

}

static void
change_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  change_t *self = (change_t *)o;

  /* force the 2nd inlet */
  fts_object_set_inlets_number(o, 2);

  fts_set_void(&self->state);

  if(ac > 0)
    fts_send_message(o, fts_s_set, ac, at, fts_nix);
}

static void
change_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  change_t *self = (change_t *)o;

  fts_atom_void(&self->state);
}

static void
change_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(change_t), change_init, change_delete);

  fts_class_input_handler(cl, change_anything);

  /* I hope we could do it in a near future */
  fts_class_message_varargs(cl, fts_s_set, change_set);
  fts_class_message_varargs(cl, fts_s_get, change_get);
  fts_class_outlet_atom(cl, 0);
}

void
change_config(void)
{
  fts_class_install(fts_new_symbol("change"), change_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
