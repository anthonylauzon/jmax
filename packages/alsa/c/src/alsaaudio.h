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
#include <alsa/asoundlib.h>
#include <fts/fts.h>



/* sample format */
fts_symbol_t s_s16_le;
fts_symbol_t s_s32_le;

/* access type */
fts_symbol_t s_mmap_noninterleaved;
fts_symbol_t s_mmap_interleaved;
fts_symbol_t s_rw_noninterleaved;
fts_symbol_t s_rw_interleaved;

fts_class_t* alsaaudioport_type;

void alsaaudiomanager_scan_devices(void);
void alsaaudiomanager_scan_plugins(void);

snd_pcm_access_t alsaaudiomanager_convert_jmax_symbol_to_alsa_access(fts_symbol_t s_access);
int alsaaudiomanager_get_channels_max(const char* device_name, int stream_mode);
