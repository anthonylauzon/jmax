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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_PACKAGE_H_
#define _FTS_PRIVATE_PACKAGE_H_

#include <fts/fts.h>
#include <fts/package.h>

struct _fts_package_t {
  fts_symbol_t name;
  fts_symbol_t dir;
  fts_package_state_t state;
  char* error;
  fts_patcher_t* definition;
  fts_list_t* packages;
  fts_hashtable_t* classes;
  fts_hashtable_t* declared_templates;
  fts_hashtable_t* templates_in_path;
  fts_hashtable_t* abstractions;
  fts_hashtable_t* help;
  fts_list_t* template_paths;
  fts_list_t* paths;
};

/** Allocates a new package. 
 *
 * @param name the name of the package
 * @return a new package or NULL in case of error
 */
fts_package_t* new_fts_package(fts_symbol_t name);

/** Deletes the ressources allocated by this package.
 *
 * @param pkg the package
 */
void delete_fts_package(fts_package_t* pkg);

/**
 * A call to this function explicitely forces the package to load. As
 * a side effect, all packages on which this package depends will be
 * loaded too. This function returns 0 when the package was
 * successfully loaded and -1 if an error occured. In case of error,
 * call fts_package_get_error() to get a user readable message.  If
 * the package is already loaded this function performs no actions and
 * simple returns 0. If the package is in the error state, this
 * function simple returns -1.
 *
 * @fn int fts_package_load(fts_package_t* pkg)
 * @param pkg the package
 * @return 0 if the package was succesfully loaded, -1 otherwise
 * @ingroup package 
 */
int fts_package_load(fts_package_t* pkg);

/** Load the patcher that contains the definition of the package. 
 *
 * @fn int fts_package_load_definition(fts_package_t* pkg)
 * @param pkg the package
 * @return 0 if the package was succesfully loaded, -1 otherwise
 * @ingroup package 
 */
int fts_package_load_definition(fts_package_t* pkg);

/** Loads all the files that have a default. This includes the shared
 *  object.
 *
 * @fn int fts_package_load_definition(fts_package_t* pkg)
 * @param pkg the package
 * @return 0 if the package was succesfully loaded, -1 otherwise
 * @ingroup package */
int fts_package_load_defaults(fts_package_t* pkg);

/** Loads all the files that have a default. This includes the shared
 *  object.
 *
 * @fn int fts_package_load_definition(fts_package_t* pkg)
 * @param pkg the package
 * @return 0 if the package was succesfully loaded, -1 otherwise
 * @ingroup package */
int fts_package_load_defaults(fts_package_t* pkg);

/** Find a file in the package.
 *
 */
fts_symbol_t fts_package_find_file(fts_package_t* pkg, fts_list_t* paths, fts_symbol_t filename);

#endif /* _FTS_PRIVATE_PACKAGE_H_ */
