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

#include <fts/fts.h>

static fts_status_description_t fts_ignore_description = {""};
fts_status_t fts_ignore = &fts_ignore_description;

fts_status_t
fts_status_new(fts_symbol_t description)
{
  fts_status_t status = (fts_status_t)fts_malloc(sizeof(fts_status_description_t));

  status->description = fts_symbol_name(description);

  return status;
}

fts_status_t
fts_status_format(const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_symbol_t error;
  
  /* make up the errdesc property  */
  va_start(ap, format);
  vsnprintf(buf, 1024, format, ap);
  va_end(ap);
  
  error = fts_new_symbol(buf);
  
  return fts_status_new(fts_new_symbol(buf));  
}

