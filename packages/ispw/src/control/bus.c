/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/* Bus based send/receive */

#include "fts.h"

/******************************************************************************/
/*                                                                            */
/*        The Control/Dsp Bus Implementation                                  */
/*                                                                            */
/******************************************************************************/

static fts_symbol_t  fts_s_bus;

typedef struct fts_bus_input
{
  fts_object_t *src;

  struct fts_bus_input *next;

} fts_bus_input_t;


typedef struct fts_bus_output
{
  fts_object_t *dst;
  int outlet;
  struct fts_bus_output *next;
} fts_bus_output_t;


typedef struct fts_bus
{
  int  nch;			/* number of channels */
  fts_bus_input_t  **inputs;	/* a list for each channel */
  fts_bus_output_t **outputs;	/* a list for each channel */
} fts_bus_t;


static fts_heap_t *bus_input_heap;
static fts_heap_t *bus_output_heap;
static fts_heap_t *bus_heap;


static fts_bus_t *fts_bus_new(int nch)
{
  fts_bus_t *bus;
  int i;

  bus = (fts_bus_t *) fts_heap_alloc(bus_heap);

  bus->nch = nch;
  bus->outputs = fts_block_alloc(nch * sizeof(fts_bus_output_t *));
  bus->inputs  = fts_block_alloc(nch * sizeof(fts_bus_input_t *));

  for (i = 0; i < nch; i++)
    {
      bus->outputs[i] = 0;
      bus->inputs[i]  = 0;
    }

  return bus;
}


static void fts_bus_free(fts_bus_t *bus)
{
  int i;
  fts_bus_input_t *in;
  fts_bus_output_t *out;

  for (i = 0; i < bus->nch; i++)
    {
      for (in = bus->inputs[i]; in;)
	{
	  fts_bus_input_t *p;

	  p = in;
	  in = in->next;
	  fts_heap_free(p, bus_input_heap);
	}

      for (out = bus->outputs[i]; out;)
	{
	  fts_bus_output_t *p;

	  p = out;
	  out = out->next;
	  fts_heap_free(p, bus_output_heap);
	}
    }

  fts_block_free(bus->outputs, bus->nch * sizeof(fts_bus_input_t *));
  fts_block_free(bus->inputs, bus->nch * sizeof(fts_bus_output_t *));
  fts_heap_free(bus, bus_heap);
}



static void fts_bus_add_input(fts_bus_t *bus, int ch, fts_object_t *src)
{
  fts_bus_input_t *p;

  p = fts_heap_alloc(bus_input_heap);

  p->src = src;
  p->next = bus->inputs[ch];
  bus->inputs[ch] = p;
}


static void fts_bus_remove_input(fts_bus_t *bus, int ch, fts_object_t *src)
{
  fts_bus_input_t **p;

  p = &(bus->inputs[ch]); 
  while (*p)
    {
      if ((*p)->src == src)
	{
	  fts_bus_input_t *in;

	  in = (*p);

	  (*p) = (*p)->next;

	  fts_heap_free(in, bus_input_heap);

	  return;
	}
      else
	p = &((*p)->next);
    }
}


static void fts_bus_add_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet)
{
  fts_bus_output_t *p;

  p = fts_heap_alloc(bus_output_heap);

  p->dst = dst;
  p->outlet = outlet;
  p->next = bus->outputs[ch];
  bus->outputs[ch] = p;
}


static void fts_bus_remove_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet)
{
  fts_bus_output_t **p;

  p = &(bus->outputs[ch]);

  while (*p)
    {
      if (((*p)->dst == dst) && ((*p)->outlet == outlet))
	{
	  fts_bus_output_t *out;

	  out = (*p);

	  (*p) = (*p)->next;

	  fts_heap_free(out, bus_output_heap);

	  return;
	}
      else
	p = &((*p)->next);
    }
}

static void fts_bus_send_message(fts_bus_t *bus, int ch, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  fts_bus_output_t *p;

  p = bus->outputs[ch];

  while (p)
    {
      fts_outlet_send(p->dst, p->outlet, selector,  ac, at);
      p = p->next;
    }
}


/******************************************************************************/
/*                                                                            */
/*        The catch/throw equivalence function                                */
/*                                                                            */
/******************************************************************************/

static int fts_bus_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  int lets0, lets1;

  if (ac0 > 2)
    lets0 = ac0 - 2;
  else 
    lets0 = ((fts_bus_t *) fts_get_ptr(at0 + 1))->nch;

  if (ac1 > 2)
    lets1 = ac1 - 2;
  else 
    lets1 = ((fts_bus_t *) fts_get_ptr(at1 + 1))->nch;

  return lets0 == lets1;
}


/******************************************************************************/
/*                                                                            */
/*        The throw object                                                    */
/*                                                                            */
/******************************************************************************/


/* "throw". Send messages to a bus.
 * send <bus> <ch1> .. <chn>
 */


typedef struct throw
{
  fts_object_t  o;
  
  fts_bus_t *bus;

  int ninlets;
  int *channels;
} throw_t;


/* send anything resend everything receive_list the receive_list to all the receives */


static void
throw_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;
  int ch;

  ch = this->channels[winlet];

  if (ch >= 0)
    fts_bus_send_message(this->bus, ch, s, ac, at);
}


static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;
  int i;

  this->bus = (fts_bus_t *) fts_get_ptr(at + 1);

  if (ac > 2)
    {
      this->ninlets = ac - 2;
      this->channels = (int *)fts_block_alloc(this->ninlets * sizeof(int));

      for (i = 0; i < this->ninlets; i++)
	{
	  int ch = fts_get_int(at + i + 2) - 1;

	  if ((ch < this->bus->nch) && (ch >= 0))
	    {
	      this->channels[i] = ch;
	      fts_bus_add_input(this->bus, this->channels[i], o);
	    }
	  else
	    this->channels[i] = -1;
	}
    }
  else
    {
      this->ninlets = this->bus->nch;
      this->channels = (int *)fts_block_alloc(this->ninlets * sizeof(int));

      for (i = 0; i < this->ninlets; i++)
	{
	  this->channels[i] = i;
	  fts_bus_add_input(this->bus, i, o);
	}
    }
}


static void
throw_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  throw_t *this = (throw_t *) o;

  for (i = 0; i < this->ninlets; i++)
    if (this->channels[i] >= 0)
      fts_bus_remove_input(this->bus, this->channels[i], o);

  fts_block_free(this->channels, this->ninlets * sizeof(int));
}


static fts_status_t
throw_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i, inlets;

  /* initialize the class */

  if (ac > 2)
    inlets = ac - 2;
  else
    inlets = ((fts_bus_t *) fts_get_ptr(at + 1))->nch;

  fts_class_init(cl, sizeof(throw_t), inlets, 0, 0); 

  /* define the system methods */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, throw_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, throw_delete, 0, 0);

  for (i = 0; i < inlets; i++)
    fts_method_define_varargs(cl, i, fts_s_anything, throw_anything);

  return fts_Success;
}


static void
internal_throw_config(void)
{
  fts_metaclass_install(fts_new_symbol("throw"), throw_instantiate,  fts_bus_equiv);
}


/******************************************************************************/
/*                                                                            */
/*        The Catch object                                                    */
/*                                                                            */
/******************************************************************************/


typedef struct catch
{
  fts_object_t  o;

  fts_bus_t *bus;

  int noutlets;
  int *channels;
} catch_t;


static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  int i;

  this->bus = fts_get_ptr(at+1);

  if (ac > 2)
    {
      this->noutlets = ac - 2;
      this->channels = (int *)fts_block_alloc(this->noutlets * sizeof(int));

      for (i = 0; i < this->noutlets; i++)
	{
	  int ch = fts_get_int(at + i + 2) - 1;

	  if ((ch < this->bus->nch) && (ch >= 0))
	    {
	      this->channels[i] = ch;
	      fts_bus_add_output(this->bus, this->channels[i], o, i);
	    }
	  else
	    this->channels[i] = -1;
	}
    }
  else
    {
      this->noutlets = this->bus->nch;
      this->channels = (int *)fts_block_alloc(this->noutlets * sizeof(int));

      for (i = 0; i < this->noutlets; i++)
	{
	  this->channels[i] = i;
	  fts_bus_add_output(this->bus, i, o, i);
	}
    }
}

static void
catch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  int i;

  for (i = 0; i < this->noutlets; i++)
    if (this->channels[i] >= 0)
      fts_bus_remove_output(this->bus, this->channels[i], o, i);

  fts_block_free(this->channels, this->noutlets * sizeof(int));
}


static fts_status_t
catch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i, outlets;

  if (ac > 2)
    outlets = ac - 2;
  else
    outlets = ((fts_bus_t *) fts_get_ptr(at + 1))->nch;

  fts_class_init(cl, sizeof(catch_t), 0, outlets, 0); 

  /* define the system methods */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, catch_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, catch_delete, 0, 0);

  return fts_Success;
}



static void
internal_catch_config(void)
{
  fts_metaclass_install(fts_new_symbol("catch"), catch_instantiate, fts_bus_equiv);
}

/******************************************************************************/
/*                                                                            */
/*        The BUS object                                                      */
/*                                                                            */
/******************************************************************************/

/* The bus object */

typedef struct bus
{
  fts_object_t  o;

  fts_bus_t *bus;
} bus_t;


static void bus_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *) o;
  int nch = fts_get_int_arg(ac, at, 1, 1);

  this->bus = fts_bus_new(nch);
}


static void bus_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *) o;

  /* Cannot delete, it is used for the meta class !!! */
  /* fts_bus_free(this->bus); */
}


static void
bus_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  bus_t *this = (bus_t *) obj;

  fts_word_set_ptr(fts_get_value(value), (void *) this->bus);
  fts_set_type(value, fts_s_bus);
}


static fts_status_t
bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(bus_t), 0, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, bus_init, 2, a, 1);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, bus_delete, 0, 0);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, bus_get_state);

  return fts_Success;
}

static void
internal_bus_config(void)
{
  fts_class_install(fts_new_symbol("bus"), bus_instantiate);
}


/******************************************************************************/
/*                                                                            */
/*        Config                                                              */
/*                                                                            */
/******************************************************************************/

/* file config */


void
bus_config(void)
{
  fts_s_bus = fts_new_symbol("bus");

  bus_input_heap = fts_heap_new(sizeof(fts_bus_input_t));
  bus_output_heap = fts_heap_new(sizeof(fts_bus_output_t));
  bus_heap = fts_heap_new(sizeof(fts_bus_t));

  /* configuring  the classes */

  internal_throw_config();
  internal_catch_config();
  internal_bus_config();
}
