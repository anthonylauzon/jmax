/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"
#include "sampfilt.h"

sampfilt_t *sampfilt_tab = 0;

int 
make_sampfilt_tab(void)
{
  int i;

  if(sampfilt_tab) 
    return(1);
  
  sampfilt_tab = (sampfilt_t *) fts_malloc(SAMP_NPOINTS * sizeof(sampfilt_t));

  if ( !sampfilt_tab)
    return 0;

  for(i = 0; i < SAMP_NPOINTS; i++)
    {
      float f = i * (1.0f/SAMP_NPOINTS);

      sampfilt_tab[i].f1 = -.1666667f * f * (1-f) * (2-f);
      sampfilt_tab[i].f2 = .5f * (1+f) * (1-f) * (2-f);
      sampfilt_tab[i].f3 = .5f * (1+f) * f * (2-f);
      sampfilt_tab[i].f4 = -.1666667f * (1+f) * f * (1-f);
    }
  
  return(1);
}

