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
#ifndef _SAMPWRITE_H_
#define _SAMPWRITE_H_

#include "sampbuf.h"

typedef struct{
  long onset;
  sampbuf_t *buf;
} sampwrite_ctl_t;

extern void ftl_sampwrite(fts_word_t *argv);


#endif /* _SAMPWRITE_H_ */
