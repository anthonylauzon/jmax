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
 */

#ifndef _FTS_PACKAGE_H_
#define _FTS_PACKAGE_H_

/**
 * Package
 *
 * The API for FTS packages.
 *
 *
 * @defgroup package package
 */

typedef struct _fts_package_t fts_package_t;

/***********************************************
 *
 *  Package registry
 */

/**
 * Registers a new package to the FTS kernel. If the package already
 * exists, nothing is done. Otherwise, a new package object is created
 * and inserted in the list of known packages. 
 *
 * @fn void fts_register_package(fts_symbol_t name)
 * @param name the name of the package
 * @ingroup package */
FTS_API void fts_register_package(fts_symbol_t name);

/**
 * Initializes an iterator for all the registered packages. The
 * iterator returns the names of the packages.
 *
 * @fn void fts_get_package_names(fts_iterator_t* i)
 * @ingroup package */
FTS_API void fts_get_package_names(fts_iterator_t* i);

/**
 * Initializes an iterator for all the registered packages. The
 * iterator returns the names of the packages.
 *
 * @fn void fts_get_packages(fts_iterator_t* i)
 * @ingroup package */
FTS_API void fts_get_packages(fts_iterator_t* i);

/**
 * Returns the package with the specified name or NULL if the package
 * is not registered.
 *
 * @fn fts_package_t* fts_get_package(fts_symbol_t name)
 * @param name the name of the package
 * @return a pointer to the package or NULL if the package could not
 * be found
 * @ingroup package */
FTS_API fts_package_t* fts_get_package(fts_symbol_t name);


/***********************************************
 *
 *  Package context and package stack
 */

/**
 * Returns the package on the top of the stack.
 *
 * @fn fts_package_t* fts_get_current_package(void)
 * @return a pointer to the current package
 * @ingroup package */
FTS_API fts_package_t* fts_get_current_package(void);

/**
 * Pushes the package on top of the stack, making it the current
 * package context.
 *
 * @fn void fts_push_package(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
FTS_API void fts_push_package(fts_package_t* pkg);

/**
 * Removes the current package from the stack.
 *
 * @fn void fts_push_package(fts_package_t* pkg)
 * @ingroup package */
FTS_API void fts_pop_package(void);


/***********************************************
 *
 *  Public API of the package object 
 */

typedef enum {
  fts_package_defined,
  fts_package_loaded,
  fts_package_corrupt
} fts_package_state_t;

/**
 * Returns the name of the package.
 *
 * @fn fts_symbol_t fts_package_get_name(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
FTS_API fts_symbol_t fts_package_get_name(fts_package_t* pkg);

/**
 * Returns the directory of the package.
 *
 * @fn fts_symbol_t fts_package_get_dir(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
FTS_API fts_symbol_t fts_package_get_dir(fts_package_t* pkg);

/**
 * Returns the state of the package. The state indicated whether this
 * package has been defined or loaded or whether an error occured
 * during the package load.
 *
 * @fn fts_package_state_t fts_package_get_state(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
FTS_API fts_package_state_t fts_package_get_state(fts_package_t* pkg);

/**
 * Returns the error message of the package. If no error occured, this
 * function returns NULL.
 *
 * @fn char* fts_package_get_error(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
FTS_API char* fts_package_get_error(fts_package_t* pkg);

/**
 * Tell this package it depends on an other package. 
 *
 * @fn void fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg)
 * @param pkg the package
 * @param required_pkg the name of the required package
 * @ingroup package */
FTS_API void fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg);

/**
 * Returns the class with the specified name. Returns NULL if this
 * package doesn't define the class.
 *
 * @fn int fts_package_get_class(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the class
 * @ingroup package */
FTS_API fts_class_t* fts_package_get_class(fts_package_t* pkg, fts_symbol_t name);

/**
 * Defines a new template.
 *
 * @fn void fts_package_add_template(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
 * @param pkg the package
 * @param name the name of the template
 * @param file the file name of the template
 * @ingroup package */
FTS_API void fts_package_add_template(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file);

/**
 * Adds a new path to the template path.
 *
 * @fn void fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path)
 * @param pkg the package
 * @param path the path
 * @ingroup package */
FTS_API void fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path);

/**
 * Returns the explicitely declared template with the specified
 * name. Returns NULL if this package doesn't define the template.
 *
 * @fn fts_template_t *fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the template
 * @ingroup package */
FTS_API fts_template_t *fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name);

/**
 * Returns the file template (found on the template path) with the
 * specified name. Returns NULL if this package doesn't define the
 * template.
 *
 * @fn fts_template_t *fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the template
 * @ingroup package */
FTS_API fts_template_t *fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name);

/**
 * Returns the template that corresponds to the given file name. This
 * functions searches only in among the templates whose full file name
 * are known to package: in the declared templates and the loaded
 * templates on the template path.  Returns NULL if this package
 * doesn't define the template or if the template has not been loaded
 * yet.
 *
 * @fn fts_template_t *fts_package_get_template_from_file(fts_package_t* pkg, fts_symbol_t filename)
 * @param pkg the package
 * @param filename the name of the file
 * @ingroup package */
FTS_API fts_template_t *fts_package_get_template_from_file(fts_package_t* pkg, fts_symbol_t filename);

/**
 * Defines a new abstraction.
 *
 * @fn void fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
 * @param pkg the package
 * @param name the name of the abstraction
 * @param file the file name of the abstraction
 * @ingroup package */
FTS_API void fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file);

/**
 * Returns the full path name of the abstraction with the specified
 * name. Returns NULL if this package doesn't define the abstraction.
 *
 * @fn int fts_package_get_abstraction(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the abstraction
 * @ingroup package */
FTS_API char* fts_package_get_abstraction(fts_package_t* pkg, fts_symbol_t name);

/**
 * Defines a new help.
 *
 * @fn void fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
 * @param pkg the package
 * @param name the name of the abstraction
 * @param file the file name 
 * @ingroup package */
FTS_API void fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file);

/**
 * Returns the path name of the help patch for the class with the
 * specified name. Returns NULL if this package doesn't define the
 * class or if no help file exists.
 *
 * @fn int fts_package_get_help(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the class
 * @ingroup package */
FTS_API char* fts_package_get_help(fts_package_t* pkg, fts_symbol_t name);

/**
 * Defines a new data path.
 *
 * @fn void fts_package_add_path(fts_package_t* pkg, fts_symbol_t path)
 * @param pkg the package
 * @param path the path 
 * @ingroup package */
FTS_API void fts_package_add_path(fts_package_t* pkg, fts_symbol_t path);

/**
 * Returns the full path of specified file name. Returns NULL if this
 * package doesn't contain the specified file.
 *
 * @fn int fts_package_get_file(fts_package_t* pkg, fts_symbol_t filename)
 * @param pkg the package
 * @param filename the name of the file
 * @ingroup package */
FTS_API char* fts_package_get_file(fts_package_t* pkg, fts_symbol_t filename);


#endif /* _FTS_PACKAGE_H_ */
