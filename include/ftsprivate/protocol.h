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

#ifndef _FTS_PRIVATE_PROTOCOL_H_
#define _FTS_PRIVATE_PROTOCOL_H_

/**
 * Protocol value tags
 */

#define FTS_PROTOCOL_INT		0x01
#define FTS_PROTOCOL_FLOAT		0x02
#define FTS_PROTOCOL_SYMBOL_INDEX	0x03
#define FTS_PROTOCOL_SYMBOL_CACHE	0x04
#define FTS_PROTOCOL_STRING		0x05
#define FTS_PROTOCOL_OBJECT		0x06
#define FTS_PROTOCOL_END_OF_MESSAGE	0x0F

#endif
