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
#include "asio.h"
#include "asiosys.h"
#include "jmax_asio.h"
#include "jmax_asio_util.h"

/*
  Asio_manager look for ASIO driver available on host.
  For each ASIO Driver it will create and register an asio_audioport
*/
void asio_manager_config()
{
  /* scan for available ASIO Driver */
  asio_util_scan_drivers_c();
}

ASIO_API asio_audioport_config();

void asio_config()
{

  fts_post("----------------------------------------------------------\n"
	         " ASIO is a trademark of Steinberg Soft-und Hardware GmbH,\n"
	         "----------------------------------------------------------\n");
  asio_audioport_config();
  asio_manager_config();


}

