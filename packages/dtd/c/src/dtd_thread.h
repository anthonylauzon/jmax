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

#ifndef DTD_THREAD_H_
#define DTD_THREAD_H_ 1

#include <fts/fts.h>
#include "dtd_buffer.h"

/**************************************************
 *
 * dtd thread
 *
 */
typedef struct
{
    fts_object_t head;
    /* sound file */
    fts_audiofile_t* sf;

    /* inter thread communication buffer */
    dtd_buffer_t* com_buffer;

    /* index of buffer write by FTS */
    const int* const buffer_index;
    /* eof flags set by dtd_thread */
    const int* const is_eof;

    /* delay for nanosleep (see fts_thread_manager) */
    int delay_ms;
} dtd_thread_t;


FTS_API void dtd_thread_set_state(dtd_thread_t* self,
				  fts_audiofile_t* sf, 
				  dtd_buffer_t* com_buffer, 
				  const int* const buffer_index,
				  const int* const is_eof);

extern fts_class_t* dtd_thread_type;

#endif /* DTD_THREAD_H_ */
