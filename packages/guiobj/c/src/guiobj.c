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
#include "fts.h"

extern void toggle_config(void);
extern void message_config(void);
extern void panel_config(void);
extern void gint_config(void);
extern void slider_config(void);
extern void gfloat_config(void);
extern void comment_config(void);
extern void button_config(void);
extern void message_doctor_init(void);
extern void comment_doctor_init(void);

static void
fts_guiobj_init(void)
{
  toggle_config();
  message_config();
  message_doctor_init();
  panel_config();
  gint_config();
  slider_config();
  gfloat_config();
  comment_config();
  comment_doctor_init();
  button_config();
}

fts_module_t guiobj_module = {"guiobj", "standard GUI objects", fts_guiobj_init};
