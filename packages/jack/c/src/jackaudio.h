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

#ifndef JACKAUDIO_H_
#define JACKAUDIO_H_ 1

typedef struct
{
  fts_audioport_t head;
  fts_symbol_t port_name;
  /* TODO: 
     Change structure if we want several input and output .
     But maybe it should be several jackaudioport, in this case we have to choose
     unique jack_client name */
  jack_port_t* input_port;
  jack_port_t* output_port;
  /* TODO:
     If we add several input/output  port, we should want several input/output buffer
  */
  float* input_buffer;
  float* output_buffer;
  /* TODO:
     Frame size could be changed by jack, we need to add a callback for buffer size 
     change */
  int nframes;
  /* TODO:
     sampling rate could be changed by jack, we need to add a callback for sampling 
     rate change */
  unsigned long samplingRate;

} jackaudioport_t;


fts_class_t* jackaudioport_type;

jack_client_t* jackaudiomanager_get_jack_client(void);
fts_object_t* jackaudiomanager_get_manager_object(void);

#endif /* JACKAUDIO_H_ */

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
