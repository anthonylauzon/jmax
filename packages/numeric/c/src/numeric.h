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

#ifndef _NUMERIC_H
#define _NUMERIC_H

#include <fts/fts.h>

#ifdef WIN32
#if defined(NUMERIC_EXPORTS)
#define NUMERIC_API __declspec(dllexport)
#else
#define NUMERIC_API __declspec(dllimport)
#endif
#else
#define NUMERIC_API extern
#endif

NUMERIC_API void
numeric_config(void);

NUMERIC_API void
random_config(void);

#endif /* _NUMERIC_H */
