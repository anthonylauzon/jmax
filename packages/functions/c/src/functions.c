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

#include <math.h>
#include <string.h>
#include <fts/fts.h>
#include <fts/packages/data/data.h>

#ifdef WIN32
#if defined(FUNCTIONS_EXPORTS)
#define FUNCTIONS_API __declspec(dllexport)
#else
#define FUNCTIONS_API __declspec(dllimport)
#endif
#else
#define FUNCTIONS_API extern
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif

FUNCTIONS_API void
functions_config(void);


static fts_status_description_t wrong_args_error_description = {"wrong arguments"};
static fts_status_t wrong_args_error = &wrong_args_error_description;

static fts_status_description_t no_args_error_description = {"argument(s) required"};
static fts_status_t no_args_error = &no_args_error_description;

/**********************************************************************
*
*  wrappers to C math functions with one argument are automatically generated
*
*/

#define DEFINE_FUN(FUN) \
static fts_status_t FUN##_function( int ac, const fts_atom_t *at, fts_atom_t *ret) \
{ \
  if (ac == 1 && fts_is_number( at)) \
    fts_set_float( ret, FUN( fts_get_number_float( at))); \
      return fts_ok;  \
}

#define INSTALL_FUN(FUN) fts_function_install( fts_new_symbol( #FUN), FUN##_function);

#define FUN DEFINE_FUN
#include "mathfuns.h"



/**********************************************************************
*
*  misc math functions with name change or multiple arguments
*
*/

static fts_status_t 
abs_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0 && fts_is_number( at))
    fts_set_float(ret, fabs(fts_get_number_float( at)));
  
  return fts_ok;
}

static fts_status_t 
pow_function (int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 1  &&  fts_is_number(at)  &&  fts_is_number(at+1))
    fts_set_float(ret, pow(fts_get_number_float(at), 
                           fts_get_number_float(at+1)));
  
  return fts_ok;
}

/**  modulo function according to fmod
*   @fn float mod (float a, float b) 
*/

static fts_status_t 
mod_function (int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 1  &&  fts_is_number(at)  &&  fts_is_number(at+1))
    fts_set_float(ret, fmod(fts_get_number_float(at), 
                            fts_get_number_float(at+1)));
  
  return fts_ok;
}


static fts_status_t
_function_convert_int(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0)
  {
    if(fts_is_int(at))
      *ret = at[0];
    else if(fts_is_float(at))
      fts_set_int(ret, (int)fts_get_float(at));
    else
      fts_set_int(ret, 0);
  }
  else
    fts_set_int(ret, 0);
  
  return fts_ok;
}

static fts_status_t
_function_convert_float(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0)
  {
    if(fts_is_int(at))
      fts_set_float(ret, (double)fts_get_int(at));
    else if(fts_is_float(at))
      *ret = at[0];
    else
      fts_set_float(ret, 0.0);
  }
  else
    fts_set_float(ret, 0.0);
  
  return fts_ok;
}

static fts_status_t
_function_floor(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0)
  {
    if(fts_is_int(at))
      *ret = at[0];
    else if(fts_is_float(at))
      fts_set_float(ret, floor(fts_get_float(at)));
    else
      return wrong_args_error;
  }
  else
    return no_args_error;
  
  return fts_ok;
}

static fts_status_t
_function_ceil(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0)
  {
    if(fts_is_int(at))
      *ret = at[0];
    else if(fts_is_float(at))
      fts_set_float(ret, ceil(fts_get_float(at)));
    else
      return wrong_args_error;
  }
  else
    return no_args_error;
  
  return fts_ok;
}

static fts_status_t
_function_min(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_number(at))
  {
    double min = fts_get_number_float(at);
    int i;
    
    *ret = at[0];
    
    for(i=1; i<ac; i++)
    {
      if(fts_is_number(at + i))
      {
        double f = fts_get_number_float(at + i);
        
        if(f < min)
        {
          min = f;
          *ret = at[i];
        }
      }
    }
    
    return fts_ok;
  }
  
  return wrong_args_error;
}

static fts_status_t
_function_max(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_number(at))
  {
    double max = fts_get_number_float(at);
    int i;
    
    *ret = at[0];
    
    for(i=1; i<ac; i++)
    {
      if(fts_is_number(at + i))
      {
        double f = fts_get_number_float(at + i);
        
        if(f > max)
        {
          max = f;
          *ret = at[i];
        }
      }
    }
    
    return fts_ok;
  }
  
  return wrong_args_error;
}

/**********************************************************************
*
*  random function
*
*/

#define RA 16807 /* multiplier */
#define RM 2147483647L /* 2**31 - 1 */
#define RQ 127773L /* m div a */
#define RR 2836 /* m mod a */

static unsigned int seed = 1;

static void
random_init(void)
{
#ifdef HAVE_SYS_TIME_H
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed = tv.tv_usec;
#else
  seed = time( NULL);
#endif		
}

static fts_status_t 
random_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  double min = 0.0;
  double max = 1.0;
  
  unsigned int hi;
  
  hi = RA * (long)((unsigned int)seed >> 16);
  seed = RA * (long)(seed & 0xFFFF);
  
  seed += (hi & 0x7FFF) << 16;
  
  if (seed > RM)
  {
    seed &= RM;
    ++seed;
  }
  
  seed += hi >> 15;
  
  if (seed > RM)
  {
    seed &= RM;
    ++seed;
  }
  
  if(ac > 0 && fts_is_number(at))
  {
    if(ac > 1 && fts_is_number(at + 1))
    {
      min = fts_get_number_float(at);
      max = fts_get_number_float(at + 1);
    }
    else
      max = fts_get_number_float(at);
  }
  
  fts_set_float(ret, min + (max - min) * ((double)seed / (double)RM));
  
  return fts_ok;
}



/**********************************************************************
*
*  symbol functions
*
*/

#define MAX_CONCAT_LENGTH 512

static fts_status_t
cat_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  char buf[MAX_CONCAT_LENGTH + 1];
  int n = 0;
  int i;
  
  buf[0] = '\0';
  buf[MAX_CONCAT_LENGTH] = '\0';
  
  for(i=0; i<ac && n<MAX_CONCAT_LENGTH; i++)
  {
    if(fts_is_int(at + i))
      snprintf(buf + n, MAX_CONCAT_LENGTH, "%d", (int)fts_get_int(at + i));
    else if(fts_is_float(at + i))
      snprintf(buf + n, MAX_CONCAT_LENGTH, "%g", fts_get_float(at + i));
    else if(fts_is_symbol(at + i))
      strncpy(buf + n, fts_symbol_name(fts_get_symbol(at + i)), MAX_CONCAT_LENGTH - n);
    else
      continue;
    
    n = strlen(buf);
  }
  
  fts_set_symbol(ret, fts_new_symbol(buf));
  
  return fts_ok;
}

/**********************************************************************
*
*  conditional functions
*
*/

/* (if cond argtrue argfalse) */

static fts_status_t
if_function (int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0)
  {
    double cond = 0.0;
    
    if (fts_is_number(at))
	    cond = fts_get_number_float(at);
    else if (!fts_is_void(at))
	    cond = 1.0;
    
    if (cond != 0.0)
    {   /* true */
	    if (ac > 1)
        fts_atom_assign(ret, at + 1);
	    /* else: nothing */
    }
    else
    {   /* false */
	    if (ac > 2)
        fts_atom_assign(ret, at + 2);
	    /* else: really nothing */
    }

return fts_ok;
  }
else
return fts_ignore;	
}


/* (case cond arg1 ... argn) is 1-based, cond = 0 meaning that nothing is output.

Note that this is not like a real case statment, where only one of
several cases is executed, but an expression where all arguments are
evaluated before one of them is chosen!
*/

static fts_status_t
case_function (int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0)
  {
    int cond = 0;	/* default: no output */
    
    if (fts_is_number(at))
	    cond = fts_get_number_int(at);
    
    if (0 < cond  &&  cond < ac)
	    fts_atom_assign(ret, at + cond);
    /* else: nothing */
    
    return fts_ok;
  }
  else
    return fts_ignore;	
}


/**********************************************************************
 *
 *  system functions
 *
 */
static fts_status_t
typeof_function (int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0)
  {
    fts_set_symbol(ret, fts_get_class_name(at));
    return fts_ok;  
  }
  else
  {
    fts_set_void(ret);
    return fts_status_new(fts_new_symbol("missing argument"));
  }
}

static fts_status_t
_function_print(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0)
    fts_sprint_atoms(fts_get_default_console_stream(), ac, at);
  else
    fts_spost(fts_get_default_console_stream(), "\n");
  
  return fts_ok;
}

/******************************************************************************
 *
 *  doc & info function
 *
 */
static void
info_class(fts_class_t *cl, mat_t *outmat)
{
  if(cl != NULL)
  {
    /* post class info */
    if (!cl->size)
      fts_class_instantiate(cl);
    
    if (!outmat)
      fts_class_doc_post(cl);
    else
    {
      fts_array_t outarr;
      int numat, numcol;
      
      fts_array_init(&outarr, 0, NULL);
      numcol = fts_class_doc_get(cl, &outarr);
      numat  = fts_array_get_size(&outarr);
      
      mat_set_size(outmat, numat / numcol, numcol);
      mat_set_with_onset_from_atoms(outmat, 0, numat, fts_array_get_atoms(&outarr));
      fts_array_destroy(&outarr);
    }
  }
  else
  {
    /* post classes */
    fts_package_t *pkg = fts_get_system_package();
    fts_iterator_t cl_it;
    int i = 0;
    
    if (!outmat)
      fts_post("FTS classes:\n");
    
    fts_package_get_classes(pkg, &cl_it);
    
    while(fts_iterator_has_more(&cl_it))
    {
      fts_symbol_t cl_name;
      fts_class_t *cl;
      fts_class_doc_t *doc;
      fts_atom_t a;
      
      fts_iterator_next(&cl_it, &a);
      
      cl_name = fts_get_symbol(&a);
      cl = fts_package_get_class(pkg, cl_name);
      
      if (!cl->size)
        fts_class_instantiate(cl);
      
      doc = fts_class_get_doc(cl);
      
      if(doc != NULL)
      {
        fts_symbol_t name = fts_class_doc_get_name(doc);
        const char *args = fts_class_doc_get_args(doc);
        const char *comment = fts_class_doc_get_comment(doc);
        
        if(args == NULL)
          args = "";
        
        if(name == cl_name)
        {
          if (!outmat)
            fts_post("  %s ... %s\n", fts_symbol_name(name), comment);
          else
          {
#            define     num_doc_col_class      3
            fts_atom_t docat[num_doc_col_class];
            
            fts_set_symbol(&docat[0], name);
            fts_set_symbol(&docat[1], fts_new_symbol(args));
            fts_set_symbol(&docat[2], fts_new_symbol(comment));
            
            mat_set_size(outmat, i+1, num_doc_col_class);
            mat_set_with_onset_from_atoms(outmat, i * num_doc_col_class, 
                                          num_doc_col_class, docat);
            i++;
          }
        }
      }
    }
  }
}

static void
informal_class(fts_class_t *cl)
{
  fts_symbol_t clname = fts_class_get_name(cl);
  fts_iterator_t iter;  
  
  fts_post("complete message list of %s\n", fts_symbol_name(clname));
  
  fts_class_get_messages(cl, &iter);
  
  while(fts_iterator_has_more(&iter))
  {
    fts_atom_t a;
    
    fts_iterator_next(&iter, &a);
    
    if(fts_is_symbol(&a))
    {
      fts_symbol_t sel = fts_get_symbol(&a);
      fts_class_doc_t *doc = fts_class_get_doc(cl);
      int has_doc = 0;
      
      while(doc != NULL && !has_doc)
      {
        fts_symbol_t s = fts_class_doc_get_name(doc);
        
        if(s == sel)
          has_doc = 1;
        
        doc = fts_class_doc_get_next(doc);
      }
      
      fts_post("  %s %s", fts_symbol_name(sel), has_doc? "": "... hidden\n");
    }
    else
      fts_post("  inlet %d\n", fts_get_int(&a));
  }
}

static void
info_functions(mat_t *outmat)
{
  /* post classes */
  fts_package_t *pkg = fts_get_system_package();
  fts_iterator_t fun_it;
  int i = 0;
  
  if (!outmat)
    fts_post("FTS functions:\n");
  
  fts_package_get_functions(pkg, &fun_it);
  
  while(fts_iterator_has_more(&fun_it))
  {
    fts_symbol_t fun_name;
    fts_atom_t a;
    
    fts_iterator_next(&fun_it, &a);
    fun_name = fts_get_symbol(&a);    
    
    
    if (!outmat)
      fts_post("  %s\n", fts_symbol_name(fun_name));
    else
    {
      fts_atom_t docat[2];
      
      fts_set_symbol(&docat[0], fun_name);
      fts_set_symbol(&docat[1], fts_s_empty_string);      /* todo: doc */
      mat_set_size(outmat, i+1, 2);
      mat_set_with_onset_from_atoms(outmat, i * 2, 2, docat);
      i++;
    }
  }
}

/* implementation of "info" function in messagebox.
todo: set output matrix column names */
static fts_status_t
_function_info(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t what = fts_get_symbol(at);
    mat_t *outmat = NULL;
    
    /* check if last arg is mat, used to return list of doc */
    if (ac > 1  &&  fts_is_a(&at[ac - 1], mat_type))
      outmat = (mat_t *) fts_get_object(&at[ac - 1]);
    
    if (what == fts_new_symbol("classes"))
      info_class(NULL, outmat);
    else if (what == fts_new_symbol("class"))
    {
      if(ac > 1 && fts_is_symbol(at + 1))
      {
        fts_symbol_t class_name = fts_get_symbol(at + 1);
        fts_class_t *cl = fts_get_class_by_name(class_name);
        
        if(cl != NULL)
          info_class(cl, outmat);
        else
          fts_post("info: unknown class '%s'", class_name);        
      }
    }
    else if (what == fts_new_symbol("rmal"))
    {
      if(ac > 1 && fts_is_symbol(at + 1))
      {
        fts_symbol_t class_name = fts_get_symbol(at + 1);
        fts_class_t *cl = fts_get_class_by_name(class_name);
        
        if(cl != NULL)
          informal_class(cl);
        else
          fts_post("info: unknown class '%s'", class_name);        
      }
    }
    else if (what == fts_new_symbol("functions"))
      info_functions(outmat);
    else
      fts_post("no info for '%s'\n", fts_symbol_name(what));
    
    /* return matrix, update editor if given */
    if (outmat)
    {
      fts_set_object(ret, (fts_object_t *) outmat);
      fts_object_set_state_dirty((fts_object_t *) outmat);
      
      if(mat_editor_is_open(outmat))
        mat_upload(outmat);
    }
  }
  
  return fts_ok;
}

/**********************************************************************
 *
 *  functions setup
 *
 */
void
functions_config(void)
{
#undef FUN
#define FUN INSTALL_FUN
#include "mathfuns.h"
  random_init();
  
  fts_function_install(fts_new_symbol( "int"), _function_convert_int);
  fts_function_install(fts_new_symbol( "i"), _function_convert_int);
  fts_function_install(fts_new_symbol( "float"), _function_convert_float);
  fts_function_install(fts_new_symbol( "f"), _function_convert_float);

  fts_function_install( fts_new_symbol("abs"), abs_function);
  fts_function_install( fts_new_symbol("pow"), pow_function);
  fts_function_install( fts_new_symbol("random"), random_function);
  fts_function_install( fts_new_symbol("mod"), mod_function);
  fts_function_install(fts_new_symbol( "floor"), _function_floor);
  fts_function_install(fts_new_symbol( "ceil"), _function_ceil);
  fts_function_install(fts_new_symbol( "min"), _function_min);
  fts_function_install(fts_new_symbol( "max"), _function_max);
  
  fts_function_install(fts_new_symbol("cat"), cat_function);
  fts_function_install(fts_new_symbol("if"), if_function);
  fts_function_install(fts_new_symbol("case"), case_function);
  
  fts_function_install(fts_new_symbol("typeof"), typeof_function);
  fts_function_install(fts_new_symbol( "print"), _function_print);
  fts_function_install(fts_new_symbol( "info"), _function_info);
}
