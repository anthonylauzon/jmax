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
 */
#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <fts/fts.h>

#ifdef WIN32
#if defined(SIGNAL_EXPORTS)
#define SIGNAL_API __declspec(dllexport)
#else
#define SIGNAL_API __declspec(dllimport)
#endif
#else
#define SIGNAL_API extern
#endif

SIGNAL_API void
signal_config(void);

#endif /* _SIGNAL_H */