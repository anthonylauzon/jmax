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
 * Author: Francois Dechelle.
 *
 */

/*
 * This file contains SGI implementation of hardware dependent initialization function.
 * 
 */

/* For sysmp(2) function */
#include <sys/types.h>
#include <sys/sysmp.h>
#include <sys/sysinfo.h>
#include <sys/pda.h>
#include <errno.h>
#include <string.h>

/* (fd) HACK !!! */
extern void post( const char *format, ...);

extern fts_real_time_on( void);
extern sgi_init_fpu( void);

static int run_on_cpu( int cpu)
{
  if (sysmp( MP_MUSTRUN, cpu) < 0)
    {
      post( "sysmp( MP_MUSTRUN, %d) failed [%d, \"%s\"]\n", cpu, strerror( errno));
      return -1;
    }

  return 0;
}

static int find_isolated_cpu( void)
{
  struct pda_stat *p;
  int n_processors, n;

  n_processors = sysmp( MP_NPROCS);
  if ( n_processors < 0)
    {
      post( "sysmp( MP_NPROCS) failed [%d, \"%s\"]\n", strerror( errno));
      return -1;
    }

  p = (struct pda_stat *)malloc( n_processors * sizeof( struct pda_stat));
  if ( sysmp( MP_STAT, p) < 0)
    {
      post( "sysmp( MP_STAT) failed [%d, \"%s\"]\n", strerror( errno));
      return -1;
    }

  for ( n = 1; n < n_processors; n++)
    {
      if (p[n].p_flags & PDAF_ISOLATED)
	return n;
    }

  return 0;
}

static int get_cpu_from_command_line( int argc, char **argv, int *pcpu)
{
  while ( argc)
    {
      if ( ! strncmp( *argv, "--cpu=", 6))
	{
	  char *p = strchr( *argv, '=') + 1;

	  if (sscanf( p, "%d", pcpu) == 1)
	    return 1;
	  else
	    {
	      post( "Invalid CPU number: %s\n", p);
	      return -1;
	    }
	}
      
      argc--;
      argv++;
    }

  return 0;
}

/*
  Function: fts_platform_init
  Description:
   performs platform specific initializations
  Arguments: none
  Returns: none
*/
void fts_platform_init( int argc, char **argv)
{
  int cpu = -1;
  int r;

  r = get_cpu_from_command_line( argc, argv, &cpu);

  if ( r == 1)
    {
      if ( run_on_cpu( cpu) >= 0)
	post( "Running on CPU %d\n", cpu);
    }
  else if ( r == 0)
    {
      cpu = find_isolated_cpu();

      if (cpu > 0 && run_on_cpu( cpu) >= 0)
	post( "Running on isolated CPU %d\n", cpu);
    }

  fts_real_time_on();

  /* Get rid of root privilege if we have them */
  if (setreuid(getuid(), getuid()) == -1)
    {
      /* Should we post a message ? */
    }

  sgi_init_fpu();
}

