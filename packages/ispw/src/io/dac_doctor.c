/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/*
  Dac doctor, for in~ and out~ objects.

  If the object have exactly two arguments, syntetize 
  a new set of arguments, correspoding to all the existing channels
  for the device choosen.
  

 */

#include "fts.h"

static fts_object_t *sigin_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac == 2)
    {
      fts_object_t *obj;
      fts_audio_input_logical_device_t *ldev;
      int i, nchans;
      fts_atom_t *args;

      ldev = fts_audio_get_input_logical_device(fts_get_symbol(at+1));
      nchans = fts_audio_get_input_channels(ldev);

      args = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (nchans + 2));

      args[0] = at[0];
      args[1] = at[1];

      for (i = 0; i < nchans; i++)
	fts_set_int(&(args[i + 2]), i + 1);

      fts_object_new(patcher, 2 + nchans, args, &obj);
      fts_object_set_description(obj, 2 + nchans, args);

      fts_free(args);
      return obj;
    }
  else
    return 0;
}

static fts_object_t *sigout_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac == 2)
    {
      fts_object_t *obj;
      fts_audio_output_logical_device_t *ldev;
      int i, nchans;
      fts_atom_t *args;

      ldev = fts_audio_get_output_logical_device(fts_get_symbol(at+1));
      nchans = fts_audio_get_output_channels(ldev);

      args = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (nchans + 2));

      args[0] = at[0];
      args[1] = at[1];

      for (i = 0; i < nchans; i++)
	fts_set_int(&(args[i + 2]), i + 1);

      fts_object_new(patcher, 2 + nchans, args, &obj);
      fts_object_set_description(obj, 2 + nchans, args);

      fts_free(args);
      return obj;
    }
  else
    return 0;
}


void dac_doctor_init(void)
{
  fts_register_object_doctor(fts_new_symbol("in~"), sigin_doctor);
  fts_register_object_doctor(fts_new_symbol("out~"), sigout_doctor);
}
    
