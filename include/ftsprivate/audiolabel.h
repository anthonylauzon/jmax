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

#ifndef _FTS_PRIVATE_AUDIOLABEL_H_
#define _FTS_PRIVATE_AUDIOLABEL_H_ 1

typedef struct _audiolabel
{
    fts_object_t o;
    fts_symbol_t name;
    fts_symbol_t input_device;
    fts_audioport_t* input_audioport;
    int input_channel;
    
    fts_symbol_t output_device;
    fts_audioport_t* output_audioport;
    int output_channel;

    struct _audiolabel* next;
} audiolabel_t;

extern fts_class_t* audiolabel_type;

/**
 * audiolabel
 */
void audiolabel_set_input_channel(audiolabel_t* label, int channel);
void audiolabel_set_input_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name);
void audiolabel_set_output_channel(audiolabel_t* label, int channel);
void audiolabel_set_output_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name);
void audiolabel_client_send_message(fts_object_t* o, fts_symbol_t selector, audiolabel_t* label, int index);

#endif /* _FTS_PRIVATE_AUDIOLABEL_H_ */





