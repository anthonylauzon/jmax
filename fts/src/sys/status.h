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
#ifndef _STATUS_H_
#define _STATUS_H_

/*

   General status return values for FTS.

*/

typedef struct 
{
  const char *description;
} fts_status_description_t;


typedef fts_status_description_t *fts_status_t;

#define fts_Success ((fts_status_t) 0)

#endif
