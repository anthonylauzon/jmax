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

#ifndef _FTS_PROPERTIES_H_
#define _FTS_PROPERTIES_H_

/* the non underscored functions will run the daemon (look to the .c file for comments) */
/* also included are shortcuts to avoid type conversions to atoms */

extern void  fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value);
extern void _fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value);

extern void  fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property);
extern void _fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property);

extern void _fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);
extern void  fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);

/* Class version

   the class set will *not* run the daemons, of course.

   Also the is remove is provided for completness, but should not
   be used after class initialization.
*/

extern void fts_class_put_prop(fts_class_t *cl, fts_symbol_t property, const fts_atom_t *value);
extern void fts_class_remove_prop(fts_class_t *cl, fts_symbol_t property);
extern const fts_atom_t *fts_class_get_prop(fts_class_t *cl, fts_symbol_t property);


/* 
   Daemon handling
 */

/* class daemons */

extern void fts_class_add_daemon(fts_class_t *cl,
				 fts_daemon_action_t  action,
				 fts_symbol_t        property,
				 fts_property_daemon_t daemon);

extern void fts_class_remove_daemon(fts_class_t *cl, 
				    fts_daemon_action_t  action, 
				    fts_symbol_t        property,
				    fts_property_daemon_t daemon);

extern void fts_obj_prop_init(void);


#endif
