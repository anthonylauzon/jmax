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

#include <fts/fts.h>
#include <ftsprivate/package.h>
#include <ftsprivate/template.h>
#include <ftsprivate/errobj.h>
#include <ftsconfig.h>
#include <stdlib.h> 

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#define PACKAGE_STACK_SIZE    32

fts_hashtable_t fts_packages;
fts_list_t* package_paths = NULL;

fts_package_t* package_stack[PACKAGE_STACK_SIZE];
int package_stack_top = 0;

/***********************************************************************
 *
 * Initialization
 *
 */
void 
fts_kernel_package_init(void)
{
  int i;

  for (i = 0; i < PACKAGE_STACK_SIZE; i++) {
    package_stack[i] = NULL;
  }

  fts_hashtable_init(&fts_packages, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

  {/* FIXME: testing [pH07] */

    fts_symbol_t base_symbol;
    fts_package_t* base_package;
    
    base_symbol = fts_new_symbol("_base");
    fts_register_package(base_symbol);
    base_package = fts_get_package(base_symbol);
    
    /* FIXME: hack [pH07] */
    base_package->state = fts_package_loaded;
    
    fts_push_package(base_package);
  }
}


/***********************************************
 *
 *  Package registry
 */

void 
fts_register_package(fts_symbol_t name)
{
  fts_package_t* pkg;
  fts_atom_t n, p;

  fts_set_symbol(&n, name);

  /* check if the package is already registered */
  if (fts_hashtable_get(&fts_packages, &n, &p)) {
    return;
  }

  /* create a new package and insert it into the hashtable */
  pkg = new_fts_package(name);
  if (pkg == NULL) {
    return;
  }
  fts_set_ptr(&p, pkg);
  fts_hashtable_put(&fts_packages, &n, &p);
}

void 
fts_get_package_names(fts_iterator_t* i)
{
  fts_hashtable_get_keys(&fts_packages, i);
}

void 
fts_get_packages(fts_iterator_t* i)
{
  fts_hashtable_get_values(&fts_packages, i);
}

fts_package_t* 
fts_get_package(fts_symbol_t name)
{
  fts_atom_t n, p;

  fts_set_symbol(&n, name);

  if (fts_hashtable_get(&fts_packages, &n, &p)) {
    return fts_get_ptr(&p);
  } else {
    return NULL;
  }
}


/***********************************************
 *
 *  Package context and package stack
 */

fts_package_t* 
fts_get_current_package(void)
{
  return (package_stack_top > 0)? package_stack[package_stack_top - 1] : NULL;
}

void 
fts_push_package(fts_package_t* pkg)
{
  if (package_stack_top < PACKAGE_STACK_SIZE) {
    package_stack[package_stack_top++] = pkg;
  } else {
    post("Package stack overflow\n");
  }
}

void 
fts_pop_package(void)
{
  if (package_stack_top > 0) {
    package_stack[--package_stack_top] = NULL;
  }
}

/***********************************************
 *
 *  Package object 
 */

fts_package_t* 
new_fts_package(fts_symbol_t name)
{
  fts_package_t* pkg;

  pkg = fts_malloc(sizeof(fts_package_t));
  if (pkg == NULL) {
    return NULL;
  }
  pkg->name = name;
  pkg->dir = fts_new_symbol("");
  pkg->state = fts_package_defined;
  pkg->error = NULL;
  pkg->definition = NULL;
  pkg->packages = NULL;
  pkg->classes = NULL;
  pkg->declared_templates = NULL;
  pkg->templates_in_path = NULL;
  pkg->abstractions = NULL;
  pkg->help = NULL;
  pkg->template_paths = NULL;
  pkg->paths = NULL;

  return pkg;
}

void 
delete_fts_package(fts_package_t* pkg)
{
  if (pkg->packages != NULL) {
    fts_list_delete(pkg->packages);
  }
  if (pkg->paths != NULL) {
    fts_list_delete(pkg->paths);
  }
  if (pkg->classes != NULL) {
    fts_hashtable_destroy(pkg->classes);
  }
  if (pkg->declared_templates != NULL) {
    fts_hashtable_destroy(pkg->declared_templates);
  }
  if (pkg->templates_in_path != NULL) {
    fts_hashtable_destroy(pkg->templates_in_path);
  }
  if (pkg->abstractions != NULL) {
    fts_hashtable_destroy(pkg->abstractions);
  }
  if (pkg->help != NULL) {
    fts_hashtable_destroy(pkg->help);
  }
  fts_free(pkg);
}

fts_symbol_t 
fts_package_get_name(fts_package_t* pkg)
{
  return pkg->name;
}

fts_symbol_t 
fts_package_get_dir(fts_package_t* pkg)
{
  return pkg->dir;
}

fts_package_state_t 
fts_package_get_state(fts_package_t* pkg)
{
  return pkg->state;
}

char* 
fts_package_get_error(fts_package_t* pkg)
{
  return pkg->error;
}

void 
fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg)
{
  fts_atom_t n;

  fts_set_symbol(&n, required_pkg);
  pkg->packages = fts_list_append(pkg->packages, &n);
}

/********************************************************************
 *
 *   - templates
 */

void 
fts_package_add_template(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
{
  fts_template_t *template;
  fts_atom_t n, p;
  char buf[MAXPATHLEN];

  /* If the declaration existed already, remove it first */
  template = fts_package_get_declared_template(pkg, name);
  
  if (template) {
    
    /* change the template definition, and redefine all the instances */
    fts_template_redefine(template, file);
    
  } else {

    fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(file), buf, MAXPATHLEN);

    /* Register the template */
    template = fts_new_template(name, fts_new_symbol(buf));

    /* Create the database if necessary */
    if (pkg->declared_templates == NULL) {
      pkg->declared_templates = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->declared_templates, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

    fts_set_symbol(&n, name);
    fts_set_ptr(&p, template);
    fts_hashtable_put(pkg->declared_templates, &n, &p);  
  }
}

void fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;

  fts_set_symbol(&n, path);
  pkg->template_paths = fts_list_append(pkg->template_paths, &n);
}

fts_template_t *
fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  if (pkg->state == fts_package_defined) {
    if (fts_package_load(pkg) != 0) {
      return NULL;
    }
  }

  fts_set_symbol( &k, name);
  if ((pkg->declared_templates != NULL) 
      && fts_hashtable_get(pkg->declared_templates, &k, &a)) {
    return (fts_template_t *) fts_get_ptr(&a);
  } else {
    return NULL;
  }
}

fts_template_t *
fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  if (pkg->state == fts_package_defined) {
    if (fts_package_load(pkg) != 0) {
      return NULL;
    }
  }

  fts_set_symbol( &k, name);

  if ((pkg->templates_in_path != NULL) 
      && fts_hashtable_get(pkg->templates_in_path, &k, &a)) {
    return (fts_template_t *) fts_get_ptr(&a);
  } else {
    char filename[MAXPATHLEN];
    char path[MAXPATHLEN];
    fts_template_t* t;
    fts_atom_t n, p;

    snprintf(filename, MAXPATHLEN, "%s.jmax", fts_symbol_name(name));
    if (!fts_find_file(pkg->template_paths, filename, path, MAXPATHLEN)) {
      return NULL;
    }

    /* Register the template */
    t = fts_new_template(name, fts_new_symbol_copy(path));

    /* Create the database if necessary */
    if (pkg->templates_in_path == NULL) {
      pkg->templates_in_path = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->templates_in_path, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

    fts_set_symbol(&n, name);
    fts_set_ptr(&p, t);
    fts_hashtable_put(pkg->templates_in_path, &n, &p);  

    return t;
  }
}


fts_template_t* 
fts_package_get_template_from_file(fts_package_t* pkg, fts_symbol_t filename)
{
  fts_iterator_t iter;
  fts_template_t *template;
  fts_atom_t tmpl_atom;
  
  fts_hashtable_get_values(pkg->declared_templates, &iter);

  while ( fts_iterator_has_more( &iter)) {
    fts_iterator_next( &iter, &tmpl_atom);
    template = fts_get_ptr(&tmpl_atom);
    if (fts_template_get_filename(template) == filename) {
      return template;
    }
  }

  fts_hashtable_get_values(pkg->templates_in_path, &iter);

  while ( fts_iterator_has_more( &iter)) {
    fts_iterator_next( &iter, &tmpl_atom);
    template = fts_get_ptr(&tmpl_atom);
    if (fts_template_get_filename(template) == filename) {
      return template;
    }
  }

  return NULL;
}

/********************************************************************
 *
 *   - abstractions
 */

void fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
{
  fts_atom_t n, p;

  if (pkg->abstractions == NULL) {
    pkg->abstractions = fts_malloc(sizeof(fts_hashtable_t));
    fts_hashtable_init(pkg->abstractions, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  }

  fts_set_symbol(&n, name);
  fts_set_symbol(&p, file);
  fts_hashtable_put(pkg->abstractions, &n, &p);
}


/********************************************************************
 *
 *   - help
 */

void fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
{
  fts_atom_t n, p;

  if (pkg->help == NULL) {
    pkg->help = fts_malloc(sizeof(fts_hashtable_t));
    fts_hashtable_init(pkg->help, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  }

  fts_set_symbol(&n, name);
  fts_set_symbol(&p, file);
  fts_hashtable_put(pkg->help, &n, &p);
}

void 
fts_package_add_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;

  fts_set_symbol(&n, path);
  pkg->paths = fts_list_append(pkg->paths, &n);
}

int 
fts_package_load(fts_package_t* pkg)
{
  return -1;
}
