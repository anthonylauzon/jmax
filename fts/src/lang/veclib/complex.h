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

/* mathtypes.h */

#ifndef _MATHTYPES_H_
#define _MATHTYPES_H_

#define MAG re
#define ARG im
#define RE re
#define IM im

typedef struct{
  float re, im;
} complex;

extern complex CZERO;

#endif /* _MATHTYPES_H_ */

