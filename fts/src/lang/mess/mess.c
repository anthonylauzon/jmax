/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/objtable.h"
#include "lang/mess/clipboard.h"

extern void fts_autosave_init(void);

static void
fts_mess_init(void)
{
  /* Init the symbol table */
  fts_symbols_init();

  /* Init the params module */
  fts_params_init();

  /* Init the FTS data module and id table */
  fts_data_id_init();
  fts_data_module_init();

  /* Initialize the classes module */
  fts_classes_init();

  /* Initialize the objects module  */
  fts_objects_init();

  /* Initialize the object doctor housekeeping */
  fts_doctor_init();

  /* Initialize the connections module  */
  fts_connections_init();

  /* Initialize the messages module  */
  fts_messages_init();

  /* Initialize the property system */
  fts_properties_init();

  /* Initialize object naming services */
  fts_mess_naming_init();

  /* Initialize the message event/error system */
  fts_events_init();

  /* Create the patcher data */
  fts_patcher_data_config();

  /* Create the patcher class */
  fts_patcher_init();

  /* Create the send/receive class */
  send_config();

  /* Initialize the .pat parser */
  fts_patparser_init();

  /* Init the loader vm  */
  fts_vm_init();

  /* Create the selection class */
  fts_selection_config();

  /* Create the selection class */
  fts_clipboard_config();

  /* Initialize the .pat abstraction subsystem */
  fts_abstraction_init();

  /* Initialize the expression subsystem */
  fts_expressions_init();
  fts_variables_init();

  /* Initialize the template subsystem */
  fts_template_init();

  /* Now that everything is up and running, create the root patcher */
  fts_create_root_patcher();

  /* panic Autosave init */

  fts_autosave_init();
}


static void
fts_mess_restart(void)
{
  fts_object_table_delete_all();
}

static void
fts_mess_shutdown(void)
{
  fts_patcher_shutdown();
  fts_object_table_delete_all();
}

fts_module_t fts_mess_module = {"Mess", "The new Mess module", fts_mess_init, fts_mess_restart, fts_mess_shutdown};
