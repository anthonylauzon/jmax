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
 * Based on Max/ISPW by Miller Puckette.
 *
 */

/*
 * This file's authors: Francois Dechelle.
 */

#ifndef _FTS_DTDDEFS_H_
#define _FTS_DTDDEFS_H_

#define N_FIFOS 8

#define BLOCK_FRAMES  65536
#define BLOCK_MAX_CHANNELS  8
#define BLOCKS_PER_FIFO 4

#define PRELOAD_BLOCK_FRAMES  2048

#define DTD_SERVER_SELECT_TIMEOUT_SEC 0
#define DTD_SERVER_SELECT_TIMEOUT_USEC 250000

#define DTD_BASE_DIR "/tmp/ftsdtd"

#endif
