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


#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/objtable.h"
#include "lang/mess/clipboard.h"

extern void fts_autosave_init(void);

static void fts_mess_init( void )
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

  /* Initialize the objects module  */
  fts_atoms_init();

  /* Initialize the object doctor housekeeping */
  fts_doctor_init();

  /* Initialize the connections module  */
  fts_connections_init();

  /* Initialize the messages module  */
  fts_messages_init();

  /* Initialize the property system */
  fts_properties_init();

  /* Create the patcher data */
  fts_patcher_data_config();

  /* Create the patcher class */
  fts_patcher_init();

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

  /* Create the inlet/outlet (send/receive) class */
  fts_inout_config();

  /* Now that everything is up and running, create the root patcher */
  fts_create_root_patcher();

  /* panic Autosave init */

  fts_autosave_init();
}


static void
fts_mess_shutdown(void)
{
  fts_patcher_shutdown();
  fts_object_table_delete_all();
}

fts_module_t fts_mess_module = {"Mess", "The new Mess module", fts_mess_init, fts_mess_shutdown, 0};
