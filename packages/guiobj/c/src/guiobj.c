/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.4 $ IRCAM $Date: 1998/08/19 15:18:07 $
 *
 */

#include "fts.h"

extern void vtoggle_config(void);
extern void vmessage_config(void);
extern void panel_config(void);
extern void gobj_config(void);
extern void gint_config(void);
extern void gfloat_config(void);
extern void comment_config(void);
extern void button_config(void);
extern void message_doctor_init(void);
extern void comment_doctor_init(void);

static void
fts_ui_init(void)
{
  vtoggle_config();
  vmessage_config();
  message_doctor_init();
  panel_config();
  gobj_config();
  gint_config();
  gfloat_config();
  comment_config();
  comment_doctor_init();
  button_config();
}

fts_module_t ui_module = {"guiobj", "standard GUI objects", fts_ui_init};
