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

#include <windows.h>

#include "mmio.h"
#include "wave.h"

void 
mmio_config(void);

/******************************************************************************/
/*                                                                            */
/* Module congiguration                                                         */
/*                                                                            */
/******************************************************************************/

fts_audiofile_loader_t mmio_loader = { 
  mmio_loader_open_write,
  mmio_loader_open_read,
  mmio_loader_write,
  mmio_loader_read,
  mmio_loader_seek,
  mmio_loader_close,
};

void 
mmio_config(void)
{
  fts_audiofile_set_loader("Windows Multimedia File I/O (MMIO)", &mmio_loader);
}

