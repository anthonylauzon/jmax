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
#include <ftsprivate/bmaxfile.h> 

#ifndef _FTS_PRIVATE_AUDIO_H_
#define _FTS_PRIVATE_AUDIO_H_

typedef struct _audiolabel
{
    fts_object_t o;
    fts_symbol_t name;
    int stereo_flag; /* 0: mono; 1:stereo */
    fts_symbol_t input_device;
    fts_audioport_t* input_audioport;
    int input_channel;
    
    fts_symbol_t output_device;
    fts_audioport_t* output_audioport;
    int output_channel;

    struct _audiolabel* next;
} audiolabel_t;

typedef struct _audioconfig
{
    fts_object_t o;
    audiolabel_t* labels;
    int n_labels;    

    int buffer_size;
    int sample_rate;

} audioconfig_t;

extern fts_class_t* audioconfig_type;
extern fts_class_t* audiolabel_type;

extern void fts_audio_idle( void);

void fts_audioconfig_set_defaults(audioconfig_t* config);
extern void fts_audioconfig_dump( audioconfig_t *mc, fts_bmax_file_t *f);

/**
 * audiolabel
 */
void audiolabel_set_stereo(audiolabel_t* label, int stereo_flag);
void audiolabel_set_input_channel(audiolabel_t* label, int channel);
void audiolabel_set_input_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name);
void audiolabel_set_output_channel(audiolabel_t* label, int channel);
void audiolabel_set_output_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name);
void audiolabel_client_send_message(fts_object_t* o, fts_symbol_t selector, audiolabel_t* label, int index);








#endif


