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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_DEVICES_H
#define _FTS_DEVICES_H
/* 
   Generic device support include file.

   New implementation of devices.

   The device are typed; the type depend on the type of data
   they implement; there are character device, signal device 
   and so on; every device type have its own specific set of
   operations; note that a device of a type *must* implement
   all the functions of his type !!!

   This module define:

      The device class registration table.
      The device class structure.
      The device structure.
      Generic support for bulding devices.
      Implementation (mostly as macros) of the fts device functions.

      All I/O functions return an fts_status_t; if the status is fts_Success,
      the transfer has been done, otherwise either the data was not
      ready or an error occurred, depending on the semantic of the
      operation.

      All I/O functions get pointers to where store data.

       NEED TO ADD a function, pointer, whatever, that allow
      to know some characteristic of signals device; in particular,
      the most important one is the number of channels; is so universally
      needed (without it, you cannot send signals to them) that is maybe a good
      idea to introduce a function for them.

      Real time Signal functions take their argument under the form of a fts_word_t array
      to be directly callable from the FTL virtual machine.
*/

/* Exported errors */

FTS_API fts_status_description_t fts_dev_invalid_value_error;

FTS_API fts_status_description_t fts_dev_open_error;
FTS_API fts_status_description_t fts_dev_close_error;
FTS_API fts_status_description_t fts_dev_eof;
FTS_API fts_status_description_t fts_dev_io_error;
FTS_API fts_status_description_t fts_data_not_ready;

/* Module declaration */

FTS_API fts_module_t fts_dev_module;

/******************************************************************************/
/*                                                                            */
/*                DEVICE CLASS SUPPORT                                        */
/*                                                                            */
/******************************************************************************/

/*
   The fts_dev_class_t structure; private, but used  by the device class writer thru macros.

*/

struct _fts_dev_class;

/* The device structure */
typedef struct _fts_dev
{
  struct _fts_dev_class *dev_class;
  struct _fts_dev *next;	/* next in the list of open devices */
  void *device_data;
} fts_dev_t;


typedef enum
{
  fts_char_dev,
  fts_sig_dev
} fts_dev_type_t; /* the type of the device */


typedef struct _fts_dev_class
{
  fts_dev_type_t dev_type; /* the type of the device */

  fts_symbol_t class_name;		/* a pointer to the registered name (only one name for a class !!) */


  /* Device handling methods: they are stored in an union; the selected union depend
     on the type of the device */

  /* Common methods */

  fts_status_t (* open_fun)(fts_dev_t *dev, int nargs, const fts_atom_t *args); /* make a device */
  fts_status_t (* close_fun)(fts_dev_t *dev); /* close a device */
  fts_status_t (* ctrl_fun)(fts_dev_t *dev, int nargs, fts_atom_t *args); /*Control the device  */

  union
    {
      struct
	{
	  /* the character device are similar in spirit to the
	   Unix devices; just a getc and a putc function, an optional flush,
	   and a seek.
	   The blocking unblocking nature of the operation depend on 
	   the device itself; some device allow toggle between them
	   with a devctrl.
	   */

	  fts_status_t (* get_fun)(fts_dev_t *dev, unsigned char *cp);
	  fts_status_t (* put_fun)(fts_dev_t *dev, unsigned char c);
	  fts_status_t (* flush_fun)(fts_dev_t *dev);
	  fts_status_t (* seek_fun)(fts_dev_t *dev, long offset, int whence);
	} char_methods;

      struct
	{
	  /* Signal device require a continuos real-time stream of
	     data, and may be bufferized; they get the argument as a
	     pointer to a fts_word_t array, including the arguments in
	     the following order: dev, n, buf1 ... bufm. 

	     */

	  void (* get_fun)(fts_word_t *argv);
	  void (* put_fun)(fts_word_t *argv);

	  /* This two are two names that will be used to register the
	     two above function in the ftl engine; names are generated
	     automatically at the creation of the device class,
	     and accessed with macros defined below */

	  fts_symbol_t get_fun_name;
	  fts_symbol_t put_fun_name;

	  /* the offset is in sample frames here */

	  fts_status_t (* seek_fun)(fts_dev_t *dev, long offset, int whence);

	  /* a disactivated device do not require a continous stream of data; not
	     all devices can be disactivated 
	     */

	  fts_status_t (* activate_fun)(fts_dev_t *dev);
	  fts_status_t (* deactivate_fun)(fts_dev_t *dev);

	  /* information methods */

	  int (* get_nchans_fun)(fts_dev_t *dev);

	  /* this function, if possible, should return
	     the number of errors (overrun samples) since
	     its last call */

	  int (* get_nerrors_fun)(fts_dev_t *dev);
	} sig_methods;

    }  methods;


  /* Pointer in the dev class list */

  struct _fts_dev_class *next;

} fts_dev_class_t;

/*
   Create a fts_dev_class_t structure, properly initialized to null for all of the functions.
   Device Classes are never freed.
*/

FTS_API fts_dev_class_t *fts_dev_class_new(fts_dev_type_t type, fts_symbol_t name);

/* Macro to access a fts_dev_class_t structure; you have to use these macros, and 
   you do not access the structure directly !!! */

#define fts_dev_class_set_open_fun(dev_class, fun)          ((dev_class)->open_fun = (fun))
#define fts_dev_class_set_ctrl_fun(dev_class, fun)          ((dev_class)->ctrl_fun = (fun))
#define fts_dev_class_set_close_fun(dev_class, fun)         ((dev_class)->close_fun = (fun))

#define fts_dev_class_char_set_get_fun(dev_class, fun)      ((dev_class)->methods.char_methods.get_fun = (fun))
#define fts_dev_class_char_set_put_fun(dev_class, fun)      ((dev_class)->methods.char_methods.put_fun = (fun))

#define fts_dev_class_char_set_flush_fun(dev_class, fun)      ((dev_class)->methods.char_methods.flush_fun = (fun))
#define fts_dev_class_char_set_seek_fun(dev_class, fun)     ((dev_class)->methods.char_methods.seek_fun = (fun))

FTS_API void fts_dev_class_sig_set_get_fun(fts_dev_class_t *dev_class,  void (* get_fun)(fts_word_t *));
FTS_API void fts_dev_class_sig_set_put_fun(fts_dev_class_t *dev_class,  void (* put_fun)(fts_word_t *));

#define fts_dev_class_sig_set_seek_fun(dev_class, fun)     ((dev_class)->methods.sig_methods.seek_fun = (fun))

#define fts_dev_class_sig_set_activate_fun(dev_class, fun)      ((dev_class)->methods.sig_methods.activate_fun = (fun))
#define fts_dev_class_sig_set_deactivate_fun(dev_class, fun)    ((dev_class)->methods.sig_methods.deactivate_fun = (fun))


#define fts_dev_class_sig_set_get_nchans_fun(dev_class, fun)    ((dev_class)->methods.sig_methods.get_nchans_fun = (fun))
#define fts_dev_class_sig_set_get_nerrors_fun(dev_class, fun)    ((dev_class)->methods.sig_methods.get_nerrors_fun = (fun))


/* direct function access for direct FTL device access */

#define fts_dev_class_get_sig_get_fun(dev_class)      ((dev_class)->methods.sig_methods.get_fun)
#define fts_dev_class_get_sig_put_fun(dev_class)      ((dev_class)->methods.sig_methods.put_fun)

#define fts_dev_class_get_sig_get_fun_name(dev_class)      ((dev_class)->methods.sig_methods.get_fun_name)
#define fts_dev_class_get_sig_put_fun_name(dev_class)      ((dev_class)->methods.sig_methods.put_fun_name)

/* device classquering */

#define fts_dev_class_get_name(dev_class)   ((dev_class)->class_name)
FTS_API fts_dev_class_t *fts_dev_class_get_by_name(fts_symbol_t  name);

/******************************************************************************/
/*                                                                            */
/*                DEVICE   SUPPORT                                            */
/*                                                                            */
/******************************************************************************/


/* Device level macros & functions */



/* Device capability quering:

   A macro for each possible I/O operation, to test if the 
   device is able to perform a specific role or not.

*/

#define fts_dev_type(dev)                  ((dev)->dev_class->dev_type)
#define fts_dev_get_class(dev)             ((dev)->dev_class)
#define fts_dev_set_device_data(dev, data) ((dev)->device_data = (data))
#define fts_dev_get_device_data(dev)       ((dev)->device_data)

/* device functions */

FTS_API fts_status_t fts_dev_open(fts_dev_t **dret, fts_symbol_t class_name, int nargs, const fts_atom_t *args);
FTS_API fts_status_t fts_dev_close(fts_dev_t *dev);
FTS_API fts_status_t fts_dev_ctrl(fts_dev_t *dev, int nargs, fts_atom_t *args);

/* char dev ops */

/* These functions are implemented as macros.
   Here are the corresponding prototypes:

  extern fts_status_t fts_char_dev_get(fts_dev_t *dev, unsigned char *cp);
  extern fts_status_t fts_char_dev_put(fts_dev_t *dev, unsigned char c);
  extern fts_status_t fts_char_dev_flush(fts_dev_t *dev);
*/

#define fts_char_dev_get(dev, cp)      ((* (dev)->dev_class->methods.char_methods.get_fun)((dev), (cp)))
#define fts_char_dev_put(dev, c)       ((* (dev)->dev_class->methods.char_methods.put_fun)((dev), (c)))
#define fts_char_dev_seek(dev, o, w)   ((* (dev)->dev_class->methods.char_methods.seek_fun)((dev), (o), (w)))
#define fts_char_dev_flush(dev)        ((* (dev)->dev_class->methods.char_methods.flush_fun)((dev)))

/* vector dev ops */

/* These functions are implemented as macros; also, the signal
   functions are usually never called this way, but by pointer thru
   the dsp chain.

*/

#define fts_sig_dev_get(dev, args)        ((* ((dev)->dev_class->methods.sig_methods.get_fun))((args)))
#define fts_sig_dev_put(dev, args)        ((* ((dev)->dev_class->methods.sig_methods.put_fun))((args)))
#define fts_sig_dev_seek(dev, o, w)         ((* ((dev)->dev_class->methods.sig_methods.seek_fun))((dev), (o), (w)))
#define fts_sig_dev_activate(dev)           ((* ((dev)->dev_class->methods.sig_methods.activate_fun))((dev)))
#define fts_sig_dev_deactivate(dev)         ((* ((dev)->dev_class->methods.sig_methods.deactivate_fun))((dev)))
#define fts_sig_dev_get_nchans(dev)         ((* ((dev)->dev_class->methods.sig_methods.get_nchans_fun))((dev)))
#define fts_sig_dev_get_nerrors(dev)         ((* ((dev)->dev_class->methods.sig_methods.get_nerrors_fun))((dev)))


/* Logical device

   A logical device is a variable keeping one or more devices;
   this variable can be accessed and set thru dedicated functions
   or thru a set of UCS commands.

   A logical device can corrispond to more than one phisical device,
   like a table of devices or so; the actual device is specified
   by a list of atoms; for compatibility with the UCS commands, 
   none of these atoms should be the symbol "as".

   The logical device is declared with fts_declare_logical_dev;
   this install functions that will be called to set unset or access the
   device itself; it also automatically add the handling of the logical
   device from UCS.

   Finally, the logical device is typed, and the type check is made
   in the support function themselve.

   The syntax for UCS commands is:

   open <logical device spec> as <phisical device spec>
   close <logical device spec>

   For example:

   open midi port0 as sgi-midi tty02

 */



FTS_API void fts_declare_logical_dev(fts_symbol_t name,
				    fts_dev_type_t dev_type,
				    fts_status_t (* set_fun)(fts_dev_t *dev, int ac, const fts_atom_t *at),
				    fts_dev_t   *(* get_fun)(int ac, const fts_atom_t *at),
				    fts_status_t (* unset_fun)(int ac, const fts_atom_t *at),
				    fts_status_t (* reset_fun)(void));

/* Low level C access to logical devices; callers should take care of 
 closing and unsetting previous devices */


FTS_API fts_status_t fts_set_logical_device(fts_dev_t *dev, fts_symbol_t name, int ac, const fts_atom_t *at);
FTS_API fts_dev_t *fts_get_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at);
FTS_API fts_status_t fts_unset_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at);

/* functions to handle logical devices */

FTS_API fts_status_t fts_open_logical_device(fts_symbol_t name, int lac, const fts_atom_t *lat,
					    fts_symbol_t pname, int pac, const fts_atom_t *pat);

FTS_API fts_status_t fts_close_logical_device(fts_symbol_t name, int ac, const fts_atom_t *at);

/* Close all the devices of a logical device */

FTS_API fts_status_t fts_reset_logical_device(fts_symbol_t name);



/* (fd) a function that is prototyped here, but defined in lang/dsp/dsp.c !!!! */
FTS_API void fts_dsp_set_dac_slip_dev( fts_dev_t *dev);

#endif









