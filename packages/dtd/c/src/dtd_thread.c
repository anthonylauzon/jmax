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

#include <fts/fts.h>
#include "dtd_buffer.h"
#include "dtd_thread.h"


#define DEFAULT_DELAY_MS 50

static fts_symbol_t dtd_thread_symbol;

/* 
   !!!! No return because must be in a infinite thread !!!
*/
static void
dtd_thread_read(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dtd_thread_t* self = (dtd_thread_t*)o;
    dtd_buffer_t* com_buffer;
    /* index to buffer to read */
    int buffer_to_write;
    int size = 0;

    /* get buffer not used by fts */
    buffer_to_write = (*self->buffer_index + 1) % 2;
    com_buffer = &self->com_buffer[buffer_to_write];

    if (com_buffer->full == 0)
    {
	size = fts_audiofile_read(self->sf, 
				  com_buffer->buffer, 
				  com_buffer->n_channels, 
				  com_buffer->size);
	com_buffer->full = 1;
	com_buffer->end_index = size;
    }

}

/* 
   !!!! No return because must be in a infinite thread !!!
*/
static void
dtd_thread_write(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dtd_thread_t* self = (dtd_thread_t*)o;
    dtd_buffer_t* com_buffer;
    /* index to buffer to write */
    int buffer_to_write;
    int size = 0;

    /* get buffer not used by fts */
    buffer_to_write = (*self->buffer_index + 1) % 2;
    com_buffer = &self->com_buffer[buffer_to_write];

    if (com_buffer->full == 1)
    {
	size = fts_audiofile_write(self->sf, 
				   com_buffer->buffer, 
				   com_buffer->n_channels, 
				   com_buffer->end_index);
	com_buffer->full = 0;
    }

}


static void
dtd_thread_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dtd_thread_t* self = (dtd_thread_t*)o;

    /* sound file to write */
    self->sf = 0;
    self->delay_ms = DEFAULT_DELAY_MS;
}

static void
dtd_thread_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dtd_thread_t* self = (dtd_thread_t*)o;
    self->sf = 0;
}

static void
dtd_thread_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    fts_class_init(cl, sizeof(dtd_thread_t), dtd_thread_init, dtd_thread_delete);
    fts_class_message_varargs(cl, fts_s_write, dtd_thread_write);
    fts_class_message_varargs(cl, fts_s_read, dtd_thread_read);

    fts_class_message_varargs(cl, fts_s_write, dtd_thread_write);
    fts_class_message_varargs(cl, fts_s_read, dtd_thread_read);
    return fts_ok;
}

void dtd_thread_config(void)
{
    dtd_thread_type = fts_class_install(dtd_thread_symbol, dtd_thread_instantiate);
}
