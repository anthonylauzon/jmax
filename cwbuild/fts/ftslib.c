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
 * This file contains Mac OS X platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>
#include <ftsprivate/package.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/sequence.h>


static void
ftslib_set_project(void)
{
  fts_package_t *project = NULL;
  
  project = fts_package_new(fts_new_symbol("ftslib_global_project"));
  fts_package_add_data_path(project, fts_new_symbol(""));
  fts_project_set(project);
}

/***************************************************************************************
 *
 *  ftslib initialisation
 *
 */
static int ftslib_initialized = 0;

extern void fts_kernel_init(void);

void 
ftslib_init(void)
{
  if(ftslib_initialized == 0)
  {   
    ftslib_initialized = 1;

    fts_kernel_init();
    
    utils_config();
    data_config();
    sequence_config();
    
    ftslib_set_project();
  }
}