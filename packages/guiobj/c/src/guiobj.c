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

#include "guiobj.h"

extern void toggle_config(void);
extern void messconst_config(void);
extern void panel_config(void);
extern void gint_config(void);
extern void slider_config(void);
extern void gfloat_config(void);
extern void comment_config(void);
extern void comment_doctor_init(void);
extern void button_config(void);
extern void fork_config(void);
extern void keyserver_config(void);
extern void key_config(void);
extern void display_config(void);
extern void vecdisplay_config(void);
extern void scope_config(void);

void
guiobj_config(void)
{
  toggle_config();
  messconst_config();
  panel_config();
  gint_config();
  slider_config();
  gfloat_config();
  comment_config();
  comment_doctor_init();
  button_config();
  fork_config();
  keyserver_config();
  key_config();
  display_config();
  vecdisplay_config();
  scope_config();
}
