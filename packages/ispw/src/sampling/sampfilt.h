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
#ifndef _SAMPFILT_H_
#define _SAMPFILT_H_

#define SAMP_NBITS 8
#define SAMP_NPOINTS (1 << SAMP_NBITS)
#define SAMPFILTSIZE (SAMP_NPOINTS * sizeof(t_sampfilt))

typedef struct{
  float f1, f2, f3, f4;
} sampfilt_t;

extern sampfilt_t *sampfilt_tab; /* also used by delay */
extern int make_sampfilt_tab(void);

#endif /* _SIGTABLE_H_ */
