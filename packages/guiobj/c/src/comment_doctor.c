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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <string.h>

/*
  Comment doctor.

  Fix comment boxes by avoiding expression evaluation in their
  arguments; later comments should work like message boxes.
 */

#include <fts/fts.h>

static fts_symbol_t get_comment_symbol(int argc, const fts_atom_t *argv);

static fts_object_t *comment_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac >= 1)
    {
      fts_atom_t v;
      fts_atom_t a;
      fts_object_t *obj;

      fts_set_symbol(&a, fts_new_symbol("jcomment"));
      fts_object_new_to_patcher(patcher, 1, &a, &obj);
      fts_object_set_description(obj, 1, &a);

      fts_set_symbol(&v, get_comment_symbol(ac - 1, at + 1));
      fts_object_put_prop(obj, fts_s_comment, &v);

      return obj;
    }
  else
    return 0;
}


void comment_doctor_init(void)
{
  fts_register_object_doctor(fts_new_symbol("comment"), comment_doctor);
}
    

/*
 * 
 * argstostr -- transform the argc,argv argument pair of  that is
 *		passed to the creation function of an object to a symbol
 * Args:
 *  number of argument 
 *  argument array 
 * 
 */

#define MAX_LENGTH 1024
#define SAFE       16

static fts_symbol_t get_comment_symbol(int argc, const fts_atom_t *argv)
{
  char buf[MAX_LENGTH];
  int i;

  buf[0] = '\0';

  for (i = 0 ; i < argc; i++)
    {
      const char *pad;

      pad = (i == (argc - 1)) ? "" : " ";

      if (fts_is_int(&argv[i]))
	{
	  if (strlen(buf) > MAX_LENGTH - SAFE)
	    break;
	  
	  sprintf(buf + strlen(buf), "%d%s", fts_get_int(&argv[i]), pad);
	}
      else if (fts_is_float(&argv[i]))
	{
	  if (strlen(buf) > MAX_LENGTH - SAFE)
	    break;

	  sprintf(buf + strlen(buf), "%f%s", fts_get_float(&argv[i]), pad);
	}
      else if (fts_is_symbol(&argv[i]))
	{
	  if (strlen(buf) + strlen(fts_get_symbol(&argv[i])) > MAX_LENGTH - 1)
	    break;

	  sprintf(buf + strlen(buf), "%s%s", fts_get_symbol(&argv[i]), pad);
	}
    }

  return fts_new_symbol_copy(buf);
}


