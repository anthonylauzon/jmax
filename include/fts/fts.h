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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_H
#define _FTS_H

/**
 * Global FTS include file. 
 * Object programmer just need to include this file as:
 *
 * @code
 *   #include <fts/fts.h>
 * @endcode
 *
 * @file fts.h
 */

/**
 * @mainpage
 * 
 * This will be the developers documentation of the FTS API.
 * For now there are only a few documentation modules.
 *
 * FTS includes mainly the following components:
 *
 * \li object system (lang/mess/)
 * \li DSP system (lang/dsp/, lang/ftl/)
 * \li scheduler (runtime/scheduler/)
 * \li timing system (runtime/time/)
 * \li device abstractions (runtime/devices/)
 * \li file handling (runtime/files/)
 * \li client/server communication (runtime/client/, tiles/messtile.*)
 * \li configuration handling: UCS (tiles/ucs.*)
 * \li system utilities: memory management, module sytem, ... (sys/)
 * \li platform dependend components: dynamic loading, memory management, scheduling, ... (non-portable/)
 * \li misc utilities: hash tables, sampling rate conversion routines, four-point interpolation, ... (lang/utils/)
 *
 * This documentation is generated automatically from the comments in the header files of FTS.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>
#include <fts/tiles/startup.h>

#ifdef __cplusplus
}
#endif

#endif









