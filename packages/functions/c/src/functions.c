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

#include <math.h>
#include <string.h>
#include <fts/fts.h>

#ifdef WIN32
#if defined(FUNCTIONS_EXPORTS)
#define FUNCTIONS_API __declspec(dllexport)
#else
#define FUNCTIONS_API __declspec(dllimport)
#endif
#else
#define FUNCTIONS_API extern
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif

FUNCTIONS_API void
functions_config(void);

/**********************************************************************
*
*  C functions
*
*/

#define DEFINE_FUN(FUN) \
static fts_status_t FUN##_function( int ac, const fts_atom_t *at, fts_atom_t *ret) \
{ \
  if (ac == 1 && fts_is_number( at)) \
    fts_set_float( ret, FUN( fts_get_number_float( at))); \
  return fts_ok;  \
}

#define INSTALL_FUN(FUN) fts_function_install( fts_new_symbol( #FUN), FUN##_function);

#define FUN DEFINE_FUN
#include "mathfuns.h"

/**********************************************************************
*
*  misc math functions
*
*/

static fts_status_t 
abs_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0 && fts_is_number( at))
    fts_set_float(ret, fabs(fts_get_number_float( at)));
  
  return fts_ok;
}

/**********************************************************************
*
*  random function
*
*/
#define RA 16807 /* multiplier */
#define RM 2147483647L /* 2**31 - 1 */
#define RQ 127773L /* m div a */
#define RR 2836 /* m mod a */

static unsigned int seed = 1;

static void
random_init(void)
{
#ifdef HAVE_SYS_TIME_H
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed = tv.tv_usec;
#else
  seed = time( NULL);
#endif		
}

static fts_status_t 
random_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  double min = 0.0;
  double max = 1.0;
  
  unsigned int hi;
  
  hi = RA * (long)((unsigned int)seed >> 16);
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
  
  if(ac > 0 && fts_is_number(at))
  {
    if(ac > 1 && fts_is_number(at + 1))
    {
      min = fts_get_number_float(at);
      max = fts_get_number_float(at + 1);
    }
    else
      max = fts_get_number_float(at);
  }
  
  fts_set_float(ret, min + (max - min) * ((double)seed / (double)RM));
  
  return fts_ok;
}

/**********************************************************************
*
*  symbol functions
*
*/
#define MAX_CONCAT_LENGTH 512

static fts_status_t
cat_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  char buf[MAX_CONCAT_LENGTH + 1];
  int n = 0;
  int i;
  
  buf[0] = '\0';
  buf[MAX_CONCAT_LENGTH] = '\0';
  
  for(i=0; i<ac && n<MAX_CONCAT_LENGTH; i++)
  {
    if(fts_is_int(at + i))
      snprintf(buf + n, MAX_CONCAT_LENGTH, "%d", fts_get_int(at + i));
    else if(fts_is_float(at + i))
      snprintf(buf + n, MAX_CONCAT_LENGTH, "%g", fts_get_float(at + i));
    else if(fts_is_symbol(at + i))
      strncpy(buf + n, fts_symbol_name(fts_get_symbol(at + i)), MAX_CONCAT_LENGTH - n);
    else
      continue;
    
    n = strlen(buf);
  }
  
  fts_set_symbol(ret, fts_new_symbol(buf));
  
  return fts_ok;
}

void
functions_config(void)
{
#undef FUN
#define FUN INSTALL_FUN
#include "mathfuns.h"
  random_init();
  
  fts_function_install( fts_new_symbol("abs"), abs_function);
  fts_function_install( fts_new_symbol("random"), random_function);
  fts_function_install( fts_new_symbol("cat"), cat_function);
}
