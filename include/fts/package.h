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


/***********************************************
 *
 *  Package registry
 */

/**
 * Initializes an iterator for all the registered packages. The
 * iterator returns the names of the packages.
 *
 * @fn void fts_get_package_names(fts_iterator_t* iter)
 * @param iter a pointer to an iterator object
 * @ingroup package */
FTS_API void fts_get_package_names(fts_iterator_t* iter);

/**
 * Initializes an iterator for all the registered packages. The
 * iterator returns the names of the packages.
 *
 * @fn void fts_get_packages(fts_iterator_t* iter)
 * @param iter a pointer to an iterator object
 * @ingroup package */
FTS_API void fts_get_packages(fts_iterator_t* iter);

/**
 * Returns the package with the specified name or NULL if the package
 * couldn't be found. The package might be loaded is it isn't loaded,
 * yet.
 *
 * @fn fts_package_t* fts_package_get(fts_symbol_t name)
 * @param name the name of the package
 * @return a pointer to the package or NULL if the package could not
 * be found
 * @ingroup package */
FTS_API fts_package_t* fts_package_get(fts_symbol_t name);

/**
 * Package
 *
 * The API for FTS packages.
 *
 *
 * @defgroup package package
 */

/***********************************************
 *
 *  Public API of the package object 
 */

typedef enum {
  fts_package_defined,
  fts_package_loading,
  fts_package_loaded,
  fts_package_corrupt
} fts_package_state_t;

/**
 * Returns the name of the package.
 *
 * @fn fts_symbol_t fts_package_get_name(fts_package_t* pkg)
 * @param pkg the package
 * @return the name of the package as a symbol
 * @ingroup package */
FTS_API fts_symbol_t fts_package_get_name(fts_package_t* pkg);

/**
 * Returns the directory of the package.
 *
 * @fn fts_symbol_t fts_package_get_dir(fts_package_t* pkg)
 * @param pkg the package
 * @return the directory of the package as a symbol
 * @ingroup package */
FTS_API fts_symbol_t fts_package_get_dir(fts_package_t* pkg);

/**
 * Returns the state of the package. The state indicated whether this
 * package has been defined or loaded or whether an error occured
 * during the package load.
 *
 * @fn fts_package_state_t fts_package_get_state(fts_package_t* pkg)
 * @param pkg the package
 * @return the state of the package
 * @ingroup package */
FTS_API fts_package_state_t fts_package_get_state(fts_package_t* pkg);

/**
 * Initializes an iterator for all the required packages of this
 * package. The iterator returns the names of the packages.
 *
 * @fn void fts_package_get_required_packages(fts_package_t* pkg, fts_iterator_t* iter)
 * @param pkg the package
 * @param iter a pointer to an iterator object
 * @ingroup package */
FTS_API void fts_package_get_required_packages(fts_package_t* pkg, fts_iterator_t* iter);

/**
 * Initializes an iterator for all the class names of this
 * package. The iterator returns the names of the classes.
 *
 * @fn void fts_package_get_class_names(fts_package_t* pkg, fts_iterator_t* iter)
 * @param pkg the package
 * @param iter a pointer to an iterator object
 * @ingroup package */
FTS_API void fts_package_get_class_names(fts_package_t* pkg, fts_iterator_t* iter);

/**
 * Returns the class with the specified name. Returns NULL if this
 * package doesn't define the class.
 *
 * @fn fts_class_t* fts_package_get_class(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the class
 * @ingroup package */
FTS_API fts_class_t* fts_package_get_class(fts_package_t* pkg, fts_symbol_t name);


