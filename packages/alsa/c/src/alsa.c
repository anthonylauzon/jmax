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

extern void alsaaudioport_config( void);

extern void alsarawmidiport_config( void);
extern void alsaseqmidiport_config(void);

extern void alsamidi_config( void);
extern void alsaseqmidi_config(void);

/* extern void alsaaudiomanager_config(void); */

void alsa_config(void)
{
  alsaaudioport_config();

  alsarawmidiport_config();
  alsaseqmidiport_config();

  alsamidi_config();
  alsaseqmidi_config();

/*   alsaaudiomanager_config(); */
}
