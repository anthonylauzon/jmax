/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
