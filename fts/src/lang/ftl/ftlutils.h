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
#ifndef _FTLUTILS_H_
#define _FTLUTILS_H_

typedef struct ftl_memory_declaration {
  int size;
  void *address;
} ftl_memory_declaration;

extern void ftl_memory_declaration_init( ftl_memory_declaration *decl, int size);
extern ftl_memory_declaration *ftl_memory_declaration_new( int size );


typedef struct ftl_function_declaration {
  ftl_wrapper_t wrapper;
} ftl_function_declaration;

extern void ftl_function_declaration_init( ftl_function_declaration *decl, ftl_wrapper_t wrapper);
extern ftl_function_declaration *ftl_function_declaration_new( ftl_wrapper_t wrapper);
extern void ftl_function_declaration_free( ftl_function_declaration *decl);
#endif
