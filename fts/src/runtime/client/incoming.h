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
#ifndef _INCOMING_H_
#define _INCOMING_H_

void fts_client_incoming_init(void);
void fts_client_incoming_restart(void);
void fts_client_parse_char(char c);
void fts_client_mess_install(char type, void (* fun) (int, const fts_atom_t *));

#endif
