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
#ifndef _POST_H_
#define _POST_H_

/* definition file for  logging functions (post and co) */

extern void post_vector(int n, float *fp);
extern void post_atoms(int ac, const fts_atom_t *at);
extern void post(const char *format , ...); 

extern void post_error(fts_object_t *obj, const char *format , ...); 


#endif



