/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* 
   Generic device support.

   This module define:

      The device class registration table.
      The device class structure.
      The device structure.
      Generic support for bulding devices.
      Implementation (mostly as macros) of the fts device functions.
      
*/

#include <string.h>
#include "sys.h"
#include "lang.h"
#include "runtime/devices/devices.h"
#include "runtime/devices/devconf.h"

/******************************************************************************/
/*                                                                            */
/*                LOCAL ERROR  DECLARATIONS                                   */
/*                                                                            */
/******************************************************************************/


static fts_status_description_t fts_devclass_already_installed =
{
  "device classes already installed"
};

static fts_status_description_t no_ctrl_cmd = 
{
  "the requested CTRL operation is not implemented for the device"
};

static fts_status_description_t no_io_operation = 
{
  "the requested i/o operation is not implemented for the device"
};

static fts_status_description_t no_ctrl_param = 
{
  "the requested CTRL parameter is not implemented for the device"
};

static fts_status_description_t ctrl_syntax_error = 
{
  "Syntax error in the requested CTRL operation"
};


static fts_status_description_t open_device_syntax_error = 
{
  "Syntax error in  open device command"
};

static fts_status_description_t open_device_error = 
{
  "Error opening device."
};

static fts_status_description_t close_device_syntax_error = 
{
  "Syntax error in  close device operation"
};


/******************************************************************************/
/*                                                                            */
/*                EXPORTED ERROR  DECLARATIONS                                */
/*                                                                            */
/******************************************************************************/

/* The following errors can be used by device writers to signal generic 
   errors in devices; they are declared in dev.h.
*/

fts_status_description_t fts_dev_invalid_value_error =
{
  "Invalid value for a parameter in open or dev_ctrl operation"
};


fts_status_description_t fts_dev_open_error =
{
  "cannot open device"
};

fts_status_description_t fts_dev_close_error =
{
  "cannot close device"
};

/* this is used to communicate eof condition on a get/put operation */

fts_status_description_t fts_dev_eof =
{
  "eof for device"
};


fts_status_description_t fts_dev_io_error =
{
  "io error for device"
};


/* This is not an error, shouldn't be raised or posted,
   it just means that a "if_any" operation didn't find the data.

   This status description is exported so to be used by the
   device implementor.
*/

fts_status_description_t fts_data_not_ready = 
{
  "data not ready"
};

/******************************************************************************/
/*                                                                            */
/*                Module DEVICE SUPPORT                                       */
/*                                                                            */
/******************************************************************************/

extern void fts_dev_configure(void);
static void fts_dev_init(void);
static void fts_dev_shutdown(void);

static fts_dev_class_t *dev_class_list = 0;
static fts_dev_t *open_dev_list = 0;

fts_module_t fts_dev_module = {"dev", "The FST device set", fts_dev_init, 0, fts_dev_shutdown};

static void
fts_dev_init(void)
{
  /* Configure the good devices depending on the platform we are */

  fts_dev_configure();		/* see file devconf.c */
}

/*
   dev_shutdown close all the open devices.
*/

static void
fts_dev_shutdown()
{
  while (open_dev_list)
    {
      fts_dev_t *p;

      p = open_dev_list;

      open_dev_list = open_dev_list->next;

      fts_dev_close(p);		/* this free the device structure, also !! */
    }
}

/******************************************************************************/
/*                                                                            */
/*                DEVICE CLASS SUPPORT                                        */
/*                                                                            */
/******************************************************************************/

/*
   The fts_dev_class_t structure is defined in dev.h; pubblic, used by the device class writer 
*/

/* default void function for devices */

static fts_status_t
void_open_fun(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  return fts_Success; /* for open/less devices  */
}

static fts_status_t
void_close_fun(fts_dev_t *dev)
{
  return fts_Success; /* for close/less devices  */
}

static fts_status_t
void_ctrl_fun(fts_dev_t *dev, int nargs, fts_atom_t *args)
{
  return &no_io_operation;
}

static fts_status_t
void_dev_fun(fts_dev_t *dev)
{
  return &no_io_operation;
}

static fts_status_t
void_char_get_fun(fts_dev_t *dev, unsigned char *cp)
{
  *cp = '\0';
  return &no_io_operation;
}

static fts_status_t
void_char_put_fun(fts_dev_t *dev, unsigned char c)
{
  return &no_io_operation;
}


static fts_status_t
void_char_flush_fun(fts_dev_t *dev)
{
  return &no_io_operation;
}

static fts_status_t
void_seek_fun(fts_dev_t *dev, long offset, int whence)
{
  return &no_io_operation;
}


static void
void_sig_get_put_fun(fts_word_t *args)
{
}

static int
void_sig_get_nchans_fun(fts_dev_t *dev)
{
  return 0;
}


static int
void_sig_get_nerrors_fun(fts_dev_t *dev)
{
  return 0;
}


/*
   Create a fts_dev_class_t structure, properly initialized to null or doing nothing functions
   for all of the functions.
*/

fts_dev_class_t *
fts_dev_class_new(fts_dev_type_t type)
{
  fts_dev_class_t *dev_class;

  dev_class = (fts_dev_class_t *) fts_malloc(sizeof(fts_dev_class_t));

  dev_class->dev_type = type;
  dev_class->open_fun = void_open_fun;
  dev_class->ctrl_fun = void_ctrl_fun;
  dev_class->close_fun = void_close_fun;

  switch(dev_class->dev_type)
    {
    case fts_char_dev:
      dev_class->methods.char_methods.get_fun = void_char_get_fun;
      dev_class->methods.char_methods.put_fun = void_char_put_fun;
      dev_class->methods.char_methods.flush_fun = void_char_flush_fun;
      dev_class->methods.char_methods.seek_fun = void_seek_fun;
      break;

    case fts_sig_dev:
      dev_class->methods.sig_methods.get_fun = void_sig_get_put_fun;
      dev_class->methods.sig_methods.put_fun = void_sig_get_put_fun;
      dev_class->methods.sig_methods.seek_fun = void_seek_fun;
      dev_class->methods.sig_methods.activate_fun = void_dev_fun;
      dev_class->methods.sig_methods.deactivate_fun = void_dev_fun;
      dev_class->methods.sig_methods.get_nchans_fun = void_sig_get_nchans_fun;
      dev_class->methods.sig_methods.get_nerrors_fun = void_sig_get_nerrors_fun;
      break;
    }

  dev_class->next = 0;		/* put it in the list at register time */

  return dev_class;
}


/* 
   fts_dev_class_register install the device class in the device class table.

   The device class table is actually only a list of classes, because the class itself
   include a pointer to the name.
*/

/* local utility to find a class by name in the table (if the table become bigger
 we can put an hash table) */

static fts_dev_class_t *
fts_dev_class_get_by_name(fts_symbol_t  name)
{
  fts_dev_class_t *p;

  for (p = dev_class_list; p; p = p->next)
    if (p->class_name ==  name)
      return p;
  
  return (fts_dev_class_t *)0;
}

fts_status_t
fts_dev_class_register(fts_symbol_t name, fts_dev_class_t *dev_class)
{
  fts_dev_class_t *p;

  /* Check to see if the class was already registered */

  for (p = dev_class_list; p; p = p->next)
    if (p == dev_class)
      return &fts_devclass_already_installed;

  /* Check to see if the class name was already installed (we give the same error :-<)*/

  if (fts_dev_class_get_by_name(name))
    return &fts_devclass_already_installed;

  dev_class->class_name = name;
  dev_class->next = dev_class_list;
  dev_class_list  = dev_class;

  return fts_Success;    
}




/******************************************************************************/
/*                                                                            */
/*                DEVICE   SUPPORT                                            */
/*                                                                            */
/******************************************************************************/


/* 
   The device structure; users should not access this structure directly.

   The device is defined by the class, a set of functions, plus a device_data void * pointer.
   
   Open take the class name as argument.

   Close/Ctrl functions take the device as argument, so that they can
   also access  to the class.

   On the contrary, the Write and Read functions are passed directly the device_data,
   that should be a pointer to the device housekeeping data structure.

   Note that the fts_dev_t structure is automatically allocated by the system,
   but is responsability of the class methods to allocate device local memory.
   
*/


/* Device level operations */


fts_dev_t *
fts_dev_open(fts_symbol_t class_name, int nargs, const fts_atom_t *args)
{
  fts_dev_class_t *dev_class;

  dev_class = fts_dev_class_get_by_name(class_name);


  if (dev_class)
    {
      fts_dev_t *dev;
      fts_status_t ret;

      dev = (fts_dev_t *) fts_malloc(sizeof(struct _fts_dev));
      
      dev->dev_class = dev_class;
      dev->device_data = (void *)0;

      ret = (* dev_class->open_fun)(dev, nargs, args);
      
      if (ret == fts_Success)
	{
	  dev->next = open_dev_list;
	  open_dev_list = dev;
	  return dev;
	}
      else
	{
	  post("Error Opening Device");
	  post_atoms(nargs, args);
	  post(": %s\n", ret->description);

	  return (fts_dev_t *)0;
	}
    }
  else
    return (fts_dev_t *)0;
}

fts_status_t
fts_dev_close(fts_dev_t *dev)
{
  fts_status_t ret;
  fts_dev_t **p;		/* indirect pre-cursor */

  ret = (* dev->dev_class->close_fun)(dev);  

  for (p = &open_dev_list; *p ; p = &((*p)->next))
    if (*p == dev)
      {
	*p = (*p)->next;
	break;
      }

  fts_free(dev);

  return ret;
}

/*
   Note that fts_dev_ctrl can be used to obtain informations about the device,
   because atoms are extended with pointers. Also fts_status_t can be used
   to retrun (boolean, may be) values.

   This function may be extended to support generic operations, like
   quering which kind of operations are available, to get the device class
   or similar games.
*/

fts_status_t
fts_dev_ctrl(fts_dev_t *dev, int nargs, fts_atom_t *args)
{
  if (dev->dev_class->ctrl_fun)
    return (* dev->dev_class->ctrl_fun)(dev, nargs, args);
  else
    return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                Logical Device SUPPORT                                      */
/*                                                                            */
/******************************************************************************/

struct fts_logical_dev_declaration
{
  fts_symbol_t name;
  fts_dev_type_t type;
  fts_status_t (* set_fun)(fts_dev_t *dev, int ac, const fts_atom_t *at);
  fts_dev_t   *(* get_fun)(int ac, const fts_atom_t *at);
  fts_status_t (* unset_fun)(int ac, const fts_atom_t *at);
  fts_status_t (* reset_fun)(void);

  struct fts_logical_dev_declaration *next;
};

static struct fts_logical_dev_declaration *logical_dev_list = 0;

/* Function to install a logical device */

void
fts_declare_logical_dev(fts_symbol_t name,
			fts_dev_type_t dev_type,
			fts_status_t (* set_fun)(fts_dev_t *dev, int ac, const fts_atom_t *at),
			fts_dev_t   *(* get_fun)(int ac, const fts_atom_t *at),
			fts_status_t (* unset_fun)(int ac, const fts_atom_t *at),
			fts_status_t (* reset_fun)(void)
			)
{
  struct fts_logical_dev_declaration *ld;

  ld = (struct fts_logical_dev_declaration *) fts_malloc(sizeof(struct fts_logical_dev_declaration));

  ld->name = name;
  ld->type = dev_type;
  ld->set_fun   = set_fun;
  ld->get_fun   = get_fun;
  ld->unset_fun = unset_fun;
  ld->reset_fun = reset_fun;

  ld->next = logical_dev_list;
  logical_dev_list = ld;
}

/* utility function to access the logical device by name */

static struct fts_logical_dev_declaration *get_logical_dev_declaration(fts_symbol_t logical_dev_name)
{
  struct fts_logical_dev_declaration *p;

  for (p = logical_dev_list; p ; p = p->next)
    if (p->name == logical_dev_name)
      return p;

  return 0;
}


fts_dev_t *fts_get_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at)
{
  struct fts_logical_dev_declaration *ld;

  ld = get_logical_dev_declaration(name);

  if (ld)
    return (* ld->get_fun)(ac, at);
  else
    return (fts_dev_t *)0;
}


fts_status_t fts_set_logical_device(fts_dev_t *dev, fts_symbol_t name, int ac, const fts_atom_t *at)
{
  struct fts_logical_dev_declaration *ld;

  ld = get_logical_dev_declaration(name);


  return (* ld->set_fun)(dev, ac, at);
}


fts_status_t fts_unset_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at)
{
  fts_status_t ret;
  struct fts_logical_dev_declaration *ld;

  ld = get_logical_dev_declaration(name);

  ret = (* ld->unset_fun)(ac, at);

  return ret;
}


/* get two arrays of atoms: first the description of the logical
   device, and then the description of the physical device to open
   */

fts_status_t fts_open_logical_device(fts_symbol_t name, int lac, const fts_atom_t *lat,
				     fts_symbol_t pname, int pac, const fts_atom_t *pat)
{
  fts_dev_t *dev;
  fts_status_t ret;

  /* close the logical device */

  fts_close_logical_device(name, lac, lat);

  /* Open the new one  */

  dev = fts_dev_open(pname, pac, pat);

  /* And set it as the logical device  */

  if (dev)
    {
      ret = fts_set_logical_device(dev, name, lac, lat);

      if (ret != fts_Success)
	fts_dev_close(dev);
	  
      return ret;
    }
  else
    {
      return &open_device_error; /* bad set device command */
    }
}




fts_status_t fts_close_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at)
{
  fts_dev_t *dev;
  fts_status_t ret;

  /* unset the logical device and close the physical device */

  dev = fts_get_logical_device(name, ac, at);

  if (dev)
    {
      ret = fts_unset_logical_device(name, ac, at);

      fts_dev_close(dev);
      return ret;
    }
  else
    return fts_Success;
}


fts_status_t fts_reset_logical_device(fts_symbol_t name)
{
  struct fts_logical_dev_declaration *ld;

  ld = get_logical_dev_declaration(name);

  if (ld)
    return (* ld->reset_fun)();
  else
    return fts_Success;
}



