/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/09/19 14:53:49 $
 *
 */

#include "fts.h"

extern void list_config(void);
extern void listarith_config(void);
extern void listelement_config(void);
extern void listhead_config(void);
extern void listjoin_config(void);
extern void listlength_config(void);
extern void listreverse_config(void);
extern void listsublist_config(void);
extern void listtail_config(void);

static void
lists_module_init(void)
{
  list_config();
  listarith_config();
  listelement_config();
  listhead_config();
  listjoin_config();
  listlength_config();
  listreverse_config();
  listsublist_config();
  listtail_config();
}

fts_module_t lists_module = {"lists", "basic list handling classes", lists_module_init};
