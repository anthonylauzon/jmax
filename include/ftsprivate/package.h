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

/***********************************************
 *
 *  Package context and package stack
 */

/**
 * Returns the system package. The system package defines the
 * system-wide classes such as: require, patcher, inlet, outlet, ...
 *
 * @fn fts_package_t* fts_get_system_package(void)
 * @return a pointer to the system package
 * @ingroup package */
fts_package_t* fts_get_system_package(void);

/**
 * Returns the package on the top of the stack.
 *
 * @fn fts_package_t* fts_get_current_package(void)
 * @return a pointer to the current package
 * @ingroup package */
fts_package_t* fts_get_current_package(void);

/**
 * Pushes the package on top of the stack, making it the current
 * package context.
 *
 * @fn void fts_package_push(fts_package_t* pkg)
 * @param pkg the package
 * @ingroup package */
void fts_package_push(fts_package_t* pkg);

/**
 * Removes the current package from the stack. 
 *
 * @fn void fts_package_pop(void);
 * @param pkg the package
 * @ingroup package */
void fts_package_pop(fts_package_t* pkg);

/**
 * Loads a jmax file as a package
 *
 * @fn fts_package_t* fts_package_load_from_file(fts_symbol_t name, char* filename)
 * @param name the name of the package
 * @param filename the path to the jmax file
 * @ingroup package */
fts_package_t* fts_package_load_from_file(fts_symbol_t name, const char* filename);


/** 
 * Loads package with the given name
 * 
 * @fn fts_package_t* fts_package_looad(fts_symbol_t name)
 * @param name the name of the package
 * 
 * @return pkg if package is found
 * @return NULL if package is not found
 * @ingroup package
 */
fts_package_t* fts_package_load(fts_symbol_t name);

/***********************************************
 *
 *  Package
 */

struct fts_package {
  fts_object_t object;

  fts_symbol_t name;
  fts_symbol_t dir;
  fts_symbol_t filename;
  fts_package_state_t state;
  char* error;
  fts_object_t* patcher;
  fts_list_t* packages;
  fts_hashtable_t* classes;

  fts_hashtable_t* declared_templates;
  fts_list_t* template_names;

  fts_hashtable_t* templates_in_path;
  fts_list_t* template_paths;

  fts_hashtable_t* declared_abstractions;
  fts_hashtable_t* abstractions_in_path;
  fts_list_t* abstraction_paths;

  fts_hashtable_t* help;
  fts_list_t* help_classes;

  fts_list_t* data_paths;

  fts_list_t* windows;

  fts_symbol_t config;

  int dirty; /* set to one if package's content is not saved */
};

/** Allocates a new package. 
 *
 * @param name the name of the package
 * @return a new package or NULL in case of error
 * @ingroup package */
fts_package_t* fts_package_new(fts_symbol_t name);

/** Deletes the ressources allocated by this package.
 *
 * @param pkg the package
 * @ingroup package */
void fts_package_delete(fts_package_t* pkg);

/**
 * Explicitely sets the state of the package. This should normally not
 * be used. It's provided only for the system package.
 *
 * @fn void fts_package_set_state(fts_package_t* pkg, fts_package_state_t s)
 * @param pkg the package
 * @param s the state of the package
 * @ingroup package */
void fts_package_set_state(fts_package_t* pkg, fts_package_state_t s);

/**
 * Set the error message of the package. This should normally not
 * be used from outside the package.
 *
 * @fn void fts_package_set_error(fts_package_t* pkg, char* message)
 * @param pkg the package
 * @param s the message string
 * @ingroup package */
void fts_package_set_error(fts_package_t* pkg, const char* s);

/**
 * Tell this package it depends on an other package. 
 *
 * @fn void fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg)
 * @param pkg the package
 * @param required_pkg the name of the required package
 * @ingroup package */
void fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg);

/**
 * Returns the class with the specified name. Returns NULL if this
 * package doesn't define the class.
 *
 * @fn int fts_package_get_class(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the class
 * @ingroup package */
fts_class_t* fts_package_get_class(fts_package_t* pkg, fts_symbol_t name);

/**
 * Adds a new path to the template path.
 *
 * @fn void fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path)
 * @param pkg the package
 * @param path the path
 * @ingroup package */
void fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path);

/**
 * Returns the explicitely declared template with the specified
 * name. Returns NULL if this package doesn't define the template.
 *
 * @fn fts_template_t *fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the template
 * @ingroup package */
fts_template_t *fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name);

/**
 * Returns the file template (found on the template path) with the
 * specified name. Returns NULL if this package doesn't define the
 * template.
 *
 * @fn fts_template_t *fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the template
 * @ingroup package */
fts_template_t *fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name);

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
fts_template_t *fts_package_get_template_from_file(fts_package_t* pkg, fts_symbol_t filename);

/**
 * Defines a new abstraction.
 *
 * @fn void fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
 * @param pkg the package
 * @param name the name of the abstraction
 * @param file the file name of the abstraction
 * @ingroup package */
void fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file);

/**
 * Adds a new path to the abstraction path.
 *
 * @fn void fts_package_add_abstraction_path(fts_package_t* pkg, fts_symbol_t path)
 * @param pkg the package
 * @param path the path
 * @ingroup package */
void fts_package_add_abstraction_path(fts_package_t* pkg, fts_symbol_t path);

/**
 * Returns the explicitely declared abstraction with the specified
 * name. Returns NULL if this package doesn't define the abstraction.
 *
 * @fn fts_abstraction_t *fts_package_get_declared_abstraction(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the abstraction
 * @ingroup package */
fts_abstraction_t *fts_package_get_declared_abstraction(fts_package_t* pkg, fts_symbol_t name);

/**
 * Returns the file abstraction (found on the abstraction path) with the
 * specified name. Returns NULL if this package doesn't define the
 * abstraction.
 *
 * @fn fts_abstraction_t *fts_package_get_abstraction_in_path(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the abstraction
 * @ingroup package */
fts_abstraction_t *fts_package_get_abstraction_in_path(fts_package_t* pkg, fts_symbol_t name);

/**
 * Add a new meta class to this package.
 *
 * @fn fts_status_t fts_package_add_class(fts_package_t* pkg, fts_class_t *cl, fts_symbol_t name)
 * @param pkg the package
 * @param cl the meta class
 * @param name the registery name
 * @return a status object 
 * @ingroup package */
fts_status_t fts_package_add_class( fts_package_t* pkg, fts_class_t *cl, fts_symbol_t name);

/**
 * Returns the meta class with the specified name. 
 *
 * @fn fts_class_t *fts_package_get_class(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the meta class
 * @return the class or NULL if this package doesn't define the meta class.
 * @ingroup package */
fts_class_t *fts_package_get_class(fts_package_t* pkg, fts_symbol_t name);

/**
 * Defines a new help.
 *
 * @fn void fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
 * @param pkg the package
 * @param name the name of the abstraction
 * @param file the file name 
 * @ingroup package */
void fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file);

/**
 * Returns the path name of the help patch for the class with the
 * specified name. Returns NULL if this package doesn't define the
 * class or if no help file exists.
 *
 * @fn int fts_package_get_help(fts_package_t* pkg, fts_symbol_t name)
 * @param pkg the package
 * @param name the name of the class
 * @ingroup package */
fts_symbol_t fts_package_get_help(fts_package_t* pkg, fts_symbol_t name);

/**
 * Defines a new data path.
 *
 * @fn void fts_package_add_data_path(fts_package_t* pkg, fts_symbol_t path)
 * @param pkg the package
 * @param path the path 
 * @ingroup package */
void fts_package_add_data_path(fts_package_t* pkg, fts_symbol_t path);

/**
 * Returns the full path of specified file name. 
 *
 * @fn int fts_package_get_file(fts_package_t* pkg, fts_symbol_t filename)
 * @param pkg the package
 * @param filename the name of the file
 * @param buf the buffer to store the file name
 * @param len the size of the buffer
 * @return 1 if the file was found, 0 if it was not found
 * @ingroup package */
int fts_package_get_data_file(fts_package_t* pkg, fts_symbol_t filename, char *buf, int len);


extern void fts_package_set_dirty(fts_package_t *this, int is_dirty);

/********************************************************************
 *
 *   Config objects
 */

fts_status_t fts_require_instantiate(fts_class_t *cl);
void fts_require_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

fts_status_t fts_template_instantiate(fts_class_t *cl);
void fts_template_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

fts_status_t fts_template_path_instantiate(fts_class_t *cl);
void fts_template_path_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

fts_status_t fts_data_path_instantiate(fts_class_t *cl);
void fts_data_path_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);


#endif /* _FTS_PRIVATE_PACKAGE_H_ */

