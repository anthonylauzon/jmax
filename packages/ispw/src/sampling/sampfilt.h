/*
 * jMax
 * Copyright (C) 1999 by IRCAM
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
