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

#include <fts/fts.h>

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/patfile.h>

static fts_status_description_t invalid_file_format_status_description = {
  "Invalid file format"
};
fts_status_t fts_invalid_file_format_error = &invalid_file_format_status_description;

static fts_status_description_t cannot_open_file_status_description = {
  "Cannot open file"
};
fts_status_t fts_cannot_open_file_error = &cannot_open_file_status_description;


fts_status_t fts_file_load( fts_symbol_t filename, fts_object_t *parent, int ac, const fts_atom_t *at, fts_object_t **ret)
{
  fts_status_t status;

  status = fts_bmax_file_load( filename, parent, ac, at, ret);

  if (status == fts_ok || status != fts_invalid_file_format_error)
    return status;

  status = fts_pat_file_load( filename, parent, ac, at, ret);

  return status;
}


