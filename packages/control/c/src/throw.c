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
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include <fts/fts.h>

typedef struct _throw_
{
  fts_send_t send;
} throw_t;

typedef struct _catch_
{
  fts_receive_t receive;
} catch_t;

/***************************************************************************
 *
 *  throw
 *
 */

/* throw anything rethrow everything channel the channel to all the receives */
static void
throw_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;

  fts_channel_send((fts_object_t *)this->channel, s, ac, at);
}

static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;

  if(fts_is_a(at + 1, fts_s_label))
    channel = (fts_channel_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Argument is not a channel");
      return;
    }

  fts_object_refer((fts_object_t *)channel);
  fts_channel_add_send(channel, &this->send);
  this->channel = channel;

  dsp_list_insert(o);
}

static void
throw_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;

  fts_channel_remove_send(this->channel, &this->send);

  fts_object_release((fts_object_t *)this->channel);

  dsp_list_remove(o);
}

static void
throw_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  fts_channel_add_to_set(this->head.channel, set);
}

fts_status_t
throw_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(throw_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, throw_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, throw_delete);

  fts_method_define_varargs(cl, 0, fts_s_anything, throw_anything);

  return fts_Success;
}

/***************************************************************************
 *
 *  catch
 *
 */

static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  fts_channel_t *channel = 0;


  if(fts_is_a(at + 1, fts_s_label))
    channel = (fts_channel_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Argument is not a channel");
      return;
    }
  fts_object_refer((fts_object_t *)channel);
  fts_channel_add_receive(channel, &this->receive);
  this->channel = channel;
}
  
static void
catch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;

  fts_channel_remove_receive(this->channel, &this->receive);

  fts_object_release((fts_object_t *)this->channel);
}

static void
catch_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  fts_channel_add_to_set(this->channel, set);
}

fts_status_t
catch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(catch_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, catch_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, catch_delete);

  return fts_Success;
}

void
throw_config(void)
{
  fts_class_install(fts_new_symbol("throw"), throw_instantiate);
  fts_class_install(fts_new_symbol("catch"), catch_instantiate);
}
