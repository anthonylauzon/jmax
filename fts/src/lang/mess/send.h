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
#ifndef _SEND_H_
#define _SEND_H_

extern fts_metaclass_t *fts_send_metaclass;
extern fts_metaclass_t *fts_receive_metaclass;

#define fts_object_is_send(o) ((o)->cl->mcl == fts_send_metaclass)
#define fts_object_is_receive(o) ((o)->cl->mcl == fts_receive_metaclass)

extern int fts_send_message_to_receives(fts_symbol_t name, fts_symbol_t selector, int ac, const fts_atom_t *at);
extern int fts_receive_exists(fts_symbol_t name);

extern fts_object_t *fts_send_get_first_receive(fts_object_t *send);
extern fts_object_t *fts_receive_get_next_receive(fts_object_t *receive);

#endif
