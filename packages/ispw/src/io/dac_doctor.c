/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
    
