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
#ifndef _FPE_H_
#define _FPE_H_

/*
 * FPE handling and collection support 
 */

extern void fts_fpe_start_collect( fts_object_set_t *set);
extern void fts_fpe_stop_collect( void);
extern void fts_fpe_empty_collection( void);
extern void fts_fpe_add_object( fts_object_t *object);

/* For memory:
#define isnanf(x)	(((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )
*/

#endif
