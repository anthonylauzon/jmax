/*
   Support for FTS modules.

   A module is declared by a data structure having a number of fields:

   1- An init function, to be called before the computation start.
   2- A shutdown function, to be called before shutting down fts.

   The init function have the responsability to install the scheduled
   function of the module, and possibly welcomes and others; however,
   these functions are not stored in the module data structure.

   Support dynamic loading of C modules.

   FTS modules are initted in the main functions startup.c;
   user, dynamically loaded modules are initted as loaded.

   Init functions are called in the order the modules
   are installed.

   The shutdown functions are called in the *Inverse* order, so that
   if there are installation/shutdown dependencies, they may be naturally
   respected.
*/

#include "sys.h"

#ifdef LINUX
#include <unist.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#ifdef HAS_DL
#include <dlfcn.h>
#endif



extern int fts_file_get_read_path(const char *path, char *full_path);

/******************************************************************************/
/*                                                                            */
/*        Configuration Support                                               */
/*                                                                            */
/******************************************************************************/

fts_module_t *fts_installed_modules = 0;
static fts_module_t *last_installed_module = 0;

/* Install a module at the end of the list of user modules */

void
fts_install_module(fts_module_t *s)
{
  if (last_installed_module)
    last_installed_module->next = s;
  else
    fts_installed_modules = s;

  last_installed_module = s;
  s->next = (fts_module_t *) 0;
}


void
fts_modules_init(void)
{
  fts_module_t *s;

  for (s = fts_installed_modules; s; s = s->next)
    {
      if (s->init_f)
	(* s->init_f)();
    }
}


void
fts_modules_restart(void)
{
  fts_module_t *s;

  for (s = fts_installed_modules; s; s = s->next)
    {
      if (s->restart_f)
	(* s->restart_f)();
    }
}

/* use a recursive function to call the shutdown functions 
   in the inverse order of the  init functions 
*/

static void
fts_recursive_module_shutdown(fts_module_t *s)
{
  if (s->next)
    fts_recursive_module_shutdown(s->next);

  if (s->shutdown_f)
    (* s->shutdown_f)();
}

void
fts_modules_shutdown(void)
{
  fts_recursive_module_shutdown(fts_installed_modules);
}


#ifdef HAS_DL

static fts_status_description_t  module_loaded = { "Module already loaded."};
static fts_status_description_t  library_not_found = { "Library Not Found."};
static fts_status_description_t  error_loading = { "Error loading module."};
static fts_status_description_t  module_not_found = { "Module Not Found."};

/* for architecuture with dynamic loading, load the module;
   the module variable name *must* be called <name>_module;
   the filename default to lib<name>.so if not specified;
   if the filename start with a "/" in the path, it is used
   like it is, otherwise we look use fts_file_get_read_path to look
   for it.

   Note that this function overwrite the module name
   to be sure that correspond to the name passed;
   i.e. force it to adhere to conventions.
 */


fts_status_t
fts_module_load(const char *name, const char *filename)
{
  char namebuf[1024], pathbuf[1024];
  const char *loadpath, *libname;
  fts_module_t  *module, *s;
  void *handle;
  int fd;

  /* test if the module is statically linked */
#if 0
  /* Doesn't work yet :-< ... */
  {
    char module_name[512];
    sprintf(module_name, "%s_module", name);
    
    if (dlsym(NULL, module_name))
      {
	fprintf(stderr, "Module %s statically linked\n", name);
	return fts_Success;
      }
    else
      fprintf(stderr, "Module %s NOT statically linked, error %s\n", name, dlerror());
  }
#endif
  /* test if the module has been already loaded  */

  /* Temporarly commenter @@@ */

  /*
  for (s = fts_installed_modules; s; s = s->next)
    {
      if (s->name && (! strcmp(s->name, name)))
	{
	  fprintf(stderr, "Library %s already loaded\n", name);
	  return &module_loaded;
	}
    }
    */

  if (! filename)
    {
      sprintf(namebuf, "lib%s.so", name);
      libname = namebuf;
    }
  else
    libname = filename;
      
  if (*libname == '/')
    {
      /* test if the file exists and is readable  */

      fd = open(libname, O_RDONLY);

      if (fd < 0)
	{
	  return &library_not_found;
	}
      else
	close(fd);

      loadpath = libname;
    }
  else
    {
      if (fts_file_get_read_path(libname, pathbuf))
	loadpath = pathbuf;
      else
	{
	  return &library_not_found;
	}
    }

  handle = dlopen(loadpath, RTLD_NOW);	

  if (! handle)
    {
      fprintf(stderr, "Error: %s\n",dlerror());
      return &error_loading;
    }

  {
    char module_name[512];
    sprintf(module_name, "%s_module", name);
    
    module  = (fts_module_t *) dlsym(handle, module_name);
  }

  if (! module)
    {
      fprintf(stderr, "Looking for module: %s\n",dlerror());
      return &module_not_found;
    }

  /* add */

  module->name = name;		/* overwrite user declared name */
  fts_install_module(module);
  (* module->init_f)();

  return fts_Success;
}

/*
  This function load the boot time dso modules, specified
  in the $ROOTDIR/fts/lib/config/bootload file, if existing;
  then it load $HOME/.ftsbootload.

  The file format is a list of lines of format

  "module-name absolute-module-file-name"

 */



void
fts_load_boot_modules(void)
{
  FILE *file;
  char name[1024];

  /* make the file name */

  file = fopen("/usr/local/fts/lib/config/bootload", "r");
  
  if (file)
    {
      char name[1024], filename[1024];

      while (! feof(file))
	{
	  int n;

	  n = fscanf(file, "%s %s", name, filename);

	  if (n >= 2)
	    fts_module_load(name, filename);
	}
      
      fclose(file);
    }

  /* make the file name */

  strcpy(name, getenv("HOME"));
  strcat(name, "/.ftsbootload");
  file = fopen(name, "r");
  
  if (file)
    {
      char name[1024], filename[1024];

      while (! feof(file))
	{
	  int n;

	  n = fscanf(file, "%s %s", name, filename);

	  if (n >= 2)
	    fts_module_load(name, filename);
	}
      
      fclose(file);
    }
}

#else

static fts_status_description_t  dynamic_load_not_supported  = { "Dynamic Load Not Supported."};

fts_status_t
fts_module_load(const char *name, const char *filename)
{
  return &dynamic_load_not_supported;
}


void
fts_load_boot_modules(void)
{
}

#endif


