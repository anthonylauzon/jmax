/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
#include "bus.h"

fts_symbol_t fts_s_bus = 0;

static fts_heap_t *bus_input_heap;
static fts_heap_t *bus_output_heap;
static fts_heap_t *bus_heap;

/******************************************************************
 *
 *  handle the bus structure
 *
 */

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

/* inputs and outputs to the bus */

void fts_bus_add_input(fts_bus_t *bus, int ch, fts_object_t *src)
{
  fts_bus_input_t *p;

  p = fts_heap_alloc(bus_input_heap);

  p->src = src;
  p->next = bus->inputs[ch];
  bus->inputs[ch] = p;
}

void fts_bus_remove_input(fts_bus_t *bus, int ch, fts_object_t *src)
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

void fts_bus_add_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet)
{
  fts_bus_output_t *p;

  p = fts_heap_alloc(bus_output_heap);

  p->dst = dst;
  p->outlet = outlet;
  p->next = bus->outputs[ch];
  bus->outputs[ch] = p;
}

void fts_bus_remove_output(fts_bus_t *bus, int ch, fts_object_t *dst, int outlet)
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

void fts_bus_send_message(fts_bus_t *bus, int ch, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  fts_bus_output_t *p;

  p = bus->outputs[ch];

  while (p)
    {
      fts_outlet_send(p->dst, p->outlet, selector,  ac, at);
      p = p->next;
    }
}

int fts_bus_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
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


/******************************************************
 *
 *  object
 *
 */

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

/******************************************************
 *
 *  class
 *
 */

static void
bus_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  bus_t *this = (bus_t *) obj;

  fts_word_set_ptr(fts_atom_value(value), (void *)this->bus);
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

void
bus_config(void)
{
  fts_s_bus = fts_new_symbol("bus");

  bus_input_heap = fts_heap_new(sizeof(fts_bus_input_t));
  bus_output_heap = fts_heap_new(sizeof(fts_bus_output_t));
  bus_heap = fts_heap_new(sizeof(fts_bus_t));

  /* configuring  the classes */
  fts_class_install(fts_new_symbol("bus"), bus_instantiate);
}
