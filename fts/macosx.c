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

/*
 * This file contains Mac OS X platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h> 
#include <sys/time.h> 
#include <mach-o/dyld.h> 

#include <ftsprivate/fpe.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <fts/thread.h>

/***********************************************************************
 *
 * Root directory
 *
 */

fts_symbol_t fts_get_default_root_directory( void)
{
  return fts_new_symbol( DEFAULT_ROOT);
}

fts_symbol_t fts_get_user_project( int create)
{
  char* home;
  char path[MAXPATHLEN];

  home = getenv("HOME");
  fts_make_absolute_path(home, ".jmax.jprj", path, MAXPATHLEN);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;
}

fts_symbol_t 
fts_get_system_project( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( fts_get_root_directory(), fts_s_default_project, path, MAXPATHLEN);
  
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;  
}

fts_symbol_t 
fts_get_user_configuration( int create)
{
  char* home;
  char path[MAXPATHLEN];

  home = getenv("HOME");
  /* @@@@@ Change default configuration file name here @@@@@ */
  fts_make_absolute_path(home, ".jmax.jcfg", path, MAXPATHLEN);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;  
}

fts_symbol_t 
fts_get_system_configuration(void)
{
  char path[MAXPATHLEN];
  fts_make_absolute_path(fts_get_root_directory(), fts_s_default_config, path, MAXPATHLEN);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;  
}

/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

/* The MacOS-X implementation uses undocumented NEXTSTEP rld()...

/* 
#define odbc_private_extern __private_extern__ 
*/ 
#define odbc_private_extern 

static char error_description[1024];
static fts_status_description_t load_library_error = { &error_description[0]};

odbc_private_extern void undefined_symbol_handler( const char *symbolName) 
{
  fts_log( "undefined symbol: %s)", symbolName);
  sprintf( error_description, "undefined symbol: %s", symbolName);
} 

odbc_private_extern NSModule multiple_symbol_handler( NSSymbol s, NSModule old, NSModule new) 
{ 
  /* 
     * Since we can't unload symbols, we're going to run into this 
     * every time we reload a module. Workaround here is to just 
     * rebind to the new symbol, and forget about the old one. 
     * This is crummy, because it's basically a memory leak. 
     * (See Radar 2262020 against dyld). 
     */ 

  fts_log( "multiply defined symbol: %s (old %s new %s)", NSNameOfSymbol(s), NSNameOfModule(old), NSNameOfModule(new)); 

  sprintf( error_description, "multiply defined symbol: %s (old %s new %s)", NSNameOfSymbol(s), NSNameOfModule(old), NSNameOfModule(new)); 

  return new; 
} 

odbc_private_extern void linkEdit_symbol_handler (NSLinkEditErrors c, int errorNumber, const char *fileName, const char *errorString) 
{ 
  fts_log( "errors during link edit for file %s : %s", fileName, errorString); 

  sprintf( error_description, "errors during link edit for file %s : %s", fileName, errorString); 
} 

fts_status_t fts_load_library( const char *filename, const char *symbol)
{
  static int dl_init = 0;
  char *full_sym_name;
  char *tmp;
  char macosx_filename[MAXPATHLEN];

  NSSymbol s; 
  NSObjectFileImage image; 
  NSModule module;
  NSObjectFileImageReturnCode ret;
  void(*fun)(void);

  if ( !dl_init)
    {
      NSLinkEditErrorHandlers handlers; 

      handlers.undefined = undefined_symbol_handler; 
      handlers.multiple = multiple_symbol_handler; 
      handlers.linkEdit = linkEdit_symbol_handler; 

      NSInstallLinkEditErrorHandlers( &handlers);

      dl_init = 1;
    }

    if( strstr(filename, ".so"))
        snprintf(macosx_filename, MAXPATHLEN, "%s", filename);
    else
    {
        snprintf(macosx_filename, MAXPATHLEN, "%s%cContents%cMacOS", filename, fts_file_separator, fts_file_separator);
        tmp = strrchr(filename, fts_file_separator);
        strncat( macosx_filename, tmp, strlen(tmp) - 7);
    }
    
  if ( (ret = NSCreateObjectFileImageFromFile( macosx_filename, &image)) != NSObjectFileImageSuccess )
    return &load_library_error;

  module = NSLinkModule( image, macosx_filename, NSLINKMODULE_OPTION_BINDNOW | NSLINKMODULE_OPTION_RETURN_ON_ERROR);

  if ( !module)
    {
      NSLinkEditErrors errors;
      int error_number;
      const char *filename;
      const char *error_string;

      NSLinkEditError( &errors, &error_number, &filename, &error_string);
      fts_log( "%d %s %s\n", error_number, filename, error_string);
      
      return &load_library_error;
    }

  full_sym_name = (char *)alloca( strlen( symbol) + 2);
  strcpy( full_sym_name, "_");
  strcat( full_sym_name, symbol);

  s = NSLookupSymbolInModule( module, full_sym_name); 

  fun = (void (*)(void))NSAddressOfSymbol( s);

  if ( !fun)
    {
      sprintf( error_description, "symbol %s not found", full_sym_name);
      return &load_library_error;
    }

  (*fun)();

  return fts_ok;
}

/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

void fts_enable_fpe_traps( void)
{
}

void fts_disable_fpe_traps( void)
{
}

unsigned int fts_check_fpe( void)
{
  return 0;
}

/* *************************************************************************** */
/*                                                                             */
/* Memory locking                                                              */
/*                                                                             */
/* *************************************************************************** */

int fts_unlock_memory( void)
{
  return 0;
}


/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

void fts_platform_init( void)
{
  post( "Mac OS X BETA version\n");
  /* load macosx package */
  fts_package_load(fts_new_symbol("macosx"));
}


/* *************************************************************************** */
/*                                                                             */
/* System time                                                                   */
/*                                                                             */
/* *************************************************************************** */

double fts_systime()
{
  struct timeval now;

  gettimeofday(&now, NULL);
  return (double) now.tv_sec * 1000.0 + (double) now.tv_usec / 1000.0;
}

/* ************************************************** */
/*                                                    */
/* Thread manager platform dependent function         */
/*                                                    */
/* ************************************************** */
int thread_manager_start(thread_manager_t* self)
{
    int success;
    /* Time to create the thread */
    success = pthread_create(&self->thread_manager_ID,
			     NULL,
			     thread_manager_main,
			     (void*)self);

    return success;
}

void* thread_manager_run_thread(void* arg)
{
    fts_thread_function_t* thread_func = (fts_thread_function_t*)arg;
    fts_object_t* object = thread_func->object;
    fts_method_t method = thread_func->method;
    fts_symbol_t symbol = thread_func->symbol;
    int ac = thread_func->ac;
    fts_atom_t* at = thread_func->at;
    int delay_ms = thread_func->delay_ms;
    struct timespec time_req;
    struct timespec time_rem;
    time_req.tv_sec = 0;
    time_req.tv_nsec = delay_ms * 1000 * 1000;

    while(1)
    {
	if (0 == thread_func->is_dead)
	{
	    method(object, fts_system_inlet, symbol, ac, at);
	}
	nanosleep(&time_req, &time_rem);
    }

    return 0;
}


/** 
 * Main function of the thread manager
 * 
 * @param arg 
 * 
 * @return 
 */
void* thread_manager_main(void* arg)
{
    int work_done;
    int success;
    thread_manager_t* manager = (thread_manager_t*)arg;

    while(1)
    {
	work_done = 0;
	/* Is there some data in create FIFO */
	if (fts_fifo_read_level(&manager->create_fifo) >= sizeof(fts_atom_t))
	{
	    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->create_fifo);	    
	    fts_thread_worker_t* worker;

	    fts_fifo_incr_read(&manager->create_fifo, sizeof(fts_atom_t));

	    worker = (fts_thread_worker_t*)fts_get_pointer(atom);
	    
	    success = pthread_create((pthread_t *)&worker->id,
				     NULL,
				     thread_manager_run_thread,
				     (void*)worker->thread_function);
	    if (0 != success)
	    {
		post("[thread_manager] cannot start a new thread \n");
	    }
	    else
	    {
		/* try to detach thread */
		success = pthread_detach(worker->id);
		if (0 != success)
		{
		    post("[thread manager] cannot detach thread %d \n", worker->id);
		}
		work_done++;

	    }

	}
	/* Is there some data in cancel FIFO */
	if (fts_fifo_read_level(&manager->cancel_fifo) >= sizeof(fts_atom_t))
	{
	    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->cancel_fifo);	    
	    fts_thread_worker_t* worker;

	    fts_fifo_incr_read(&manager->cancel_fifo, sizeof(fts_atom_t));

	    worker = (fts_thread_worker_t*)fts_get_pointer(atom);
	    
	    success = pthread_cancel(worker->id);
	    if (0 != success)
	    {
		if (ESRCH == success)
		{
		    post("[thread manager] no such thread \n");
		}
		post("[thread manager] error while cancelling thread \n");
	    }
	    work_done++;
	}
	if (0 == work_done)
	{	
	    int delay_ms = manager->delay_ms;
	    struct timespec time_req;
	    struct timespec time_rem;
	    time_req.tv_sec = 0;
	    time_req.tv_nsec = delay_ms * 1000 * 1000;
	    
	    /* post("[thread_manager] thread is running \n"); */
	    nanosleep(&time_req, &time_rem);
	}
    }
    
    return NULL;
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
