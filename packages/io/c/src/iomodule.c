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

#include "iomodule.h"

extern void filestream_config(void);
extern void in_config(void);
extern void in_out_tilda_config( void);
extern void loopback_config(void);
extern void out_config(void);
extern void profileaudioport_config( void);
extern void serial_config( void);
extern void udp_config(void);
extern void udpreceive_config(void);
extern void udpsend_config(void);
extern void wacom_config(void);

void
io_config(void)
{
  filestream_config();
  in_config();
  in_out_tilda_config();
  loopback_config();
  out_config();
  udp_config();
#ifndef WIN32
  udp_config();
  udpreceive_config();
  udpsend_config();

/* serial devices are only available on linux ? */
#ifdef linux
  serial_config();
#endif /* linux */

#if 0 
  wacom_config();
#endif /* no yet tested */
 
#endif /* WIN32 */
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
