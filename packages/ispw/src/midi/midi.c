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

/*
 * MIDI Objects; a new implementation of the old FTS objects (written by Miller Puckette)
 * based on the new FTS MIDI  parser substrate.
 */


#include "fts.h"

/* Utility macro to map a channel number 
 in a 1-16 range; note that for compatibility 
 with old patches, we define ch 17-32 mapped on channels
 1-16, for every midi stream; also the range control
 is done with rules to assure compatibility with old
 patches with wrong numbers.
*/

#define RANGE_CH(n)    ((n) < 1 ? 1 : ((n) > 16 ? (((n) <= 32) ? ((n) - 16) : 16) : (n)))
#define RANGE_VALUE(n)  ((n) < 0 ? 0 : (((n) > 127) ? 127 : (n)))

#define SNOTE 0x90
#define SPOLY 0xa0
#define SCTL 0xb0
#define SPGM 0xc0
#define STCH 0xd0
#define SBEND 0xe0

/* Common equiv function */


static int
midi_ch_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  long a1_0, a1_1;

  if (ac0 >= 2)
    a1_0 = fts_get_long(&at0[1]);
  else
    a1_0 = 0;

  if (ac1 >= 2)
    a1_1 = fts_get_long(&at1[1]);
  else
    a1_1 = 0;

  if (a1_0 && (! a1_1))
    return 0;

  if (a1_1 && (! a1_0))
    return 0;

  return 1;
}



static int
midi_ch_arg_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  long a1_0, a1_1, a2_0, a2_1;

  if (ac0 >= 2)
    a1_0 = fts_get_long(&at0[1]);
  else
    a1_0 = 0;

  if (ac1 >= 2)
    a1_1 = fts_get_long(&at1[1]);
  else
    a1_1 = 0;

  if (ac0 >= 3)
    a2_0 = fts_get_long(&at0[2]);
  else
    a2_0 = 0;

  if (ac1 >= 3)
    a2_1 = fts_get_long(&at1[2]);
  else
    a2_1 = 0;

  if (a1_0 && (! a1_1))
    return 0;

  if (a1_1 && (! a1_0))
    return 0;

  if (a2_0 && (! a2_1))
    return 0;

  if (a2_1 && (! a2_0))
    return 0;

  return 1;
}


/* 1. program change */

typedef struct 
{
  fts_object_t ob;
  long status;		/* status byte to send */
  fts_midi_port_t *port;
} pgmout_t;

static void
pgmout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmout_t *x = (pgmout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, RANGE_VALUE(n - 1));
}

static void
pgmout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmout_t *x = (pgmout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (SPGM + RANGE_CH(n) - 1);
}

static void
pgmout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 1)
    pgmout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    pgmout_number(o, 2, s, 1, at + 0);
}

static void
pgmout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmout_t *x = (pgmout_t *)o;
  long int ch   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  x->status = (SPGM + RANGE_CH(ch) - 1);
}

static fts_status_t
pgmout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(pgmout_t), 2, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, pgmout_init, 3, a, 1);

  /* Pgmout args */

  fts_method_define_varargs(cl, 0, fts_s_list,  pgmout_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   pgmout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, pgmout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   pgmout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, pgmout_number_1, 1, a);

  return fts_Success;
}

void
pgmout_config(void)
{
  fts_class_install(fts_new_symbol("pgmout"),pgmout_instantiate);
}

/* --- */

typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  fts_midi_port_t *port;
} pgmin_t;

static void
pgmin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  pgmin_t *x = (pgmin_t *) user_data;

  if (! x->chanout)
    fts_outlet_send((fts_object_t *)x, 1, fts_s_int, 1, at);

  fts_outlet_send((fts_object_t *)x, 0, fts_s_int, 1, at + 1);
}


static void
pgmin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmin_t *x = (pgmin_t *)o;
  long int n   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_PROGRAM_CHANGE_CH(x->chanout), pgmin_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_PROGRAM_CHANGE, pgmin_midi_action, (void *)x);
    }
}

static void
pgmin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmin_t *x = (pgmin_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_PROGRAM_CHANGE_CH(x->chanout), pgmin_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_PROGRAM_CHANGE, pgmin_midi_action, (void *)x);
}



static fts_status_t
pgmin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_class_init(cl, sizeof(pgmin_t), 0, 2, 0);
  else
    fts_class_init(cl, sizeof(pgmin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, pgmin_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, pgmin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_outlet_type_define(cl, 1,  fts_s_int, 1, a);

  return fts_Success;
}

static void
pgmin_config(void)
{
  fts_metaclass_install(fts_new_symbol("pgmin"),pgmin_instantiate, midi_ch_equiv);
}

/* 2. pitch bend */

typedef struct 
{
  fts_object_t ob;
  long status;		/* status byte to send */
  fts_midi_port_t *port;
} bendout_t;

static void
bendout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bendout_t *x = (bendout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  n = RANGE_VALUE(n);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, n);
  fts_midi_send(x->port, n);	/* actually, we intend to duplicate the byte */
}

static void
bendout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bendout_t *x = (bendout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (SBEND + RANGE_CH(n) - 1);
}

static void
bendout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 1)
    bendout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    bendout_number(o, 2, s, 1, at + 0);
}

static void
bendout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bendout_t *x = (bendout_t *)o;
  long int ch   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  x->status = (SBEND + RANGE_CH(ch) - 1);
}

static fts_status_t
bendout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(bendout_t), 2, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, bendout_init, 3, a, 1);


  /* Bendout args */

  fts_method_define_varargs(cl, 0, fts_s_list,  bendout_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   bendout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, bendout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   bendout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float,   bendout_number_1, 1, a);

  return fts_Success;
}


void
bendout_config(void)
{
  fts_class_install(fts_new_symbol("bendout"),bendout_instantiate);
}


typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  fts_midi_port_t *port;
} bendin_t;

static void
bendin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  bendin_t *x = (bendin_t *) user_data;

  if (! x->chanout)
    fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at);

  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 2);
}


static void
bendin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pgmin_t *x = (pgmin_t *)o;
  long int n   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_PITCH_BEND_CH(x->chanout), bendin_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_PITCH_BEND, bendin_midi_action, (void *)x);
    }
}


static void
bendin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bendin_t *x = (bendin_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_PITCH_BEND_CH(x->chanout), bendin_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_PITCH_BEND, bendin_midi_action, (void *)x);
}

static fts_status_t
bendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_class_init(cl, sizeof(bendin_t), 0, 2, 0);
  else
    fts_class_init(cl, sizeof(bendin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, bendin_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, bendin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_outlet_type_define(cl, 1,  fts_s_int, 1, a);

  return fts_Success;
}

static void
bendin_config(void)
{
  fts_metaclass_install(fts_new_symbol("bendin"),bendin_instantiate, midi_ch_equiv);
}

/* 3. channel aftertouch */

typedef struct touchout
{
  fts_object_t ob;
  long status;		/* status byte to send */
  fts_midi_port_t *port;
} touchout_t;

static void
touchout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  touchout_t *x = (touchout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, RANGE_VALUE(n));
}

static void
touchout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  touchout_t *x = (touchout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (STCH + RANGE_CH(n) - 1);
}

static void
touchout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 1)
    touchout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    touchout_number(o, 2, s, 1, at + 0);
}


static void
touchout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  touchout_t *x = (touchout_t *)o;
  long int ch   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  x->status = (STCH + RANGE_CH(ch) - 1);
}

static fts_status_t
touchout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(touchout_t), 2, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, touchout_init, 3, a, 1);


  /* Touchout args */

  fts_method_define_varargs(cl, 0, fts_s_list,  touchout_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   touchout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, touchout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   touchout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, touchout_number_1, 1, a);

  return fts_Success;
}


void
touchout_config(void)
{
  fts_class_install(fts_new_symbol("touchout"),touchout_instantiate);
}


typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  fts_midi_port_t *port;
} touchin_t;

static void
touchin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  touchin_t *x = (touchin_t *) user_data;

  if (! x->chanout)
    fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at);

  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 1);
}


static void
touchin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  touchin_t *x = (touchin_t *)o;
  long int n   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_CHANNEL_AFTERTOUCH_CH(x->chanout), touchin_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_CHANNEL_AFTERTOUCH, touchin_midi_action, (void *)x);
    }
}


static void
touchin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  touchin_t *x = (touchin_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_CHANNEL_AFTERTOUCH_CH(x->chanout), touchin_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_CHANNEL_AFTERTOUCH, touchin_midi_action, (void *)x);
}

static fts_status_t
touchin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_class_init(cl, sizeof(touchin_t), 0, 2, 0);
  else
    fts_class_init(cl, sizeof(touchin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, touchin_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, touchin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_outlet_type_define(cl, 1,  fts_s_int, 1, a);

  return fts_Success;
}


static void
touchin_config(void)
{
  fts_metaclass_install(fts_new_symbol("touchin"),touchin_instantiate, midi_ch_equiv);
}


/* 4. midi control */

typedef struct ctlout
{
  fts_object_t ob;
  long status;			/* status byte to send */
  long ctlno;			/* control number */
  fts_midi_port_t *port;	/* the midi port */
} ctlout_t;


static void
ctlout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlout_t *x = (ctlout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, x->ctlno);
  fts_midi_send(x->port, RANGE_VALUE(n));
}

static void
ctlout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlout_t *x = (ctlout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->ctlno = RANGE_VALUE(n);
}


static void
ctlout_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlout_t *x = (ctlout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (SCTL + RANGE_CH(n) - 1);
}

static void
ctlout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 2)
    ctlout_number_2(o, 2, s, 1, at + 2);

  if (ac > 1)
    ctlout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    ctlout_number(o, 2, s, 1, at + 0);
}

static void
ctlout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlout_t *x = (ctlout_t *)o;
  long int ctlno = fts_get_long_arg(ac, at, 1, 0);
  long int ch    = fts_get_long_arg(ac, at, 2, 0);
  long int idx  = fts_get_long_arg(ac, at, 3, 0);

  x->port = fts_midi_get_port(idx);
  x->ctlno = RANGE_VALUE(ctlno);
  x->status = (STCH + RANGE_CH(ch) - 1);
}

static fts_status_t
ctlout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  /* initialize the class */

  fts_class_init(cl, sizeof(ctlout_t), 3, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, ctlout_init, 4, a, 1);

  /* Ctlout args */

  fts_method_define_varargs(cl, 0, fts_s_list,  ctlout_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   ctlout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, ctlout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   ctlout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, ctlout_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int,   ctlout_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, ctlout_number_2, 1, a);

  return fts_Success;
}


void
ctlout_config(void)
{
  fts_class_install(fts_new_symbol("ctlout"),ctlout_instantiate);
}


typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  long ctlno;			/* ctl number  */
  fts_midi_port_t *port;
} ctlin_t;

static void
ctlin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  ctlin_t *x = (ctlin_t *) user_data;

  if (x->ctlno == -1)
    {
      if (! x->chanout)
	fts_outlet_send((fts_object_t *) x, 2, fts_s_int, 1, at);

      fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at + 1);
      fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 2);
    }
  else if (x->ctlno == fts_get_long(at + 1))
    {
      if (! x->chanout)
	fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at);

      fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 2);
    }
}


static void
ctlin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlin_t *x   = (ctlin_t *)o;
  long int ctl = fts_get_long_arg(ac, at, 1, 0);
  long int n   = fts_get_long_arg(ac, at, 2, 0);
  long int idx = fts_get_long_arg(ac, at, 3, 0);

  /* controller range from 1 to 128 in this object;
     0 means send out the number */

  if (ctl == 0)
    x->ctlno = -1;
  else if (ctl == 128)		/* backward compatibility */
    x->ctlno = 0;
  else
    x->ctlno = RANGE_VALUE(ctl);
    
  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_CONTROLLER_CH(x->chanout), ctlin_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_CONTROLLER, ctlin_midi_action, (void *)x);
    }
}

static void
ctlin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ctlin_t *x = (ctlin_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_CONTROLLER_CH(x->chanout), ctlin_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_CONTROLLER, ctlin_midi_action, (void *)x);
}



static fts_status_t
ctlin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  int outlets, i;

  /* computes how many outlets */

  if ((ac == 1) ||
      ((ac == 2) && (fts_get_long(&at[1]) == 0)) ||
      ((ac >= 3) && (fts_get_long(&at[1]) == 0) && (fts_get_long(&at[2]) == 0)))
    outlets = 3;
  else if ((ac == 2) ||
	   ((ac >= 3) && (fts_get_long(&at[2]) == 0)))
    outlets = 2;
  else if (ac >= 3)
    outlets = 1;

  /* initialize the class */

  fts_class_init(cl, sizeof(ctlin_t), 0, outlets, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, ctlin_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, ctlin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  
  for (i = 0; i < outlets; i++)
    fts_outlet_type_define(cl, i,  fts_s_int, 1, a);

  return fts_Success;
}

static void
ctlin_config(void)
{
  fts_metaclass_install(fts_new_symbol("ctlin"),ctlin_instantiate, midi_ch_arg_equiv);
}


/* 5. notes */

typedef struct 
{
  fts_object_t ob;
  long status;			/* status byte to send */
  long vel;			/* velocity */
  fts_midi_port_t *port;	/* the midi port */
} noteout_t;


static void
noteout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteout_t *x = (noteout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, RANGE_VALUE(n));
  fts_midi_send(x->port, x->vel);
}

static void
noteout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteout_t *x = (noteout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->vel = RANGE_VALUE(n);
}


static void
noteout_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteout_t *x = (noteout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (SNOTE + RANGE_CH(n) - 1);
}

static void
noteout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 2)
    noteout_number_2(o, 2, s, 1, at + 2);

  if (ac > 1)
    noteout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    noteout_number(o, 2, s, 1, at + 0);
}


static void
noteout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteout_t *x = (noteout_t *)o;
  long int ch    = fts_get_long_arg(ac, at, 1, 0);
  long int idx  = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  x->status = (SNOTE + RANGE_CH(ch) - 1);
}

static fts_status_t
noteout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(noteout_t), 3, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, noteout_init, 3, a, 1);

  /* Noteout args */

  fts_method_define_varargs(cl, 0, fts_s_list,  noteout_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   noteout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, noteout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   noteout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, noteout_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int,   noteout_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, noteout_number_2, 1, a);

  return fts_Success;
}


void
noteout_config(void)
{
  fts_class_install(fts_new_symbol("noteout"),noteout_instantiate);
}


typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  fts_midi_port_t *port;
} notein_t;

static void
notein_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  notein_t *x = (notein_t *) user_data;

  if (! x->chanout)
    fts_outlet_send((fts_object_t *) x, 2, fts_s_int, 1, at);

  fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at + 2);
  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 1);
}


static void
notein_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  notein_t *x = (notein_t *)o;
  long int n   = fts_get_long_arg(ac, at, 1, 0);
  long int idx = fts_get_long_arg(ac, at, 2, 0);

  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_NOTE_CH(x->chanout), notein_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_NOTE, notein_midi_action, (void *)x);
    }
}

static void
notein_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  notein_t *x = (notein_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_NOTE_CH(x->chanout), notein_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_NOTE, notein_midi_action, (void *)x);
}



static fts_status_t
notein_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_class_init(cl, sizeof(notein_t), 0, 3, 0);
  else
    fts_class_init(cl, sizeof(notein_t), 0, 2, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, notein_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, notein_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,  fts_s_int, 1, a);

  if ((ac == 1) || (fts_get_long(&at[1]) == 0))
    fts_outlet_type_define(cl, 2,  fts_s_int, 1, a);

  return fts_Success;
}

static void
notein_config(void)
{
  fts_metaclass_install(fts_new_symbol("notein"),notein_instantiate, midi_ch_equiv);
}

/* DDZ 7. polyphonic keypressure */

typedef struct 
{
  fts_object_t ob;
  long status;			/* status byte to send */
  long keyno;			/* pressure value */
  fts_midi_port_t *port;	/* the midi port */
} polyout_t;


static void
polyout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyout_t *x = (polyout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, x->status);
  fts_midi_send(x->port, x->keyno);
  fts_midi_send(x->port, RANGE_VALUE(n));
}

static void
polyout_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyout_t *x = (polyout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->keyno = RANGE_VALUE(n);
}


static void
polyout_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyout_t *x = (polyout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  x->status = (SPOLY + RANGE_CH(n) - 1);
}

static void
polyout_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 2)
    polyout_number_2(o, 2, s, 1, at + 2);

  if (ac > 1)
    polyout_number_1(o, 2, s, 1, at + 1);

  if (ac > 0)
    polyout_number(o, 2, s, 1, at + 0);
}

static void
polyout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyout_t *x = (polyout_t *)o;
  long int keyno = fts_get_long_arg(ac, at, 1, 0);
  long int ch    = fts_get_long_arg(ac, at, 2, 0);
  long int idx  = fts_get_long_arg(ac, at, 3, 0);

  x->port = fts_midi_get_port(idx);
  x->keyno = RANGE_VALUE(keyno);
  x->status = (SPOLY + RANGE_CH(ch) - 1);
}

static fts_status_t
polyout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  /* initialize the class */

  fts_class_init(cl, sizeof(polyout_t), 3, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, polyout_init, 4, a, 1);

  /* Polyout args */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_list,  polyout_list, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   polyout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, polyout_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   polyout_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, polyout_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int,   polyout_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, polyout_number_2, 1, a);

  return fts_Success;
}

void
polyout_config(void)
{
  fts_class_install(fts_new_symbol("polyout"),polyout_instantiate);
}


/* polyphonic key pressure in */

typedef struct 
{
  fts_object_t obj;
  int chanout;			/* MIDI channel if any (zero otherwise) */
  long keyno;			/* key number  */
  fts_midi_port_t *port;
} polyin_t;

static void
polyin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  polyin_t *x = (polyin_t *) user_data;

  if (x->keyno == -1)
    {
      if (! x->chanout)
	fts_outlet_send((fts_object_t *) x, 2, fts_s_int, 1, at);

      fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at + 1);
      fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 2);
    }
  else if (x->keyno == fts_get_long(at + 1))
    {
      if (! x->chanout)
	fts_outlet_send((fts_object_t *) x, 1, fts_s_int, 1, at);

      fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at + 2);
    }
}


static void
polyin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyin_t *x   = (polyin_t *)o;
  long int key = fts_get_long_arg(ac, at, 1, 0);
  long int n   = fts_get_long_arg(ac, at, 2, 0);
  long int idx = fts_get_long_arg(ac, at, 3, 0);

  /* controller range from 1 to 128 in this object;
     0 means send out the number */

  if (key == 0)
    x->keyno = -1;
  else if (key == 128)		/* backward compatibility */
    x->keyno = 0;
  else
    x->keyno = RANGE_VALUE(key);
    
  x->port = fts_midi_get_port(idx);
  
  if (n)
    {
      x->chanout = RANGE_CH(n);
      fts_midi_install_fun(x->port, FTS_MIDI_POLY_AFTERTOUCH_CH(x->chanout), polyin_midi_action, (void *)x);
    }
  else
    {
      x->chanout = 0;
      fts_midi_install_fun(x->port, FTS_MIDI_POLY_AFTERTOUCH, polyin_midi_action, (void *)x);
    }
}

static void
polyin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  polyin_t *x = (polyin_t *)o;

  if (x->chanout)
    fts_midi_deinstall_fun(x->port, FTS_MIDI_POLY_AFTERTOUCH_CH(x->chanout), polyin_midi_action, (void *)x);
  else
    fts_midi_deinstall_fun(x->port, FTS_MIDI_POLY_AFTERTOUCH, polyin_midi_action, (void *)x);
}



static fts_status_t
polyin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  int outlets, i;

  /* computes how many outlets */

  if ((ac == 1) ||
      ((ac == 2) && (fts_get_long(&at[1]) == 0)) ||
      ((ac >= 3) && (fts_get_long(&at[1]) == 0) && (fts_get_long(&at[2]) == 0)))
    outlets = 3;
  else if ((ac == 2) ||
	   ((ac >= 3) && (fts_get_long(&at[2]) == 0)))
    outlets = 2;
  else if (ac >= 3)
    outlets = 1;

  /* initialize the class */

  fts_class_init(cl, sizeof(polyin_t), 0, outlets, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, polyin_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, polyin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  
  for (i = 0; i < outlets; i++)
    fts_outlet_type_define(cl, i,  fts_s_int, 1, a);

  return fts_Success;
}

static void
polyin_config(void)
{
  fts_metaclass_install(fts_new_symbol("polyin"),polyin_instantiate, midi_ch_arg_equiv);
}



/* midi exclusive in, bytewise in and out */

typedef struct 
{
  fts_object_t ob;
  fts_midi_port_t *port;	/* the midi port */
} midiout_t;


static void
midiout_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *x = (midiout_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);

  fts_midi_send(x->port, n);
}

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *x = (midiout_t *)o;
  long int idx  = fts_get_long_arg(ac, at, 1, 0);

  x->port = fts_midi_get_port(idx);
}

static fts_status_t
midiout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(midiout_t), 1, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, midiout_init, 2, a, 1);

  /* Midiout args */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   midiout_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, midiout_number, 1, a);

  return fts_Success;
}

void
midiout_config(void)
{
  fts_class_install(fts_new_symbol("midiout"),midiout_instantiate);
}

typedef struct 
{
  fts_object_t obj;
  fts_midi_port_t *port;
} midiin_t;

static void
midiin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  midiin_t *x = (midiin_t *) user_data;

  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at);
}


static void
midiin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiin_t *x = (midiin_t *)o;
  long int idx = fts_get_long_arg(ac, at, 1, 0);

  x->port = fts_midi_get_port(idx);
  
  fts_midi_install_fun(x->port, FTS_MIDI_BYTE, midiin_midi_action, (void *)x);
}


static void
midiin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiin_t *x = (midiin_t *)o;

  fts_midi_deinstall_fun(x->port, FTS_MIDI_BYTE, midiin_midi_action, (void *)x);
}

static fts_status_t
midiin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(midiin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, midiin_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, midiin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  return fts_Success;
}


static void
midiin_config(void)
{
  fts_metaclass_install(fts_new_symbol("midiin"),midiin_instantiate, midi_ch_equiv);
}



typedef struct 
{
  fts_object_t obj;
  fts_midi_port_t *port;
} sysexin_t;

static void
sysexin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  sysexin_t *x = (sysexin_t *) user_data;

  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at);
}


static void
sysexin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysexin_t *x = (sysexin_t *)o;
  long int idx = fts_get_long_arg(ac, at, 1, 0);

  x->port = fts_midi_get_port(idx);
  
  fts_midi_install_fun(x->port, FTS_MIDI_SYSEX, sysexin_midi_action, (void *)x);
}


static void
sysexin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysexin_t *x = (sysexin_t *)o;

  fts_midi_deinstall_fun(x->port, FTS_MIDI_SYSEX, sysexin_midi_action, (void *)x);
}

static fts_status_t
sysexin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(sysexin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sysexin_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sysexin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  return fts_Success;
}


static void
sysexin_config(void)
{
  fts_metaclass_install(fts_new_symbol("sysexin"),sysexin_instantiate, midi_ch_equiv);
}


typedef struct 
{
  fts_object_t obj;
  fts_midi_port_t *port;
} rtin_t;

static void
rtin_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  rtin_t *x = (rtin_t *) user_data;

  fts_outlet_send((fts_object_t *) x, 0, fts_s_int, 1, at);
}


static void
rtin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rtin_t *x = (rtin_t *)o;
  long int idx = fts_get_long_arg(ac, at, 1, 0);

  x->port = fts_midi_get_port(idx);
  
  fts_midi_install_fun(x->port, FTS_MIDI_REALTIME, rtin_midi_action, (void *)x);
}


static void
rtin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rtin_t *x = (rtin_t *)o;

  fts_midi_deinstall_fun(x->port, FTS_MIDI_REALTIME, rtin_midi_action, (void *)x);
}

static fts_status_t
rtin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(rtin_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, rtin_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, rtin_delete, 0, 0);

  /* Type the outlets  */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,  fts_s_int, 1, a);

  return fts_Success;
}


static void
rtin_config(void)
{
  fts_metaclass_install(fts_new_symbol("rtin"),rtin_instantiate, midi_ch_equiv);
}

/* mtc */

typedef struct 
{
  fts_object_t obj;

  fts_midi_port_t *port;
} mtc_t;



static void
mtc_midi_action(fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at)
{
  mtc_t *x = (mtc_t *) user_data;

  fts_outlet_send((fts_object_t *) x, 0, fts_s_list, 4, at + 1);	/* skip the mtc type */
}


static void
mtc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtc_t *x = (mtc_t *)o;
  long int idx = fts_get_long_arg(ac, at, 1, 0);

  x->port = fts_midi_get_port(idx);
  
  fts_midi_install_fun(x->port, FTS_MIDI_MTC, mtc_midi_action, (void *)x);
}


static void
mtc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtc_t *x = (mtc_t *)o;

  fts_midi_deinstall_fun(x->port, FTS_MIDI_MTC, mtc_midi_action, (void *)x);
}


static fts_status_t
mtc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(mtc_t), 0, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, mtc_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, mtc_delete, 0, 0);

  /* Type the outlets  */

  fts_outlet_type_define_varargs(cl, 0,  fts_s_list);

  return fts_Success;
}


static void
mtc_config(void)
{
  fts_metaclass_install(fts_new_symbol("mtc"), mtc_instantiate, midi_ch_equiv);
}



/* end mtc */

/* global conf function  */

void
midi_config(void)
{
  pgmout_config();
  pgmin_config();
  bendout_config();
  bendin_config();
  touchout_config();
  touchin_config();
  ctlout_config();
  ctlin_config();
  noteout_config();
  notein_config();
  polyout_config();
  polyin_config();
  midiout_config();
  midiin_config();
  sysexin_config();
  rtin_config();
  mtc_config();
}
