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

#include <unistd.h>
#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"


static fts_symbol_t fts_search_path = 0;
static fts_symbol_t fts_project_dir = 0;

/*
   The default one is set with an UCS command,
   but also from the user interface.
 */

void
fts_set_search_path(fts_symbol_t search_path)
{
  fts_search_path = search_path;
}

fts_symbol_t 
fts_get_search_path()
{
  return fts_search_path;
}


void
fts_set_project_dir(fts_symbol_t project_dir)
{
  fts_project_dir = project_dir;
}

fts_symbol_t
fts_get_project_dir(void)
{
  if (fts_project_dir)
    return fts_project_dir;
  else
    {
      char buf[1024];

      return fts_new_symbol(getcwd(buf, 1024));
    }
}

