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
 */

/*
 * This file contains a template for platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 */

#include <fts/sys/hw.h>


/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

void *fts_dl_open( const char *filename, char *error)
{
  return 0;
}

int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error)
{
  return 1;
}

/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

void fts_enable_fpe_traps( void)
{
}

void fts_disable_fpe_traps( void)
{
}

unsigned int fts_check_fpe( void)
{
  return 0;
}

/* *************************************************************************** */
/*                                                                             */
/* Memory locking                                                              */
/*                                                                             */
/* *************************************************************************** */

int fts_unlock_memory( void)
{
  return 0;
}


/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

void fts_platform_init( int argc, char **argv)
{
}

