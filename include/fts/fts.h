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
 * \li object system
 * \li DSP system
 * \li scheduler
 * \li timing system
 * \li file handling
 * \li client/server communication
 * \li system utilities: memory management
 * \li platform dependend components: dynamic loading, memory management, scheduling, ...
 * \li misc utilities: hash tables, sampling rate conversion routines, four-point interpolation, ...
 *
 * This documentation is generated automatically from the comments in the header files of FTS.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* *** The inclusion order is important *** */
#include <fts/types.h>
#include <fts/status.h>

#include <fts/mem.h>
#include <fts/word.h>
#include <fts/atom.h>
#include <fts/symbol.h>

#include <fts/array.h>
#include <fts/buffer.h>
#include <fts/iterator.h>
#include <fts/hashtable.h>
#include <fts/objectlist.h>
#include <fts/objectset.h>

#include <fts/connection.h>
#include <fts/property.h>
#include <fts/expression.h>
#include <fts/class.h>
#include <fts/variable.h>
#include <fts/object.h>
#include <fts/patcher.h>
#include <fts/message.h>
#include <fts/doctor.h>
#include <fts/errobj.h>
#include <fts/inout.h>
#include <fts/saver.h>
#include <fts/param.h>

#include <fts/ftl.h>
#include <fts/ftlmem.h>
#include <fts/dsp.h>

#include <fts/fpe.h>

#include <fts/file.h>
#include <fts/atomfile.h>
#include <fts/soundfile.h>
#include <fts/midifile.h>
#include <fts/post.h>
#include <fts/midi.h>
#include <fts/time.h>
#include <fts/sched.h>
#include <fts/audio.h>
#include <fts/bytestream.h>


/*
 * FTS global initialization 
 */
FTS_API void fts_init( void);

#ifdef __cplusplus
}
#endif

#endif









