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
#include <ftsconfig.h>

#include <stdlib.h> 
#include <string.h> 

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <ftsprivate/package.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/template.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/class.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/client.h>
#include <ftsprivate/config.h>

#define PACKAGE_STACK_SIZE    32

fts_symbol_t s_setNames = 0;
fts_symbol_t s_hasSummary = 0;
fts_symbol_t s_updateDone = 0;
fts_symbol_t s_uploadDone = 0;

static fts_hashtable_t fts_packages;
static fts_list_t* fts_package_paths = NULL;

static fts_class_t *fts_package_type = NULL;
static fts_package_t* fts_system_package = NULL;
static fts_package_t* fts_package_stack[PACKAGE_STACK_SIZE];
static int fts_package_stack_top = 0;

static fts_status_description_t fts_DuplicatedClass = {"Duplicated class"};

#if defined(WIN32)
#define fts_lib_prefix   ""
#define fts_lib_postfix  ".dll"
#define fts_other_lib_postfix  ""
#elif defined(__APPLE__) && defined(__MACH__) 
#define fts_lib_prefix   "lib"
#define fts_lib_postfix  ".so"
#define fts_other_lib_postfix  ".bundle"
#else
#define fts_lib_prefix   "lib"
#define fts_lib_postfix  ".so"
#define fts_other_lib_postfix  ""
#endif

static void fts_package_load_default_files(fts_package_t* pkg);
static void fts_package_upload_requires(fts_package_t* pkg);
static void fts_package_upload_template_paths( fts_package_t* pkg);
static void fts_package_upload_data_paths( fts_package_t* pkg);
static void fts_package_upload_help( fts_package_t* pkg);
static void fts_package_upload_templates( fts_package_t* pkg);
static void fts_package_upload_windows( fts_package_t* pkg);
static fts_symbol_t fts_package_make_relative_path( fts_package_t* pkg, fts_symbol_t path);

static fts_symbol_t fts_package_make_relative_path( fts_package_t* pkg, fts_symbol_t file)
{
  fts_symbol_t rel_file;
  int dirlen;

  if( pkg->dir == NULL) return file;
  
  if ( strstr( file, pkg->dir) != NULL)
    {
      dirlen = strlen( pkg->dir) + 1;
      rel_file = fts_new_symbol( file + dirlen);
      return rel_file;
    }  
  else
    return file;
}

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
  s = fts_get_root_directory();
  if (s) 
    {
      /* init system_package dir */
      fts_system_package->dir = fts_new_symbol(s);
      
      snprintf(path, MAXPATHLEN, "%s%c%s", s, fts_file_separator, "packages");
      fts_set_symbol(&a, fts_new_symbol(path));
      fts_package_paths = fts_list_append(fts_package_paths, &a);
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
  char *dir;
  fts_object_t* obj;
  fts_package_t* pkg = NULL;

  /* this is a hack but not a big one: load the patcher in the context
     of the system package. */
  fts_package_push(fts_system_package);

  fts_make_absolute_path(NULL, filename, path, MAXPATHLEN);

  obj = fts_binary_file_load( path, (fts_object_t *)fts_get_root_patcher(), 0, 0);

  if (!obj) {
    fts_log("[package]: Failed to load package file %s\n", path);
    pkg = fts_package_new(name);
    pkg->state = fts_package_corrupt;
    goto gracefull_exit;
  }

  /* check whether it's a package object */
  if (fts_object_get_class(obj) != fts_package_type) {
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
  pkg->filename = fts_new_symbol(filename);

  dir = strcpy( fts_malloc( strlen( path) + 1), path);
  pkg->dir = fts_new_symbol( fts_dirname( dir));

  fts_package_pop(fts_system_package);
  return pkg;
}

fts_package_t*
fts_package_load(fts_symbol_t name)
{
  fts_package_t* pkg = NULL;
  char path[MAXPATHLEN];
  char filename[MAXPATHLEN];
  fts_atom_t n, p;

  /* avoid loading the package twice */
  fts_set_symbol(&n, name);
  if (fts_hashtable_get(&fts_packages, &n, &p)) {
    return fts_get_pointer(&p);
  }

  /* locate the directory of the package */
  if (!fts_file_find_in_path(NULL, fts_get_package_paths(), name, path, MAXPATHLEN) 
      || !fts_is_directory(path)) {
    fts_log("[package]: Couldn't find package %s\n", name);
    pkg = fts_package_new(name);
    pkg->state = fts_package_corrupt;
    goto graceful_exit;
  }
  
  /* load the definition file */
  sprintf(filename, "%s%c%s.jpkg", path, fts_file_separator, name);

  if (fts_file_exists(filename)) {
    pkg = fts_package_load_from_file(name, filename);
    fts_log("[package]: Loaded %s package definition\n", name);
    post("load package: %s\n", name);
  } else {
    pkg = fts_package_new(name);
    pkg->state = fts_package_defined;
    pkg->name = name;
    pkg->filename = fts_new_symbol(filename);
    pkg->dir = fts_new_symbol(path);
  }
  
  /* load the default files */
  fts_package_load_default_files(pkg);

 graceful_exit:

  /* put the package in the hashtable */
  fts_set_symbol(&n, name);
  fts_set_pointer(&p, pkg);
  fts_hashtable_put(&fts_packages, &n, &p);    

  return pkg;
}

fts_package_t* 
fts_package_get(fts_symbol_t name)
{
  fts_package_t* pkg;
  fts_atom_t n, p;

  fts_set_symbol(&n, name);

  if (fts_hashtable_get(&fts_packages, &n, &p)) {
    return fts_get_pointer(&p);
  } else {
    /* try to load it */
    pkg = fts_package_load(name);
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

static void fts_package_load_default_files(fts_package_t* pkg)
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
	  pkg->dir, fts_file_separator, 
	  "c", fts_file_separator, 
	  fts_lib_prefix, pkg->name, fts_lib_postfix);

  if(!fts_file_exists(filename)) {
    sprintf(filename, "%s%c%s%c%s%c%s%c%s%s%s", 
	    pkg->dir, fts_file_separator, 
	    "c", fts_file_separator,
	    "src", fts_file_separator,
	    ".libs", fts_file_separator,
	    fts_lib_prefix, pkg->name, fts_lib_postfix);
  }

  if(!fts_file_exists(filename)) {
    sprintf(filename, "%s%c%s%c%s%s%s", 
	  pkg->dir, fts_file_separator, 
	  "c", fts_file_separator, 
	  fts_lib_prefix, pkg->name, fts_other_lib_postfix);
  }

  if(!fts_file_exists(filename)) {
    sprintf(filename, "%s%c%s%c%s%c%s%c%s%s%s", 
	    pkg->dir, fts_file_separator, 
	    "c", fts_file_separator, 
	    "src", fts_file_separator,
	    ".libs", fts_file_separator,
	  fts_lib_prefix, pkg->name, fts_other_lib_postfix);
  }

  if (fts_file_exists(filename)) {
    snprintf(function, 256, "%s_config", pkg->name);
    ret = fts_load_library(filename, function);
    if (ret != fts_ok) {
      fts_log("[package]: Error loading library of package %s: %s\n", pkg->name, ret->description);
    } else {
      fts_log("[package]: Loaded %s library\n", pkg->name);
    }
  } else {
    fts_log("[package]: Didn't found no library for %s (tried %s)\n", pkg->name, filename);
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

  /* provoke the loading the package */
  fts_package_load(required_pkg);
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

static void fts_package_add_template(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file, int index)
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
      /* Register the template */
      template = fts_template_new(name, NULL, file);

      fts_set_symbol(&n, name);
      fts_set_pointer(&p, template);
      fts_hashtable_put(pkg->declared_templates, &n, &p);

      fts_template_set_package(template, pkg);
      
      if( index >= 0)
	pkg->template_names = fts_list_insert( pkg->template_names, &n, index);
      else    
	pkg->template_names = fts_list_append( pkg->template_names, &n);
    }
}

static void fts_package_remove_template(fts_package_t* pkg, fts_symbol_t name, fts_symbol_t file)
{
  fts_atom_t n;
  fts_set_symbol(&n, name);
  fts_hashtable_remove( pkg->declared_templates, &n);
  pkg->template_names = fts_list_remove( pkg->template_names, &n);
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
  char buf[MAXPATHLEN];

  fts_set_symbol( &k, name);

  if (fts_hashtable_get(pkg->declared_templates, &k, &a)) 
    {
      fts_template_t *template = (fts_template_t *) fts_get_pointer(&a);
      
      if (fts_template_get_filename(template) == NULL) 
	{
	  fts_make_absolute_path(pkg->dir, fts_template_get_original_filename(template), buf, MAXPATHLEN);
	  fts_template_set_filename(template, fts_new_symbol(buf));
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
    return (fts_template_t *) fts_get_pointer(&a);
  else
    {
      char filename[MAXPATHLEN];
      char path[MAXPATHLEN];
      fts_template_t* template;
      fts_atom_t n, p;
      const char* root;
      
      root = (pkg->dir != NULL)? pkg->dir : NULL;
      
      snprintf(filename, MAXPATHLEN, "%s.jmax", name);

      if (!fts_file_find_in_path(root, pkg->template_paths, filename, path, MAXPATHLEN))
	return NULL;
      
      /* Register the template */
      template = fts_template_new(name, fts_new_symbol(path), NULL);
      
      /* Create the database if necessary */
      if (pkg->templates_in_path == NULL) 
	{
	  pkg->templates_in_path = (fts_hashtable_t*) fts_malloc(sizeof(fts_hashtable_t));
	  fts_hashtable_init( pkg->templates_in_path, FTS_HASHTABLE_SMALL);
	}
      
      fts_set_symbol(&n, name);
      fts_set_pointer(&p, template);
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
  
  fts_hashtable_get_values(pkg->declared_templates, &iter);

  while ( fts_iterator_has_more( &iter)) {
    fts_iterator_next( &iter, &tmpl_atom);
    template = fts_get_pointer(&tmpl_atom);
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
    template = fts_get_pointer(&tmpl_atom);
    if (fts_template_get_filename(template) == filename) {
      return template;
    }
  }

  return NULL;
}

/********************************************************************
 *
 *   - classes 
 */

fts_status_t 
fts_package_add_class( fts_package_t* pkg, fts_class_t *cl, fts_symbol_t name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(pkg->classes, &k, &data))
    return &fts_DuplicatedClass;

  fts_set_pointer(&data, cl);
  fts_hashtable_put(pkg->classes, &k, &data);

  if(fts_class_get_package(cl) == NULL)
    fts_class_set_package(cl, pkg);

  return fts_ok;
}

fts_class_t *
fts_package_get_class(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t k, v;
  
  fts_set_symbol( &k, name);
  if ( fts_hashtable_get( pkg->classes, &k, &v))
    return (fts_class_t *)fts_get_object(&v);

  return NULL;
}

void 
fts_package_get_class_names(fts_package_t* pkg, fts_iterator_t* iter)
{
  fts_hashtable_get_keys(pkg->classes, iter);
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
  return fts_file_find_in_path( pkg->dir, pkg->data_paths, filename, buf, len);
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
    fts_hashtable_init( pkg->help, FTS_HASHTABLE_SMALL);
  }
  
  fts_set_symbol(&n, name);
  fts_set_symbol(&p, file);
  fts_hashtable_put(pkg->help, &n, &p);

  pkg->help_classes = fts_list_append(pkg->help_classes, &n);
}

fts_symbol_t
fts_package_get_help(fts_package_t* pkg, fts_symbol_t name)
{
  fts_atom_t data, k;
  
  fts_set_symbol( &k, name);
  
  if ((pkg->help != NULL) && fts_hashtable_get(pkg->help, &k, &data)) {
    return fts_get_symbol(&data);
  } else {
    return NULL;
  }
}

/***********************************************
 *
 *  print and save utilities for methods
 */

static void 
fun_template( fts_atom_t *a)
{
  fts_template_t *template = (fts_template_t *)fts_get_pointer( a);

  fts_set_symbol( a, fts_template_get_original_filename( template));
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
  fts_bmax_code_obj_mess( f, selector, ac);
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
      fts_bmax_code_obj_mess(f, selector, 2);
      fts_bmax_code_pop_args(f, 2);
    }
}

static void 
fts_package_print_list_aux( fts_list_t *list)
{
  if (!list)
    return;

  post( " %s", fts_get_symbol( fts_list_get( list)));

  fts_package_print_list_aux( fts_list_next( list));
}

static void 
fts_package_print_list( fts_list_t *list, fts_symbol_t selector)
{
  post( "  %s", selector);

  fts_package_print_list_aux( list);

  post( "\n");
}

static void 
fts_package_print_hashtable( fts_hashtable_t *ht, fts_symbol_t selector, void (*fun)(fts_atom_t *))
{
  const char *msg = selector;
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

      post( "  %s %s %s\n", msg, fts_get_symbol( a), fts_get_symbol( a+1));
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

  pkg->packages = NULL; 

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      fts_package_require(pkg, fts_get_symbol(&at[i]));
    }
  }
  
  if( fts_object_has_id( o) && pkg->packages)
    fts_package_upload_requires( pkg);

  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_template(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;
  
  for (i = 0; i < ac; i += 2)
    fts_package_add_template(pkg, fts_get_symbol(&at[i]), 
			     fts_package_make_relative_path(pkg, fts_get_symbol(&at[i+1])), 
			     -1);
  
  if( fts_object_has_id( o))
    fts_package_upload_templates( pkg);
  
  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_template_insert(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  fts_package_add_template(pkg, fts_get_symbol(&at[0]), 
			   fts_package_make_relative_path(pkg, fts_get_symbol(&at[1])), 
			   fts_get_int( &at[2]));
  
  if( fts_object_has_id( o))
    fts_package_upload_templates( pkg);
  
  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_template_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;

  fts_package_remove_template(pkg, fts_get_symbol(&at[0]), fts_get_symbol(&at[1]));

  if( fts_object_has_id( o))
    fts_package_upload_templates( pkg);
  
  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_template_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;
  fts_symbol_t path;

  pkg->template_paths = NULL;  

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      path = fts_package_make_relative_path(pkg, fts_get_symbol(&at[i]));
      fts_package_add_template_path(pkg, path);
    }
  }

  if( fts_object_has_id( o) && pkg->template_paths)
    fts_package_upload_template_paths( pkg);

  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_data_path(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;
  fts_symbol_t path;

  pkg->data_paths = NULL;  

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol(&at[i])) {
      path = fts_package_make_relative_path(pkg, fts_get_symbol(&at[i]));
      fts_package_add_data_path(pkg, path);
    }
  }

  if( fts_object_has_id( o) && pkg->data_paths)
    fts_package_upload_data_paths( pkg);

  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_save_windows(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i; 
  fts_atom_t n;
  fts_symbol_t file_name;

  pkg->windows = NULL;  

  for (i = 0; i < ac; i++) {
    if (fts_is_symbol( &at[i])) {
      file_name = fts_package_make_relative_path( pkg, fts_get_symbol(&at[i]));
      fts_set_symbol( &n, file_name);
      pkg->windows = fts_list_append( pkg->windows, &n);
    }
  }
}

static void 
__fts_package_open_windows(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i; 

  pkg->windows = NULL;  

  for (i = 0; i < ac; i++) {
    if ( fts_is_symbol( &at[i]))
      pkg->windows = fts_list_append( pkg->windows, &at[i]);
  }
}

static void 
fts_package_upload_windows( fts_package_t *this)
{
  fts_iterator_t i;
  fts_atom_t a[1];
  fts_symbol_t file_name;
  char buf[MAXPATHLEN];

  fts_list_get_values( this->windows, &i);
  
  while (fts_iterator_has_more( &i))
    {
      fts_iterator_next( &i, a);
      fts_make_absolute_path( this->dir, fts_get_symbol( a), buf, MAXPATHLEN);
      if ( fts_file_exists(buf) && fts_is_file(buf)) 
	{
	  file_name = fts_new_symbol(buf);
	  fts_client_load_patcher( file_name, fts_get_client_id( (fts_object_t *)this));
	} 
    }
}

static void 
__fts_package_help(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  int i;

  pkg->help = NULL;  
  pkg->help_classes = NULL;  

  for (i = 0; i < ac; i += 2)
    fts_package_add_help(pkg, fts_get_symbol(&at[i]), fts_get_symbol(&at[i+1]));

  if( fts_object_has_id( o) && pkg->help)
    fts_package_upload_help( pkg);
  
  fts_package_set_dirty( pkg, 1);
}

static void 
__fts_package_config(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t* pkg = (fts_package_t *)o;
  
  if( ac == 1)
    pkg->config = fts_package_make_relative_path(pkg, fts_get_symbol(&at[0]));  
  else
    pkg->config = NULL;

  if( fts_object_has_id( o))
    {
      fts_client_start_message( o, fts_s_config);
      if( pkg->config != NULL)
	fts_client_add_symbol( o, pkg->config);      
      fts_client_done_message( o);    
    }

  fts_package_set_dirty( pkg, 1);

  if(( pkg->config != NULL) && ( fts_config_get() != NULL))
    {
      fts_atom_t a[1];
      fts_set_symbol( a, pkg->config);
      fts_send_message((fts_object_t *)fts_config_get(), fts_s_load, 1, a);
    }
  else
    fts_send_message((fts_object_t *)fts_config_get(), fts_s_default, 0, 0);
}

static void 
__fts_package_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;
  fts_bmax_file_t f;
  const char *filename;
  char path[MAXPATHLEN];
  char *dir;

  if (ac == 0)
    {
      post( "No filename specified\n");    
      return;
    }

  filename = fts_get_symbol( at);
  
  if (fts_bmax_file_open( &f, filename, 0, 0, 0) < 0)
    {
      post( "Cannot open file %s\n", filename);
      return;
    }

  fts_bmax_code_new_object( &f, o, -1);

  if ( this->packages)
    fts_package_save_list( &f, this->packages, fts_s_require);

  if ( this->template_paths)
    fts_package_save_list( &f, this->template_paths, fts_s_template_path);

  if ( this->data_paths)
    fts_package_save_list( &f, this->data_paths, fts_s_data_path);

  if(( this->declared_templates) &&  fts_hashtable_get_size(this->declared_templates))
    {
      fts_atom_t* a;
      int i = 0;
      fts_iterator_t keys;

#if HAVE_ALLOCA
      a = alloca(( fts_hashtable_get_size( this->declared_templates)*2 + 1) * sizeof(fts_atom_t));
#else
      a = malloc(( fts_hashtable_get_size( this->declared_templates)*2 + 1) * sizeof(fts_atom_t));
#endif
              
      fts_list_get_values( this->template_names, &keys);
      
      while ( fts_iterator_has_more( &keys))
	{
	  fts_iterator_next( &keys, a+i);
	  fts_hashtable_get( this->declared_templates, a+i, a+i+1);
	  fts_set_symbol( a+i+1, fts_template_get_original_filename(((fts_template_t *)fts_get_object( a+i+1))));
	  i+=2;
	}      
      fts_bmax_code_push_atoms(&f, i, a);
      fts_bmax_code_obj_mess(&f, fts_s_template, i);
      fts_bmax_code_pop_args(&f, i);
  
#ifndef HAVE_ALLOCA
      free(a);
#endif    
    }

  if ((this->help ) &&  fts_hashtable_get_size(this->help))
    {
      fts_atom_t* a;
      int i = 0;
      fts_iterator_t keys;

#if HAVE_ALLOCA
      a = alloca(( fts_hashtable_get_size( this->help)*2 + 1) * sizeof(fts_atom_t));
#else
      a = malloc(( fts_hashtable_get_size( this->help)*2 + 1) * sizeof(fts_atom_t));
#endif
              
      fts_list_get_values( this->help_classes, &keys);
      
      while ( fts_iterator_has_more( &keys))
	{
	  fts_iterator_next( &keys, a+i);
	  fts_hashtable_get( this->help, a+i, a+i+1);
	  i+=2;
	}      
      fts_bmax_code_push_atoms(&f, i, a);
      fts_bmax_code_obj_mess(&f, fts_s_help, i);
      fts_bmax_code_pop_args(&f, i);
  
#ifndef HAVE_ALLOCA
      free(a);
#endif    
    }

  if( this->config)
    {
      fts_atom_t a[1];
      fts_set_symbol(a, this->config);
      fts_bmax_code_push_atoms(&f, 1, a);
      fts_bmax_code_obj_mess(&f, fts_s_config, 1);
      fts_bmax_code_pop_args(&f, 1);
    }

  if( this->windows)
    {
      fts_package_save_list( &f, this->windows, fts_s_windows);
    }

  fts_bmax_code_return( &f);

  fts_bmax_file_close( &f);

  fts_package_set_dirty( this, 0);

  if( this->filename == NULL)
    {  
      this->filename = fts_new_symbol(filename);
  
      fts_make_absolute_path(NULL, filename, path, MAXPATHLEN);
      dir = strcpy( fts_malloc( strlen( path) + 1), path);
      this->dir = fts_new_symbol( fts_dirname( dir));
    }
}

static void 
__fts_package_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;

  post( "package %s:\n", fts_package_get_name( this));

  if ( this->packages) {
    fts_package_print_list( this->packages, fts_s_require);
  }
  if ( this->template_paths) {
    fts_package_print_list( this->template_paths, fts_s_template_path);
  }

  if ( this->data_paths) {
    fts_package_print_list( this->data_paths, fts_s_data_path);
  }

  if ( this->declared_templates) {
    fts_package_print_hashtable( this->declared_templates, fts_s_template, fun_template);
  }

  if ( this->help) {
    fts_package_print_hashtable( this->help, fts_s_help, NULL);
  }
}

/* set a package as dirty or as saved.
 * A "setDirty" message is sent to the client after is_dirty flag changed
 */
void 
fts_package_set_dirty(fts_package_t *this, int is_dirty)
{
  if(this->dirty != is_dirty)
    {
      this->dirty = is_dirty;

      if ( fts_object_has_id( (fts_object_t *)this))
	{
	  fts_atom_t a[1];
	  
	  fts_set_int(&a[0], is_dirty);
	  fts_client_send_message((fts_object_t *)this, fts_s_set_dirty, 1, a);
	}
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
  fts_atom_t a;

  if ((ac > 0) && fts_is_symbol(at)) { 
    pkg->name = fts_get_symbol(at);
  } else {
    pkg->name = NULL;
  }
  pkg->dir = NULL;
  pkg->filename = NULL;
  pkg->state = fts_package_defined;
  pkg->error = NULL;
  pkg->patcher = NULL;
  pkg->packages = NULL;

  pkg->classes = fts_hashtable_new( FTS_HASHTABLE_SMALL);
  pkg->declared_templates = fts_hashtable_new( FTS_HASHTABLE_SMALL);

  pkg->template_names = NULL;
  pkg->templates_in_path = NULL;
  pkg->template_paths = NULL;

  pkg->help = NULL;
  pkg->help_classes = NULL;

  pkg->data_paths = NULL;

  pkg->windows = NULL;

  pkg->config = NULL;

  fts_set_symbol(&a, fts_s_package);
  fts_object_set_description(o, 1, &a);
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
  if (pkg->windows != NULL) {
    fts_list_delete(pkg->windows);
  }

  fts_hashtable_destroy(pkg->classes);

  fts_hashtable_destroy(pkg->declared_templates);
  fts_list_delete(pkg->template_names);

  if (pkg->templates_in_path != NULL) {
    fts_hashtable_destroy(pkg->templates_in_path);
  }
  if (pkg->template_paths != NULL) {
    fts_list_delete(pkg->template_paths);
  }

  if (pkg->help != NULL) {
    fts_hashtable_destroy(pkg->help);
    fts_list_delete(pkg->help_classes);
  }
  if (pkg->patcher != NULL) {
    fts_object_destroy(pkg->patcher);
  }
}

static void 
fts_package_send_list( fts_object_t *o, fts_iterator_t i, fts_symbol_t selector)
{
  fts_atom_t a[1];

  fts_client_start_message( o, selector);
  while (fts_iterator_has_more( &i))
    {
      fts_iterator_next( &i, a);
      fts_client_add_symbol( o, fts_get_symbol( a));      
    } 
  fts_client_done_message( o);
}

static void 
fts_package_upload_requires( fts_package_t *this)
{
  fts_package_t *pkg;
  fts_atom_t a[1]; 
  fts_iterator_t i;
  fts_list_get_values( this->packages, &i);
  fts_client_start_message( (fts_object_t *)this, fts_s_require);
  while (fts_iterator_has_more( &i))
    {
      fts_iterator_next( &i, a);
      fts_client_add_symbol( (fts_object_t *)this, fts_get_symbol( a));      
      pkg = fts_package_get( fts_get_symbol( a));
      if (!fts_object_has_id( (fts_object_t *)pkg))
	fts_client_register_object( (fts_object_t *)pkg, fts_get_client_id( (fts_object_t *)this));
      fts_client_add_int( (fts_object_t *)this, fts_get_object_id( (fts_object_t *)pkg));      
    } 
  fts_client_done_message( (fts_object_t *)this);    
}

static void 
fts_package_upload_template_paths( fts_package_t *this)
{
  fts_iterator_t i;
  fts_list_get_values( this->template_paths, &i);
  fts_package_send_list( (fts_object_t *)this, i, fts_s_template_path);
}

static void 
fts_package_upload_data_paths( fts_package_t *this)
{
  fts_iterator_t i;
  fts_list_get_values( this->data_paths, &i);
  fts_package_send_list( (fts_object_t *)this, i, fts_s_data_path);
}

static void 
fts_package_upload_help( fts_package_t *this)
{
  fts_atom_t a[2]; 
  int ok = 0;
  fts_iterator_t i;
  fts_list_get_values( this->help_classes, &i);
  
  fts_client_start_message( (fts_object_t *)this, fts_s_help);
  
  while ( fts_iterator_has_more( &i))
    {
      fts_iterator_next( &i, a);
      ok = fts_hashtable_get( this->help, a, a+1);
      if( ok)
	{
	  fts_client_add_symbol( (fts_object_t *)this, fts_get_symbol( a));   
	  fts_client_add_symbol( (fts_object_t *)this, fts_get_symbol( a+1));   
	}
    } 
  
  fts_client_done_message( (fts_object_t *)this);    
}

static void 
fts_package_upload_templates( fts_package_t *this)
{
  fts_template_t *tmpl;
  fts_atom_t a[2]; 
  int ok = 0;
  fts_iterator_t i;
  fts_list_get_values( this->template_names, &i);
  
  fts_client_start_message( (fts_object_t *)this, fts_s_template);
  
  while ( fts_iterator_has_more( &i))
    {
      fts_iterator_next( &i, a);
      ok = fts_hashtable_get( this->declared_templates, a, a+1);
      if( ok)
	{
	  tmpl = (fts_template_t *)fts_get_object( a+1);
	  fts_client_add_symbol( (fts_object_t *)this, fts_get_symbol( a));   
	  fts_client_add_symbol( (fts_object_t *)this, fts_template_get_original_filename( tmpl));   
	}
    } 
  
  fts_client_done_message( (fts_object_t *)this);    
}

static void 
__fts_package_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;
  fts_atom_t a[3]; 
  fts_iterator_t h;
  char summary[256];

  if( this->state == fts_package_corrupt)
    return;

  if ( this->packages)
    fts_package_upload_requires( this);

  if ( this->template_paths)
    fts_package_upload_template_paths( this);

  if ( this->data_paths)
    fts_package_upload_data_paths( this);

  if ( this->help)
    fts_package_upload_help( this);

  if ( this->declared_templates)
    fts_package_upload_templates( this);

  if( this->config)
    if( fts_object_has_id( o))
      {
	fts_client_start_message( o, fts_s_config);
	fts_client_add_symbol( o, this->config);      
	fts_client_done_message( o);    
      }
  
  fts_set_symbol(a, this->name);
  fts_set_symbol(a+1, this->dir);
  fts_set_symbol(a+2, this->filename);
  fts_client_send_message( o, s_setNames, 3, a);  
  
  /*test if summary help patch exists */
  snprintf(summary, 256, "%s%c%s%c%s%s", this->dir, fts_file_separator, "help", 
	   fts_file_separator, this->name, ".summary.jmax");
  fts_set_int(a, fts_file_exists( summary));
  fts_client_send_message( o, s_hasSummary, 1, a);  

  if ( this->windows)
    fts_package_upload_windows( this);

  fts_client_send_message( o, s_uploadDone, 0, 0);  

  fts_package_set_dirty( this, 0);
}

static void 
__fts_package_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  __fts_package_upload( o, winlet, s, 0, 0);
  fts_client_send_message( o, s_updateDone, 0, 0);  
}

static void
__fts_package_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
__fts_package_set_as_current_project(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *this = (fts_package_t *)o;
  fts_project_set( this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
fts_package_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_package_t), __fts_package_init, __fts_package_delete);

  fts_class_message_varargs(cl, fts_s_upload, __fts_package_upload);
  fts_class_message_varargs(cl, fts_s_update, __fts_package_update);
  fts_class_message_varargs(cl, fts_s_print, __fts_package_print);

  fts_class_message_varargs(cl, fts_s_require, __fts_package_require);
  fts_class_message_varargs(cl, fts_s_template, __fts_package_template);
  fts_class_message_varargs(cl, fts_new_symbol("insert_template"), __fts_package_template_insert);
  fts_class_message_varargs(cl, fts_new_symbol("remove_template"), __fts_package_template_remove);
  fts_class_message_varargs(cl, fts_s_template_path, __fts_package_template_path);
  fts_class_message_varargs(cl, fts_s_data_path, __fts_package_data_path);
  fts_class_message_varargs(cl, fts_s_help, __fts_package_help);
  fts_class_message_varargs(cl, fts_s_save, __fts_package_save);
  fts_class_message_varargs(cl, fts_new_symbol("save_windows"), __fts_package_save_windows);
  fts_class_message_varargs(cl, fts_s_windows, __fts_package_open_windows);
  fts_class_message_varargs(cl, fts_s_openEditor, __fts_package_open_editor);
  fts_class_message_varargs(cl, fts_new_symbol("set_as_current_project"), __fts_package_set_as_current_project);
  fts_class_message_varargs(cl, fts_s_config, __fts_package_config);
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
  return (fts_package_t *) fts_object_create(fts_package_type, NULL, 1, a);
}

void 
fts_package_delete(fts_package_t* pkg)
{
  fts_client_send_message( (fts_object_t *)pkg, fts_s_destroyEditor, 0, 0);

  if( ((fts_object_t *)pkg)->patcher)
    {
      fts_patcher_remove_object( ((fts_object_t *)pkg)->patcher, (fts_object_t *)pkg);
      ((fts_object_t *)pkg)->patcher = 0;
    }
  else
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

  /* Load the .jmax file */
  obj = fts_binary_file_load( fts_get_symbol( at), (fts_object_t *)fts_get_root_patcher(), 0, a);

  if (!obj)
    {
      post( "Load failed\n");
      return;
    }

  /* Send a "print" message to the result */
  fts_send_message( obj, fts_s_print, 0, 0);

  if (fts_object_get_class(obj) == fts_package_type) {
    post( "Loaded package\n");
  }
}

static void loader_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0);

  fts_class_message_varargs(cl, fts_new_symbol( "load"), loader_load);
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

  s_setNames = fts_new_symbol("setNames");
  s_hasSummary = fts_new_symbol("hasSummaryHelp");
  s_updateDone = fts_new_symbol("updateDone");
  s_uploadDone = fts_new_symbol("uploadDone");

  for (i = 0; i < PACKAGE_STACK_SIZE; i++) {
    fts_package_stack[i] = NULL;
  }

  fts_hashtable_init( &fts_packages, FTS_HASHTABLE_MEDIUM);

  /* create the system package */
  system_symbol = fts_new_symbol("_system_");
  fts_s_package = fts_new_symbol("package");


  /* since we need a class to construct a package and a package to
     find a class, we have to do some non standard hacking to
     bootstrap the system */
  fts_system_package = fts_zalloc(sizeof(fts_package_t));

  fts_system_package->object.cl = 0;
  fts_system_package->object.client_id = FTS_NO_ID;
  fts_system_package->object.properties = 0;
  fts_system_package->object.refcnt = 0;
  fts_system_package->object.out_conn = 0;
  fts_system_package->object.in_conn = (fts_connection_t **) fts_zalloc(sizeof(fts_connection_t *));
  
  __fts_package_init((fts_object_t*) fts_system_package, 0, fts_s_init, 0, 0); 
  fts_package_set_state(fts_system_package, fts_package_loaded); 

  fts_set_symbol(&a, fts_new_symbol("builtin"));
  fts_set_pointer(&p, fts_system_package);
  fts_hashtable_put(&fts_packages, &a, &p);

  fts_package_push(fts_system_package);

  /* now that there's a system package, we can define the package class */
  fts_package_type = fts_class_install(fts_s_package, fts_package_instantiate);

  /* update the system package with the correct class */
  fts_system_package->object.cl = fts_package_type;

  /* Debug code */
  fts_class_install( fts_new_symbol( "loader"), loader_instantiate);
}

