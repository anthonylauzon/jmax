/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:05 $
 *
 */

#include "fts.h"

extern void list_config(void);
extern void listclasses_config(void);
extern void listelement_config(void);
extern void listhead_config(void);
extern void listjoin_config(void);
extern void listlength_config(void);
extern void listreverse_config(void);
extern void listsublist_config(void);
extern void listtail_config(void);

static void
fts_lists_init(void)
{
  list_config();
  listclasses_config();
  listelement_config();
  listhead_config();
  listjoin_config();
  listlength_config();
  listreverse_config();
  listsublist_config();
  listtail_config();
}

fts_module_t lists_module = {"lists", "Basic Lists Objects", fts_lists_init};
