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
 * Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell.
 *
 */

/*
 * 31 bit random number generator
 *
 * Based on Ray Garder's public domain code
 * found at http://www.snippets.org/snippets/RG_RAND.C.
 * 
 * Based on "Random Number Generators: Good Ones Are Hard to Find",
 * S.K. Park and K.W. Miller, Communications of the ACM 31:10 (Oct 1988),
 * and "Two Fast Implementations of the 'Minimal Standard' Random
 * Number Generator", David G. Carta, Comm. ACM 33, 1 (Jan 1990), p. 87-88
 *
 * Linear congruential generator: f(z) = (16807 * z) mod (2 ** 31 - 1)
 *
 * Uses L. Schrage's method to avoid overflow problems.
 *
 */

/* just another one to remind: seed = 1664525 * seed + 1013904223; */

#define RA 16807 /* multiplier */
#define RM 2147483647L /* 2**31 - 1 */
#define RQ 127773L /* m div a */
#define RR 2836 /* m mod a */

static unsigned long seed = 1;

void
fts_random_set_seed(unsigned long ul)
{
  seed = ul;
}

unsigned long
fts_random(void)
{
  unsigned long lo, hi;

  hi = RA * (long)((unsigned long)seed >> 16);
  seed = RA * (long)(seed & 0xFFFF);
  
  seed += (hi & 0x7FFF) << 16;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  seed += hi >> 15;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  return seed;
}

/**********************************************************************
 *
 * float random between -1. and 1.
 *
 */

#if defined(HAS_IEEE_FLOAT)

typedef union
{
  long i;
  float f;
} intfloat_t;

float
fts_random_float(void)
{
  unsigned long lo, hi;
  intfloat_t x;

  hi = RA * (long)((unsigned long)seed >> 16);
  seed = RA * (long)(seed & 0xFFFF);
  
  seed += (hi & 0x7FFF) << 16;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  seed += hi >> 15;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  x.i = (seed >> 9) | 0x40800000;
  
  return x.f - 5.0;
}

#else

float
fts_random_float(void)
{
  unsigned long lo, hi;

  hi = RA * (long)((unsigned long)seed >> 16);
  seed = RA * (long)(seed & 0xFFFF);
  
  seed += (hi & 0x7FFF) << 16;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  seed += hi >> 15;
  
  if (seed > RM)
    {
      seed &= RM;
      ++seed;
    }
  
  return (2.0 * seed / RM) - 1.0;
}

#endif



