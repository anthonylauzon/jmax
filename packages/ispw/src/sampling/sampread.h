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
#ifndef _SAMPREAD_H_
#define _SAMPREAD_H_

#include "sampbuf.h"

typedef struct
{
  float     inv_n;
  float     last_in;		/* previous value of input */
  float     max_extent;		/* maximum span between last_in and new input */
  sampbuf_t *buf;
  float conv;			/* unit/samples conversion factor */
} sampread_ctl_t;

extern void ftl_sampread(fts_word_t *argv);

#endif /* _SAMPREAD_H_ */
