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
#include <ftsprivate/bmaxhdr.h>
#include <ftsprivate/saver.h>
#include <ftsconfig.h>
#include <stdlib.h> 

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#define PACKAGE_STACK_SIZE    32

static fts_hashtable_t fts_packages;
static fts_list_t* fts_package_paths = NULL;

static fts_class_t *fts_package_class = NULL;
static fts_package_t* fts_system_package = NULL;
static fts_package_t* fts_package_stack[PACKAGE_STACK_SIZE];
static int fts_package_stack_top = 0;

static fts_status_description_t fts_DuplicatedMetaclass = {"Duplicated metaclass"};

static fts_symbol_t s_require = 0;
static fts_symbol_t s_template_path = 0;
static fts_symbol_t s_data_path = 0;
static fts_symbol_t s_package = 0;

#ifdef WIN32
#define fts_lib_prefix   ""
#define fts_lib_postfix  ".dll"
#else
#define fts_lib_prefix   "lib"
#define fts_lib_postfix  ".so"
#endif

/***********************************************
 *
 *  Package registry
 */

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

fts_list_t *
fts_get_package_paths(void)
{
  /* initialize the package path */
  if (fts_package_paths == NULL) {
    fts_init_package_paths();
  }
  
  return fts_package_paths;
}

fts_package_t*
fts_package_load_from_file(fts_symbol_t name, const char* filename)
{
  char path[MAXPATHLEN];
  char dir[MAXPATHLEN];
  fts_object_t* obj;
  fts_package_t* pkg = NULL;

  /* this is a hack but not a big one: load the patcher in the context
     of the system package. */
  fts_package_push(fts_system_package);

  fts_make_absolute_path(NULL, filename, path, MAXPATHLEN);

  obj = fts_binary_file_load( path, (fts_object_t *)fts_get_root_patcher(), 0, 0, 0);

  if (!obj) {
    fts_log("[package]: Failed to load package file %s\n", path);
    pkg = fts_package_new(name);
    pkg->state = fts_package_corrupt;
    goto gracefull_exit;
  }

  /* check whether it's a package object */
  if (fts_object_get_class(obj) != fts_package_class) {
/* FIXME: error corruption     fts_object_destroy(obj); */
    fts_log("[package]: Invalid package file %s\n", path);
    pkg = fts_package_new(name);
    pkg->state = fts_package_corrupt;
    goto gracefull_exit;
  }

  /* so, we have a package. now load all the default files. */
  pkg = (fts_package_t*) obj;

 gracefull_exit:

  pkg->name = name;

  fts_dirname(path, dir, MAXPATHLEN);
  pkg->dir = fts_new_symbol_copy(dir);

  fts_package_pop(fts_system_package);
  return pkg;
}

fts_package_t*
fts_package_load(fts_symbol_t name)
{
  fts_package_t* pkg;
  char path[MAXPATHLEN];
  char filename[MAXPATHLEN];

  /* locate the directory of the package */
  if (!fts_find_file(NULL, fts_get_package_paths(), fts_symbol_name(name), path, MAXPATHLEN) 
      || !fts_is_directory(path)) {
    fts_log("[package]: Couldn't find package %s\n", fts_symbol_name(name));
    pkg = fts_package_new(name);
    pkg->state = fts_package_corrupt;
    return pkg;
  }
  
  /* load the definition file */
  sprintf(filename, "%s%c%s.jmax", path, fts_file_separator, fts_symbol_name(name));

  if (fts_file_exists(filename)) {
    pkg = fts_package_load_from_file(name, filename);
    fts_log("[package]: Loaded %s package definition\n", fts_symbol_name(name));
  } else {
    pkg = fts_package_new(name);
    pkg->state = fts_package_defined;
    pkg->name = name;
    pkg->dir = fts_new_symbol_copy(path);
  }
  
  /* load the default files */
  fts_package_load_default_files(pkg);
  
  return pkg;
}

fts_package_t* 
fts_package_get(fts_symbol_t name)
{
  fts_package_t* pkg;
  fts_atom_t n, p;

  fts_set_symbol(&n, name);

  if (fts_hashtable_get(&fts_packages, &n, &p)) {
    return fts_get_ptr(&p);
  } else {
    /* try to load it */
    pkg = fts_package_load(name);

    fts_set_ptr(&p, pkg);
    fts_hashtable_put(&fts_packages, &n, &p);    

    return pkg;
  }
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
fts_package_push(fts_package_t* pkg)
{
  if (fts_package_stack_top < PACKAGE_STACK_SIZE) {
    fts_package_stack[fts_package_stack_top++] = pkg;
  } else {
    post("Package stack overflow\n");
  }
}

void 
fts_package_pop(fts_package_t* pkg)
{
  if (fts_get_current_package() != pkg) {
    post("Warning: fts_package_pop: interleaved push-pop pairs\n");
  }

  if (fts_package_stack_top > 0) {
    fts_package_stack[--fts_package_stack_top] = NULL;
  }
}

/********************************************************************
 *
 *   - load 
 */

void
fts_package_load_default_files(fts_package_t* pkg)
{
  fts_list_t* list;
  char filename[MAXPATHLEN];
  char function[256];
  fts_status_t ret;

  /* load all required packages before loading anything else. This
     package may depend on the libraries and other data loaded by the
     required packages. */
  list = pkg->packages;
  while (list) {
    fts_package_get(fts_get_symbol(fts_list_get(list)));
    list = fts_list_next(list);
  }

  fts_package_push(pkg);

  /* load the shared library */
  sprintf(filename, "%s%c%s%c%s%s%s", 
	  fts_symbol_name(pkg->dir), fts_file_separator, 
	  "c", fts_file_separator, 
	  fts_lib_prefix, fts_symbol_name(pkg->name), fts_lib_postfix);

  if (fts_file_exists(filename)) {
    snprintf(function, 256, "%s_config", fts_symbol_name(pkg->name));
    ret = fts_load_library(filename, function);
    if (ret != fts_Success) {
      fts_log("[package]: Error loading library of package %s: %s\n", fts_symbol_name(pkg->name), ret->description);
    } else {
      fts_log("[package]: Loaded %s library\n", fts_symbol_name(pkg->name));
    }
  } else {
    fts_log("[package]: Didn't found no library for %s (tried %s)\n", fts_symbol_name(pkg->name), filename);
  }

  fts_package_pop(pkg);
}

/********************************************************************
 *
 *   - required packages 
 */

void 
fts_package_require(fts_package_t* pkg, fts_symbol_t required_pkg)
{
  fts_atom_t n;

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

  /* If the declaration existed already, remove it first */
  template = fts_package_get_declared_template(pkg, name);
  
  if (template) 
    {
      /* change the template definition, and redefine all the instances */
      fts_template_redefine(template, file);
    } 
  else 
    {
      
      /* Create the database if necessary */
      if (pkg->declared_templates == NULL) 
	{
	  pkg->declared_templates = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
	  fts_hashtable_init(pkg->declared_templates, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
	}
      
      /* Register the template */
      template = fts_template_new(name, NULL, file);

      fts_set_symbol(&n, name);
      fts_set_ptr(&p, template);
      fts_hashtable_put(pkg->declared_templates, &n, &p);

      fts_template_set_package(template, pkg);
    }
}

void 
fts_package_add_template_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;

  fts_set_symbol(&n, path);
  pkg->template_paths = fts_list_append(pkg->template_paths, &n);
}

fts_template_t *
fts_package_get_declared_template(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;
  char buf[MAXPATHLEN];

  fts_set_symbol( &k, name);

  if ((pkg->declared_templates != NULL) && fts_hashtable_get(pkg->declared_templates, &k, &a)) 
    {
      fts_template_t *template = (fts_template_t *) fts_get_ptr(&a);
      
      if (fts_template_get_filename(template) == NULL) 
	{
	  fts_make_absolute_path(fts_symbol_name(pkg->dir), fts_symbol_name(fts_template_get_original_filename(template)), buf, MAXPATHLEN);
	  fts_template_set_filename(template, fts_new_symbol_copy(buf));
	}
      
      return template;
    } 
  else
    return NULL;
}

fts_template_t *
fts_package_get_template_in_path(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  fts_set_symbol( &k, name);

  if ((pkg->templates_in_path != NULL) && fts_hashtable_get(pkg->templates_in_path, &k, &a)) 
    return (fts_template_t *) fts_get_ptr(&a);
  else
    {
      char filename[MAXPATHLEN];
      char path[MAXPATHLEN];
      fts_template_t* template;
      fts_atom_t n, p;
      const char* root;
      
      root = (pkg->dir != NULL)? fts_symbol_name(pkg->dir) : NULL;
      
      snprintf(filename, MAXPATHLEN, "%s.jmax", fts_symbol_name(name));

      if (!fts_find_file(root, pkg->template_paths, filename, path, MAXPATHLEN))
	return NULL;
      
      /* Register the template */
      template = fts_template_new(name, fts_new_symbol_copy(path), NULL);
      
      /* Create the database if necessary */
      if (pkg->templates_in_path == NULL) 
	{
	  pkg->templates_in_path = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
	  fts_hashtable_init(pkg->templates_in_path, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
	}
      
      fts_set_symbol(&n, name);
      fts_set_ptr(&p, template);
      fts_hashtable_put(pkg->templates_in_path, &n, &p);  

      fts_template_set_package(template, pkg);
      
      return template;
    }
}


fts_template_t * 
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

  if (pkg->templates_in_path == NULL) {
    return NULL;    
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

  /* If the declaration existed already, remove it first */
  abstraction = fts_package_get_declared_abstraction(pkg, name);
  
  if (abstraction) {

    /* FIXME */
    
    /* change the abstraction definition, and redefine all the instances */
/*      fts_abstraction_redefine(abstraction, file); */

  } else {

    /* Create the database if necessary */
    if (pkg->declared_abstractions == NULL) {
      pkg->declared_abstractions = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->declared_abstractions, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

    /* Register the abstraction */
    abstraction = fts_abstraction_new(name, NULL, file);
    fts_set_symbol(&n, name);
    fts_set_ptr(&p, abstraction);
    fts_hashtable_put(pkg->declared_abstractions, &n, &p);  
  }
}

void fts_package_add_abstraction_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;

  fts_set_symbol(&n, path);
  pkg->abstraction_paths = fts_list_append(pkg->abstraction_paths, &n);
}

fts_abstraction_t *
fts_package_get_declared_abstraction(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;
  fts_abstraction_t *abstraction;
  char buf[MAXPATHLEN];

  fts_set_symbol( &k, name);
  if ((pkg->declared_abstractions != NULL) 
      && fts_hashtable_get(pkg->declared_abstractions, &k, &a)) {
    abstraction = (fts_abstraction_t *) fts_get_ptr(&a);

    if (fts_abstraction_get_filename(abstraction) == NULL) {
      fts_make_absolute_path(fts_symbol_name(pkg->dir), 
			     fts_symbol_name(fts_abstraction_get_original_filename(abstraction)), 
			     buf, MAXPATHLEN);
      fts_abstraction_set_filename(abstraction, fts_new_symbol_copy(buf));
    }

    return abstraction;
  } else {
    return NULL;
  }
}

fts_abstraction_t *
fts_package_get_abstraction_in_path(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t a, k;

  fts_set_symbol( &k, name);

  if ((pkg->abstractions_in_path != NULL) 
      && fts_hashtable_get(pkg->abstractions_in_path, &k, &a)) {
    return (fts_abstraction_t *) fts_get_ptr(&a);
  } else {
    char filename[MAXPATHLEN];
    char path[MAXPATHLEN];
    fts_abstraction_t* t;
    fts_atom_t n, p;
    const char* root;

    root = (pkg->dir != NULL)? fts_symbol_name(pkg->dir) : NULL;

    snprintf(filename, MAXPATHLEN, "%s.abs", fts_symbol_name(name));
    if (!fts_find_file(root, pkg->abstraction_paths, filename, path, MAXPATHLEN)) {
      return NULL;
    }

    /* Register the abstraction */
    t = fts_abstraction_new(name, fts_new_symbol_copy(path), fts_new_symbol_copy(filename));

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
fts_package_add_metaclass( fts_package_t* pkg, fts_metaclass_t *mcl)
{
  fts_symbol_t name = mcl->name;
  fts_atom_t data, k;

  /* Create the database if necessary */
  if (pkg->classes == NULL) 
    {
      pkg->classes = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init(pkg->classes, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
    }

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(pkg->classes, &k, &data))
    return &fts_DuplicatedMetaclass;
  else
    {
      fts_set_ptr(&data, mcl);
      fts_hashtable_put(pkg->classes, &k, &data);
    }

  fts_metaclass_set_package(mcl, pkg);

  return fts_Success;
}

fts_metaclass_t *
fts_package_get_metaclass(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t data, k;
  
  fts_set_symbol( &k, name);
  
  if ((pkg->classes != NULL) && fts_hashtable_get(pkg->classes, &k, &data)) 
    return fts_get_ptr(&data);
  else 
    return NULL;
}

fts_status_t 
fts_package_add_alias(fts_package_t* pkg, fts_symbol_t alias, fts_symbol_t name)
{
  fts_metaclass_t *mcl = fts_package_get_metaclass(pkg, name);
  fts_atom_t data, k;
  
  fts_set_symbol( &k, alias);
  if (fts_hashtable_get(pkg->classes, &k, &data))
    return &fts_DuplicatedMetaclass;
  else
    {
      fts_set_ptr(&data, mcl);
      fts_hashtable_put(pkg->classes, &k, &data);
    }
  
  return fts_Success;
}

/********************************************************************
 *
 *   - data files
 */

void 
fts_package_add_data_path(fts_package_t* pkg, fts_symbol_t path)
{
  fts_atom_t n;

  fts_set_symbol(&n, path);
  pkg->data_paths = fts_list_append(pkg->data_paths, &n);
}

int 
fts_package_get_data_file(fts_package_t* pkg, fts_symbol_t filename, char *buf, int len)
{
  const char* root = (pkg->dir != NULL)? fts_symbol_name(pkg->dir) : NULL;

  return fts_find_file(root, pkg->data_paths, fts_symbol_name(filename), buf, len);
}

/********************************************************************
 *
 *   - help
 */

void 
fts_package_add_help(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
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

/***********************************************
 *
 *  print and save utilities for methods
 */

static void 
fun_template( fts_atom_t *a)
{
  fts_template_t *template = (fts_template_t *)fts_get_ptr( a);

  fts_set_symbol( a, fts_template_get_original_filename( template));
}

static void 
fun_abstraction( fts_atom_t *a)
{
  fts_abstraction_t *abstraction = (fts_abstraction_t *)fts_get_ptr( a);

  /* Must be done the same way as in templates */
/*    fts_set_symbol( a, fts_abstraction_get_original_filename( template)); */
}

static int 
fts_package_save_list_aux( fts_bmax_file_t *f, fts_list_t *list)
{
  int r;

  if (!list)
    return 0;

  r = fts_package_save_list_aux( f, fts_list_next( list));

  fts_bmax_code_push_symbol( f, fts_get_symbol( fts_list_get( list)));

  return r+1;
}

static void 
fts_package_save_list( fts_bmax_file_t *f, fts_list_t *list, fts_symbol_t selector)
{
  int ac;

  ac = fts_package_save_list_aux( f, list);
  fts_bmax_code_obj_mess( f, fts_SystemInlet, selector, ac);
  fts_bmax_code_pop_args( f, ac);
}

static void 
fts_package_save_hashtable( fts_bmax_file_t *f, fts_hashtable_t *ht, fts_symbol_t selector, void (*fun)(fts_atom_t *))
{
  fts_iterator_t keys, values;

  fts_hashtable_get_keys( ht, &keys);
  fts_hashtable_get_values( ht, &values);

  while ( fts_iterator_has_more( &keys))
    {
      fts_atom_t a[2];

      fts_iterator_next( &keys, a);
      fts_iterator_next( &values, a+1);

      if (fun != NULL)
	(*fun)(a+1);

      fts_bmax_code_push_atoms(f, 2, a);
      fts_bmax_code_obj_mess(f, fts_SystemInlet, selector, 2);
      fts_bmax_code_pop_args(f, 2);
    }
}

static void 
fts_package_print_list_aux( fts_list_t *list)
{
  if (!list)
    return;

  post( " %s", fts_symbol_name( fts_get_symbol( fts_list_get( list))));

  fts_package_print_list_aux( fts_list_next( list));
}

static void 
fts_package_print_list( fts_list_t *list, fts_symbol_t selector)
{
  post( "  %s", fts_symbol_name( selector));

  fts_package_print_list_aux( list);

  post( "\n");
}

static void 
fts_package_print_hashtable( fts_hashtable_t *ht, fts_symbol_t selector, void (*fun)(fts_atom_t *))
{
  const char *msg = fts_symbol_name( selector);
  fts_iterator_t keys, values;

  fts_hashtable_get_keys( ht, &keys);
  fts_hashtable_get_values( ht, &values);

  while ( fts_iterator_has_more( &keys))
    {
      fts_atom_t a[2];

      fts_iterator_next( &keys, a);
      fts_iterator_next( &values, a+1);

      if (fun != NULL)
	(*fun)(a+1);

      post( "  %s %s %s\n", msg, fts_symbol_name( fts_get_symbol( a)), fts_symbol_name( fts_get_symbol( a+1)));
    }
}

/***********************************************
 *
 *  methods 
 */

static void 
__fts_package_require(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      fts_package_require(pkg, fts_get_symbol(&at[i]));
    }
  }
}

static void 
__fts_package_template(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  if ((ac >= 2) && fts_is_symbol(&at[0]) && fts_is_symbol(&at[1])) {
    fts_package_add_template(pkg, fts_get_symbol(&at[0]), fts_get_symbol(&at[1]));
  }
}

static void 
__fts_package_template_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      fts_package_add_template_path(pkg, fts_get_symbol(&at[i]));
    }
  }
}

static void 
__fts_package_abstraction(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  if ((ac >= 2) && fts_is_symbol(&at[0]) && fts_is_symbol(&at[1])) {
    fts_package_add_abstraction(pkg, fts_get_symbol(&at[0]), fts_get_symbol(&at[1]));
  }
}

static void 
__fts_package_abstraction_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      fts_package_add_abstraction_path(pkg, fts_get_symbol(&at[i]));
    }
  }
}

static void 
__fts_package_data_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;
  
  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      fts_package_add_data_path(pkg, fts_get_symbol(&at[i]));
    }
  }
}

static void 
__fts_package_help(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  if ((ac >= 2) && fts_is_symbol(&at[0]) && fts_is_symbol(&at[1])) {
    fts_package_add_help(pkg, fts_get_symbol(&at[0]), fts_get_symbol(&at[1]));
  }
}

static void 
__fts_package_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;
  fts_bmax_file_t f;
  const char *filename;

  if (ac == 0) {
    post( "No filename specified\n");    
    return;
  }

  filename = fts_symbol_name( fts_get_symbol( at));

  if (fts_bmax_file_open( &f, filename, 0, 0, 0) < 0)
    {
      post( "Cannot open file %s\n", filename);
      return;
    }

  fts_bmax_code_new_object( &f, o, -1);

  if ( this->packages) {
    fts_package_save_list( &f, this->packages, fts_s_require);
  }
  if ( this->template_paths) {
    fts_package_save_list( &f, this->template_paths, fts_s_template_path);
  }
  if ( this->abstraction_paths) {
    fts_package_save_list( &f, this->abstraction_paths, fts_s_abstraction_path);
  }
  if ( this->data_paths) {
    fts_package_save_list( &f, this->data_paths, fts_s_data_path);
  }

  if ( this->declared_templates) {
    fts_package_save_hashtable( &f, this->declared_templates, fts_s_template, fun_template);
  }
  if ( this->declared_abstractions) {
    fts_package_save_hashtable( &f, this->declared_abstractions, fts_s_abstraction, fun_abstraction);
  }
  if ( this->help) {
    fts_package_save_hashtable( &f, this->help, fts_s_help, NULL);
  }

  fts_bmax_code_return( &f);

  fts_bmax_file_close( &f);
}

static void 
__fts_package_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;

  post( "package %s:\n", fts_symbol_name( fts_package_get_name( this)));

  if ( this->packages) {
    fts_package_print_list( this->packages, fts_s_require);
  }
  if ( this->template_paths) {
    fts_package_print_list( this->template_paths, fts_s_template_path);
  }
  if ( this->abstraction_paths) {
    fts_package_print_list( this->abstraction_paths, fts_s_abstraction_path);
  }
  if ( this->data_paths) {
    fts_package_print_list( this->data_paths, fts_s_data_path);
  }

  if ( this->declared_templates) {
    fts_package_print_hashtable( this->declared_templates, fts_s_template, fun_template);
  }
  if ( this->declared_abstractions) {
    fts_package_print_hashtable( this->declared_abstractions, fts_s_abstraction, fun_abstraction);
  }
  if ( this->help) {
    fts_package_print_hashtable( this->help, fts_s_help, NULL);
  }
}

/***********************************************
 *
 *  Package class 
 */

void 
__fts_package_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  ac--;
  at++;

  if ((ac > 0) && fts_is_symbol(at)) { 
    pkg->name = fts_get_symbol(at);
  } else {
    pkg->name = NULL;
  }
  pkg->dir = NULL;
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
}

void 
__fts_package_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
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
}

static fts_status_t
fts_package_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_package_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, __fts_package_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, __fts_package_delete);

  fts_method_define_varargs(cl, 0, fts_s_print, __fts_package_print);

  fts_method_define_varargs(cl, 0, fts_s_require, __fts_package_require);
  fts_method_define_varargs(cl, 0, fts_s_template, __fts_package_template);
  fts_method_define_varargs(cl, 0, fts_s_template_path, __fts_package_template_path);
  fts_method_define_varargs(cl, 0, fts_s_abstraction, __fts_package_abstraction);
  fts_method_define_varargs(cl, 0, fts_s_abstraction_path, __fts_package_abstraction_path);
  fts_method_define_varargs(cl, 0, fts_s_data_path, __fts_package_data_path);
  fts_method_define_varargs(cl, 0, fts_s_help, __fts_package_help);
  fts_method_define_varargs(cl, 0, fts_s_save, __fts_package_save);

  /* All these methods are also defined for SystemInlet, as bmax saving
     allows only messages to SystemInlet... 
  */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_require, __fts_package_require);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_template, __fts_package_template);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_template_path, __fts_package_template_path);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_abstraction, __fts_package_abstraction);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_abstraction_path, __fts_package_abstraction_path);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_data_path, __fts_package_data_path);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save, __fts_package_save);

  return fts_Success;
}

/***********************************************
 * 
 *  package object: old methods 
 */

fts_package_t* 
fts_package_new(fts_symbol_t name)
{
  fts_atom_t a[1];
  
  fts_set_symbol(&a[0], name);
  return (fts_package_t *) fts_object_create(fts_package_class, 1, a);
}

void 
fts_package_delete(fts_package_t* pkg)
{
  fts_object_destroy( (fts_object_t*) pkg);
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

/***************************************************
 *
 * Debug code to test saving a package
 *
 */

static void loader_load(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[1];
  fts_status_t status;

  /* Load the .jmax file */
  obj = fts_binary_file_load( fts_symbol_name( fts_get_symbol( at)), (fts_object_t *)fts_get_root_patcher(), 0, a, 0);

  if (!obj)
    {
      post( "Load failed\n");
      return;
    }

  /* Send a "print" message to the result */
  if ( (status = fts_send_message( obj, 0, fts_s_print, 0, 0)) != fts_Success)
    post( "Message send failed (%s)\n", fts_status_get_description(status));

  if (fts_object_get_class(obj) == fts_package_class) {
    post( "Loaded package\n");
  }
}

static fts_status_t loader_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof(fts_object_t), 1, 0, 0);

  fts_method_define_varargs(cl, 0, fts_new_symbol( "load"), loader_load);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */
void 
fts_kernel_package_init(void)
{
  fts_symbol_t system_symbol;
  fts_atom_t a, p;
  int i;

  for (i = 0; i < PACKAGE_STACK_SIZE; i++) {
    fts_package_stack[i] = NULL;
  }

  fts_hashtable_init(&fts_packages, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

  /* create the system package */
  system_symbol = fts_new_symbol("_system_");
  fts_s_package = fts_new_symbol("package");
  fts_set_symbol(&a, fts_s_package);

  /* since we need a class to construct a package and a package to
     find a class, we have to do some non standard hacking to
     bootstrap the system */
  fts_system_package = fts_calloc(sizeof(fts_package_t));

  fts_system_package->object.head.cl = 0;
  fts_system_package->object.head.id = FTS_NO_ID;
  fts_system_package->object.properties = 0;
  fts_system_package->object.varname = 0;
  fts_system_package->object.refcnt = 0;
  fts_system_package->object.out_conn = 0;
  fts_system_package->object.in_conn = (fts_connection_t **) fts_calloc(sizeof(fts_connection_t *));
  
  __fts_package_init((fts_object_t*) fts_system_package, 0, fts_s_init, 1, &a); 
  fts_package_set_state(fts_system_package, fts_package_loaded); 

  fts_set_ptr(&p, fts_system_package);
  fts_hashtable_put(&fts_packages, &a, &p);

  fts_package_push(fts_system_package);

  /* now that there's a system package, we can define the package
     (meta-) class */
  fts_metaclass_install(fts_s_package, fts_package_instantiate, fts_always_equiv);

  /* update the system package with the correct class */
  fts_package_class = fts_class_get_by_name(fts_s_package);
  fts_system_package->object.head.cl = fts_package_class;

  /* Debug code */
  fts_class_install( fts_new_symbol( "loader"), loader_instantiate);
}
