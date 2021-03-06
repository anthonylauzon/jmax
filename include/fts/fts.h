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

#ifndef FTS_FTS_H_
#define FTS_FTS_H_

/**
 * Global FTS include file. 
 * Object programmers just need to include this file as:
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
 * This will be the developer's documentation of the FTS API.
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

#ifdef WIN32
#if defined(FTSDLL_EXPORTS)
#define FTS_API __declspec(dllexport)
#else
#define FTS_API __declspec(dllimport)
#endif
#else
#define FTS_API extern
#endif

#define FTS_MODULE_INIT(m) void fts_module_##m##_init(void); void fts_module_##m##_init(void)
#define FTS_MODULE_INIT_CALL(m) { extern void fts_module_##m##_init(void); fts_module_##m##_init(); }
#define FTS_PACKAGE_INIT(p) void p##_config(void); void p##_config(void)

/* *** The inclusion order is important *** */
#include <fts/types.h>
#include <fts/status.h>
#include <fts/malloc.h>
#include <fts/symbol.h>
#include <fts/stack.h>
#include <fts/fifo.h>
#include <fts/property.h>
#include <fts/iterator.h>
#include <fts/hashtable.h>
#include <fts/list.h>
#include <fts/objectlist.h>
#include <fts/variable.h>
#include <fts/atom.h>
#include <fts/array.h>
#include <fts/patcherobject.h>
#include <fts/object.h>
#include <fts/class.h>
#include <fts/function.h>
#include <fts/tokenizer.h>
#include <fts/expression.h>
#include <fts/objectset.h>
#include <fts/tuple.h>
#include <fts/message.h>
#include <fts/connection.h>
#include <fts/outlet.h>
#include <fts/patcher.h>
#include <fts/dumper.h>
#include <fts/errobj.h>
#include <fts/label.h>
#include <fts/param.h>
#include <fts/package.h>
#include <fts/preset.h>
#include <fts/timebase.h>
#include <fts/ftl.h>
#include <fts/ftlmem.h>
#include <fts/dsp.h>
#include <fts/fpe.h>
#include <fts/file.h>
#include <fts/atomfile.h>
#include <fts/midievent.h>
#include <fts/midi.h>
#include <fts/midifile.h>
#include <fts/bytestream.h>
#include <fts/socketstream.h>
#include <fts/post.h>
#include <fts/sched.h>
#include <fts/audio.h>
#include <fts/audiofile.h>
#include <fts/project.h>
#include <fts/client.h>
#include <fts/update.h>
#include <fts/binaryprotocol.h>

/*
 * FTS version 
 */
FTS_API const char *fts_get_version( void);

/*
 * FTS global initialization 
 */
FTS_API void fts_init( int argc, char **argv);

/*
 * FTS global shutdown 
 */
FTS_API void fts_shutdown( void);

/*
 * Command line arguments access
 */
FTS_API fts_symbol_t fts_cmd_args_get( fts_symbol_t name);

/*
 * Root directory
 */
FTS_API fts_symbol_t fts_get_root_directory( void);

/**
 * Return the user's home directory as a symbol.
 *
 * @return The current user's home directory.
 */
FTS_API fts_symbol_t fts_get_user_directory(void);

#ifdef __cplusplus
}
#endif

#endif /* FTS_FTS_H_ */







