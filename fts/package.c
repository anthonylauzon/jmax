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
#include <ftsprivate/abstraction.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/class.h>
#include <ftsconfig.h>
#include <stdlib.h> 

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#define PACKAGE_STACK_SIZE    32

static fts_hashtable_t fts_packages;
static fts_list_t* fts_package_paths = NULL;

static fts_package_t* fts_system_package = NULL;
static fts_package_t* fts_package_stack[PACKAGE_STACK_SIZE];
static int fts_package_stack_top = 0;

static fts_status_description_t fts_DuplicatedMetaclass = {"Duplicated metaclass"};

static fts_symbol_t fts_s_require;
static fts_symbol_t fts_s_template_path;
static fts_symbol_t fts_s_data_path;


#ifdef WIN32
#define fts_lib_prefix   ""
#define fts_lib_postfix  ".dll"
#else
#define fts_lib_prefix   "lib"
#define fts_lib_postfix  ".so"
#endif

/***********************************************************************
 *
 * Initialization
 *
 */
void 
fts_kernel_package_init(void)
{
  fts_symbol_t system_symbol;
  int i;

  for (i = 0; i < PACKAGE_STACK_SIZE; i++) {
    fts_package_stack[i] = NULL;
  }

  fts_hashtable_init(&fts_packages, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

  /* create the system package */
  system_symbol = fts_new_symbol("_system_");
  fts_register_package(system_symbol);
  fts_system_package = fts_get_package(system_symbol);
  fts_package_set_state(fts_system_package, fts_package_loaded); /* FIXME: hack [pH07] */
  fts_push_package(fts_system_package);

  /* create the package configuration objects */
  fts_s_require = fts_new_symbol("require");
  fts_s_template_path = fts_new_symbol("template-path");
  fts_s_data_path = fts_new_symbol("data-path");
  
  fts_metaclass_install(fts_s_require, fts_require_instantiate, fts_always_equiv);
  fts_metaclass_install(fts_s_template, fts_template_instantiate, fts_always_equiv);
  fts_metaclass_install(fts_s_template_path, fts_template_path_instantiate, fts_always_equiv);
  fts_metaclass_install(fts_s_data_path, fts_data_path_instantiate, fts_always_equiv);
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

static void
fts_init_package_paths(void)
{
  fts_atom_t a;
  fts_symbol_t s;
  char path[MAXPATHLEN];

  /* add the default package path */
  if (fts_package_paths == NULL) {
    s = fts_get_root_directory();
    if (s) {
      snprintf(path, MAXPATHLEN, "%s%c%s", fts_symbol_name(s), fts_file_separator, "packages");
      fts_set_symbol(&a, fts_new_symbol_copy(path));
      fts_package_paths = fts_list_append(fts_package_paths, &a);
    }
  }
}

fts_list_t*
fts_get_package_paths(void)
{
  /* initialize the package path */
  if (fts_package_paths == NULL) {
    fts_init_package_paths();
  }
  
  return fts_package_paths;
}

/***********************************************
 *
 *  Package context and package stack
 */

fts_package_t* 
fts_get_current_package(void)
{
  return (fts_package_stack_top > 0)? fts_package_stack[fts_package_stack_top - 1] : NULL;
}

fts_package_t* 
fts_get_system_package(void)
{
  return fts_system_package;
}

void 
fts_push_package(fts_package_t* pkg)
{
  if (fts_package_stack_top < PACKAGE_STACK_SIZE) {
    fts_package_stack[fts_package_stack_top++] = pkg;
  } else {
    post("Package stack overflow\n");
  }
}

void 
fts_pop_package(void)
{
  if (fts_package_stack_top > 0) {
    fts_package_stack[--fts_package_stack_top] = NULL;
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
  pkg->patcher = NULL;
  pkg->packages = NULL;
  pkg->classes = NULL;

  pkg->declared_templates = NULL;
  pkg->templates_in_path = NULL;
  pkg->template_paths = NULL;

  pkg->declared_abstractions = NULL;
  pkg->abstractions_in_path = NULL;
  pkg->abstraction_paths = NULL;

  pkg->help = NULL;
  pkg->data_paths = NULL;

  return pkg;
}

void 
delete_fts_package(fts_package_t* pkg)
{
  if (pkg->packages != NULL) {
    fts_list_delete(pkg->packages);
  }
  if (pkg->data_paths != NULL) {
    fts_list_delete(pkg->data_paths);
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
  if (pkg->template_paths != NULL) {
    fts_list_delete(pkg->template_paths);
  }
  if (pkg->declared_abstractions != NULL) {
    fts_hashtable_destroy(pkg->declared_abstractions);
  }
  if (pkg->abstractions_in_path != NULL) {
    fts_hashtable_destroy(pkg->abstractions_in_path);
  }
  if (pkg->abstraction_paths != NULL) {
    fts_list_delete(pkg->abstraction_paths);
  }
  if (pkg->help != NULL) {
    fts_hashtable_destroy(pkg->help);
  }
  if (pkg->patcher != NULL) {
    fts_object_destroy(pkg->patcher);
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

void 
fts_package_set_state(fts_package_t* pkg, fts_package_state_t s)
{
  pkg->state = s;
}

char* 
fts_package_get_error(fts_package_t* pkg)
{
  return pkg->error;
}

int 
fts_package_load(fts_package_t* pkg)
{
  fts_list_t* list;
  fts_package_t* required_pkg;
  char path[MAXPATHLEN];
  char filename[MAXPATHLEN];
  char function[256];
  fts_status_t ret;

  /* don't try to load it twice */
  if (pkg->state != fts_package_defined) {
    return (pkg->state == fts_package_corrupt)? -1 : 0;
  }

  pkg->state = fts_package_loading;

  /* locate the directory of the package */
  if (!fts_find_file(fts_get_package_paths(), fts_symbol_name(pkg->name), path, MAXPATHLEN) 
      || !fts_is_directory(path)) {
    fprintf(stderr, "Couldn't find package %s\n", fts_symbol_name(pkg->name));
    pkg->state = fts_package_corrupt;
    return -1;
  }
  pkg->dir = fts_new_symbol_copy(path);

  /* push the current package on the stack */
  fts_push_package(pkg);
  
  /* load the definition patcher */
  sprintf(filename, "%s%c%s.jmax", fts_symbol_name(pkg->dir), fts_file_separator, fts_symbol_name(pkg->name));

  if (fts_file_exists(filename)) {
    pkg->patcher = fts_binary_file_load(filename, (fts_object_t*) fts_get_root_patcher(), 0, 0, 0);
  }

  /* load all required packages, before loading anything else. This
     package may depend on the libraries and other data loaded by the
     required packages. */
  list = pkg->packages;
  while (list) {
    required_pkg = fts_get_package(fts_get_symbol(fts_list_get(list)));
    fts_package_load(required_pkg);
    list = fts_list_next(list);
  }

  /* load the shared library */
  sprintf(filename, "%s%c%s%c%s%s%s", 
	  fts_symbol_name(pkg->dir), fts_file_separator, 
	  "c", fts_file_separator, 
	  fts_lib_prefix, fts_symbol_name(pkg->name), fts_lib_postfix);

  if (fts_file_exists(filename)) {
    snprintf(function, 256, "%s_config", fts_symbol_name(pkg->name));
    ret = fts_load_library(filename, function);
    if (ret != fts_Success) {
      fprintf(stderr, "Error loading library of package %s: %s\n", fts_symbol_name(pkg->name), ret->description);
    } else {
      fprintf(stderr, "debug: loaded library %s\n", fts_symbol_name(pkg->name));
    }
  } else {
    fprintf(stderr, "debug: no found no library for %s (tried %s)\n", fts_symbol_name(pkg->name), filename);
  }

  /* pop the current package of the stack */
  fts_pop_package();

  pkg->state = fts_package_loaded;

  return 0;
}

/********************************************************************
 *
 *   - required packages 
 */

void 
fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg)
{
  fts_atom_t n;

  /* make sure the required package is registered globally */
  fts_register_package(required_pkg);

  fts_set_symbol(&n, required_pkg);
  pkg->packages = fts_list_append(pkg->packages, &n);
}

void 
fts_package_get_required_packages(fts_package_t* pkg, fts_iterator_t* iter)
{
  fts_list_get_values(pkg->packages, iter);
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
    template = fts_new_template(name, fts_new_symbol_copy(buf));

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
  char buf[MAXPATHLEN];

  fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(path), buf, MAXPATHLEN);

  fts_set_symbol(&n, fts_new_symbol_copy(buf));
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
  
  if (pkg->declared_templates == NULL) {
    return NULL;
  }

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


void 
fts_package_add_abstraction(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
{
  fts_abstraction_t *abstraction;
  fts_atom_t n, p;
  char buf[MAXPATHLEN];

  /* If the declaration existed already, remove it first */
  abstraction = fts_package_get_declared_abstraction(pkg, name);
  
  if (abstraction) {

    /* FIXME */
    
    /* change the abstraction definition, and redefine all the instances */
/*      fts_abstraction_redefine(abstraction, file); */

  } else {

    fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(file), buf, MAXPATHLEN);

    /* Register the abstraction */
    abstraction = fts_new_abstraction(name, fts_new_symbol_copy(buf));

    /* Create the database if necessary */
    if (pkg->declared_abstractions == NULL) {
      pkg->declared_abstractions = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->declared_abstractions, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

    fts_set_symbol(&n, name);
    fts_set_ptr(&p, abstraction);
    fts_hashtable_put(pkg->declared_abstractions, &n, &p);  
  }
}

void fts_package_add_abstraction_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;
  char buf[MAXPATHLEN];

  fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(path), buf, MAXPATHLEN);

  fts_set_symbol(&n, fts_new_symbol_copy(buf));
  pkg->abstraction_paths = fts_list_append(pkg->abstraction_paths, &n);
}

fts_abstraction_t *
fts_package_get_declared_abstraction(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  if (pkg->state == fts_package_defined) {
    if (fts_package_load(pkg) != 0) {
      return NULL;
    }
  }

  fts_set_symbol( &k, name);
  if ((pkg->declared_abstractions != NULL) 
      && fts_hashtable_get(pkg->declared_abstractions, &k, &a)) {
    return (fts_abstraction_t *) fts_get_ptr(&a);
  } else {
    return NULL;
  }
}

fts_abstraction_t *
fts_package_get_abstraction_in_path(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  if (pkg->state == fts_package_defined) {
    if (fts_package_load(pkg) != 0) {
      return NULL;
    }
  }

  fts_set_symbol( &k, name);

  if ((pkg->abstractions_in_path != NULL) 
      && fts_hashtable_get(pkg->abstractions_in_path, &k, &a)) {
    return (fts_abstraction_t *) fts_get_ptr(&a);
  } else {
    char filename[MAXPATHLEN];
    char path[MAXPATHLEN];
    fts_abstraction_t* t;
    fts_atom_t n, p;

    snprintf(filename, MAXPATHLEN, "%s.abs", fts_symbol_name(name));
    if (!fts_find_file(pkg->abstraction_paths, filename, path, MAXPATHLEN)) {
      return NULL;
    }

    /* Register the abstraction */
    t = fts_new_abstraction(name, fts_new_symbol_copy(path));

    /* Create the database if necessary */
    if (pkg->abstractions_in_path == NULL) {
      pkg->abstractions_in_path = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->abstractions_in_path, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

    fts_set_symbol(&n, name);
    fts_set_ptr(&p, t);
    fts_hashtable_put(pkg->abstractions_in_path, &n, &p);  

    return t;
  }
}


/********************************************************************
 *
 *   - classes 
 */

fts_status_t 
fts_package_add_metaclass( fts_package_t* pkg, 
			   fts_symbol_t name, 
			   fts_instantiate_fun_t instantiate_fun, 
			   fts_equiv_fun_t equiv_fun)
{
  fts_atom_t data, k;
  fts_metaclass_t *mcl;

  /* Create the database if necessary */
  if (pkg->classes == NULL) {
    pkg->classes = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
    fts_hashtable_init(pkg->classes, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  }

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(pkg->classes, &k, &data))
    {
      return &fts_DuplicatedMetaclass;
    }
  else
    {
      mcl = fts_new_metaclass(name, instantiate_fun, equiv_fun);
      fts_set_ptr(&data, mcl);
      fts_hashtable_put(pkg->classes, &k, &data);
    }

  return fts_Success;
}

fts_status_t 
fts_package_add_metaclass_alias(fts_package_t* pkg, fts_symbol_t new_name, fts_symbol_t old_name)
{
  fts_atom_t data, k;
  fts_metaclass_t *mcl;


  /* Create the database if necessary */
  if (pkg->classes == NULL) {
    pkg->classes = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
    fts_hashtable_init(pkg->classes, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  }

  fts_set_symbol( &k, new_name);
  if (fts_hashtable_get(pkg->classes, &k, &data))
    {
      return &fts_DuplicatedMetaclass;
    }
  else
    {
      mcl = fts_package_get_metaclass(pkg, old_name);
      fts_set_ptr(&data, mcl);
      fts_hashtable_put(pkg->classes, &k, &data);
    }

  return fts_Success;
}

fts_metaclass_t*
fts_package_get_metaclass(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t data, k;

  if (pkg->state == fts_package_defined) {
    if (fts_package_load(pkg) != 0) {
      return NULL;
    }
  }

  fts_set_symbol( &k, name);

  if ((pkg->classes != NULL) 
      && fts_hashtable_get(pkg->classes, &k, &data)) {
    return fts_get_ptr(&data);
  } else {
    return NULL;
  }
}

/********************************************************************
 *
 *   - data files
 */

void 
fts_package_add_data_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;
  char buf[MAXPATHLEN];

  fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(path), buf, MAXPATHLEN);

  fts_set_symbol(&n, fts_new_symbol_copy(buf));
  pkg->data_paths = fts_list_append(pkg->data_paths, &n);
}

int 
fts_package_get_data_file(fts_package_t* pkg, fts_symbol_t filename, char *buf, int len)
{
  return fts_find_file(pkg->data_paths, fts_symbol_name(filename), buf, len);
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

/********************************************************************
 *
 *   Config objects
 */

fts_status_t
fts_require_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_require_init);
  return fts_Success;
}

void 
fts_require_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg = fts_get_current_package();
  int i;
  
  ac--;
  at++;
  
  if (fts_package_get_state(pkg) == fts_package_loading) {
    for (i = 0; i < ac; i++) {
      if (fts_is_symbol(&at[i])) {
	fts_package_require(pkg, fts_get_symbol(&at[i]));
      }
    }
  }
}

fts_status_t
fts_template_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_template_init);
  return fts_Success;
}

void 
fts_template_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg = fts_get_current_package();
  
  ac--;
  at++;
  
  if ((fts_package_get_state(pkg) == fts_package_loading) 
      && (ac >= 2) 
      && fts_is_symbol(&at[0])
      && fts_is_symbol(&at[1])) {
	fts_package_add_template(pkg, fts_get_symbol(&at[0]), fts_get_symbol(&at[1]));
  }
}

fts_status_t
fts_template_path_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_template_path_init);
  return fts_Success;
}

void 
fts_template_path_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg = fts_get_current_package();
  int i;
  
  ac--;
  at++;
  
  if (fts_package_get_state(pkg) == fts_package_loading) {
    for (i = 0; i < ac; i++) {
      if (fts_is_symbol(&at[i])) {
	fts_package_add_template_path(pkg, fts_get_symbol(&at[i]));
      }
    }
  }
}

fts_status_t
fts_data_path_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_data_path_init);
  return fts_Success;
}

void 
fts_data_path_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg = fts_get_current_package();
  int i;
  
  ac--;
  at++;
  
  if (fts_package_get_state(pkg) == fts_package_loading) {
    for (i = 0; i < ac; i++) {
      if (fts_is_symbol(&at[i])) {
	fts_package_add_data_path(pkg, fts_get_symbol(&at[i]));
      }
    }
  }
}
