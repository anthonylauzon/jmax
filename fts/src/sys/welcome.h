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

#ifndef _WELCOME_H_
#define _WELCOME_H_

typedef struct _welcome
{
  const char *msg;
  struct _welcome *next;
} fts_welcome_t;

extern void fts_add_welcome(fts_welcome_t *w);
extern void fts_welcome_apply(void (* f)(const char *));

#endif
