/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.5 $ IRCAM $Date: 1998/04/16 18:04:47 $
 *
 *  Eric Viara for Ircam, January 1995
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/objtable.h"

static void  fts_mess_init(void);
static void  fts_mess_restart(void);

fts_module_t fts_mess_module = {"Mess", "The new Mess module", fts_mess_init, fts_mess_restart, 0};



/* Forward declarations  */

static void
fts_mess_init(void)
{
  /* Init the symbol table */

  fts_symbols_init();

  /* Initialize the classes module */

  fts_classes_init();

  /* Initialize the objects module  */

  fts_objects_init();

  /* Initialize the property system */

  fts_properties_init();

  /* Initialize object naming services */

  fts_mess_naming_init();

  /* Initialize the message event/error system */

  fts_events_init();

  /* Create the patcher class */

  fts_patcher_init();

  /* Create the send/receive class */

  send_config();

  /* Initialize the .pat parser */

  fts_patparser_init();

  /* Initialize the .pat abstraction subsystem */
  
  fts_abstraction_init();
}


static void
fts_mess_restart(void)
{
  fts_object_table_delete_all();
}

