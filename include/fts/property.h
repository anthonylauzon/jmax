/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */


/* 
   Object dynamic properties.

   A property list is associated to every object, that can
   store there properties needed by rest of the sytem.
   
   A list is associated also the class, and by default,
   if a property is not found in the object, is looked for in the
   class itself; the property list should be initialized in
   the class instantiation function.

   Properties are not intended as substitute of object memory;
   they should *not* be used during standard control computation (their
   use is memory and computing intensive).

   The DSP compiler will make large use of properties.

   DAEMON: a daemon is a function that is called when an object property is
   set or unset; the daemon is called with the object, the property name
   and value as argument; daemon are defined at the *class* level.


   By convention, properties have upper case names (like DSP_IS_THRU),
   and use "_" as word separator (like #define macros); first word
   should refer to the kind of module/functionalities we are referring
   to, and the others should specify better.
 */


typedef struct fts_plist fts_plist_t;


/* Class version

   the class set will *not* run the daemons, of course.

   Also the is remove is provided for completness, but should not
   be used after class initialization.
*/

FTS_API void fts_class_put_prop(fts_class_t *cl, fts_symbol_t property, const fts_atom_t *value);
FTS_API void fts_class_remove_prop(fts_class_t *cl, fts_symbol_t property);
FTS_API const fts_atom_t *fts_class_get_prop(fts_class_t *cl, fts_symbol_t property);

/* 
   Daemon handling
 */

/* class daemons */

typedef enum fts_daemon_action {
  obj_property_put, 
  obj_property_get, 
  obj_property_remove
} fts_daemon_action_t;

typedef void (* fts_property_daemon_t)(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);

