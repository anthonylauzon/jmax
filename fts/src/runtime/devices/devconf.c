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

/*
  function to install the correct device set for the given configuration
  */


#include "sys.h"
#include "runtime/devices/profdev.h"

#ifdef HAS_UNIX
#include "runtime/devices/unixdev.h"
#endif

#ifdef SGI
#include "runtime/devices/sgidev.h"
#endif

#ifdef HAS_OSS
#include "runtime/devices/ossdev.h"
#endif

extern void null_init(void);
extern void profdev_init(void);

#ifdef HAS_OSS
extern void ossdev_init(void);
#endif

extern void sfdev_init(void);

#ifdef SGI
extern void shmdev_init( void);
#endif

void fts_dev_configure(void)
{
  profdev_init();

#ifdef HAS_UNIX
  unixdev_init();
#endif

#ifdef SGI
  sgidev_init();
#endif

#ifdef HAS_OSS
  ossdev_init();
#endif

  null_init();
  sfdev_init();

#ifdef SGI
  shmdev_init();
#endif
}






