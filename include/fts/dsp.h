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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


/**
 * The FTS DSP subsystem
 *
 * The DSP subsystem consits of a compiler and a virtual machine.
 * The graph of DSP objects (connected DSP objects) is serialized by the DSP compiler and the
 * resulting program is run by the virtual machine once for each block of samples (tick size).
 *
 * The global sample rate and tick size of the DSP subsystem can be obtained by the functions 
 * fts_dsp_get_sample_rate() and fts_dsp_get_tick_size(). Although the sample rate and tick size 
 * of an objects inputs or outputs can be differ from the global parameters due to up or 
 * downsampling instructions of the DSP compiler (not part of the general API).
 *
 * An object can be declared as DSP object in its init method using the function fts_dsp_add_object().
 * By convention a DSP object has a name ending with '~'.
 *
 * A DSP object will be send a message \e put when it is scheduled by the compiler of the DSP subsystem 
 * during the serialization of the DSP graph. In the \e put method the object can check the parameter
 * values of its inputs and outputs and insert (fts_dsp_add_function()) its DSP function to the program 
 * of the virtual machine.
 *
 * The \e put method is called with a DSP descriptor as its only argument.
 * From this descriptor the local DSP parameters (vector size and sample rate) for each DSP input and output
 * can be extracted using the functions fts_dsp_get_input_size(), fts_dsp_get_output_size(), 
 * fts_dsp_get_input_srate() and fts_dsp_get_output_srate().
 * 
 * Code example of a typical \e put method:
 *
 * @code
 * static void
 * my_dsp_object_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * { 
 *    my_dsp_object_t *this = (my_dsp_object_t *)o;
 *    fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
 *    double sr = fts_dsp_get_output_srate(dsp, 0);
 *    int n_tick = fts_dsp_get_output_size(dsp, 0);
 *    fts_atom_t a[4];
 *
 *    my_dsp_object_reset(this, n_tick, sr);
 *
 *    fts_set_pointer(a + 0, this->params);
 *    fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
 *    fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
 *    fts_set_int(a + 3, n_tick);
 *    fts_dsp_add_function(my_dsp_object_dsp_function, 4, a);
 * }
 * @endcode
 *
 * Note that in the current implementation all DSP inputs and outputs have the same vector size and sample rate.
 *
 * Code example of a typical DSP function (matching the \e put example above):
 *
 * @code
 * static void
 * my_dsp_object_dsp_function(fts_word_t *argv)
 * {
 *   my_dsp_params_t *params = (my_dsp_params_t *)fts_word_get_pointer(argv + 0);
 *   float *in = (float *)fts_word_get_pointer(argv + 1);
 *   float *out = (float *)fts_word_get_pointer(argv + 2);
 *   int n_tick = fts_word_get_int(argv + 3);
 *   int i;
 *
 *   for(i=0; i<n_tick; i++)
 *     out[i] = my_dsp_transformation(in[i], params);
 * }
 * @endcode
 *
 * @defgroup dsp DSP subsystem
 */

/* Kernel Parameter names */ 
FTS_API fts_symbol_t fts_s_dsp_on;
FTS_API fts_symbol_t fts_s_sample_rate;

/* Symbols used by the DSP compiler */
/* Signal 0: always 0 */
FTS_API fts_symbol_t fts_s_sig_zero;
/* Up/downsampling properties */
FTS_API fts_symbol_t fts_s_dsp_upsampling;
FTS_API fts_symbol_t fts_s_dsp_downsampling;



/** 
 * @name DSP compiler structures
 */
/*@{*/

typedef struct _fts_dsp_signal_t {
  fts_symbol_t name;
  int refcnt;
  int length;
  float srate;
} fts_dsp_signal_t;

typedef struct _fts_dsp_descr_t {
  int ninputs;
  int noutputs;
  fts_dsp_signal_t **in;
  fts_dsp_signal_t **out;
} fts_dsp_descr_t;


/*@}*/ /* DSP compiler structures */


FTS_API void fts_dsp_run_tick(void);
FTS_API int fts_dsp_is_running(void);
FTS_API void fts_dsp_restart(void);

FTS_API void fts_dsp_timebase_configure(void);

/** 
 * @name Runtime parameters
 */
/*@{*/

/**
 * Get the global sample rate of the DSP subsystem
 *
 * Note that the actual values for sample rate and tick size can be different from
 * the parameter values returned by fts_dsp_get_sample_rate() and fts_get_tick_size().
 * Use the functions fts_dsp_get_input_size(), fts_dsp_get_output_size(), 
 * fts_dsp_get_input_srate() or fts_dsp_get_output_srate() in order 
 * to get the correct local values for a particular object inside the put method.
 *
 * @fn double fts_dsp_get_sample_rate(void)
 * @return the sample rate
 *
 * @see fts_get_tick_size()
 *
 * @ingroup dsp
 */
FTS_API double fts_dsp_get_sample_rate(void);

/**
 * Get the global tick size of the DSP subsystem
 *
 * The \e tick \e size is the size block size of the DSP calculations performed by the
 * DSP subsystem.
 *
 * @fn double fts_dsp_get_tick_size(void)
 * @return the sample rate
 *
 * @see fts_get_sample_rate()
 *
 * @ingroup dsp
 */
FTS_API int fts_dsp_get_tick_size(void);

/*@}*/ /* runtime parameters */

/** 
 * @name Class declarations
 */
/*@{*/

/**
 * Declare a DSP function by name
 *
 * @fn void fts_dsp_declare_function(fts_symbol_t name, void (*fun)(fts_word_t *))
 * @param name name of the DSP function
 * @param the DSP function
 *
 * @see fts_dsp_add_function()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *));

/**
 * Declare a DSP inlet for a class
 *
 * @fn fts_dsp_declare_inlet(fts_class_t *class, int number)
 * @param class the class
 * @param number number of the inlet
 *
 * @see fts_dsp_declare_outlet()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_declare_inlet(fts_class_t *class, int number);

/**
 * Declare a DSP outlet for a class
 *
 * @fn fts_dsp_declare_outlet(fts_class_t *class, int number)
 * @param class the class
 * @param number number of the outlet
 *
 * @see fts_dsp_declare_inlet()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_declare_outlet(fts_class_t *class, int number);

/*@}*/ /* Class declarations */

/** 
 * @name Object declarations
 */
/*@{*/

/**
 * Add object to the DSP graph (set of DSP objects).
 *
 * This function declares an object as DSP object and add it to the DSP graph.
 * Typically it is called in the objects init method.
 *
 * @fn void fts_dsp_add_object(fts_object_t *o)
 * @param object
 *
 * @see fts_dsp_remove_object()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_add_object(fts_object_t *object);

/** 
 * Remove object from the DSP graph.
 *
 * This function removes the object from the DSP graph which is added with fts_dsp_add_object().
 * Typically it is called in the objects delete method.
 *
 * @fn void fts_dsp_remove_object(fts_object_t *o)
 * @param object
 *
 * @see fts_dsp_add_object()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_remove_object(fts_object_t *object);

/* test inputs */
FTS_API int fts_dsp_is_sig_inlet(fts_object_t *object, int number);
FTS_API int fts_dsp_is_input_null(fts_dsp_descr_t *descriptor, int in);

/*@}*/ /* Object declarations */

/** 
 * @name Functions of the put method
 */
/*@{*/

/**
 * Get name of input
 *
 * Returns a symbol associated to the specified input. Using this symbol inputs and outputs can be
 * compared and given as arguments fts_dsp_add_function().
 *
 * @fn int fts_dsp_get_input_name(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the input (must be declared as DSP inlet)
 * @return name of input
 *
 * @see fts_dsp_add_function()
 *
 * @ingroup dsp
 */
FTS_API fts_symbol_t fts_dsp_get_input_name(fts_dsp_descr_t *descriptor, int number);

/**
 * Get input vector size
 *
 * Returns size of an input signal associated to the specified input.
 *
 * Note that in the current implementation all DSP inlets have the same vector size and sample rate.
 *
 * @fn int fts_dsp_get_input_size(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the input (must be declared as DSP inlet)
 * @return size of input signal vector
 *
 * @see fts_dsp_declare_inlet()
 * @see fts_dsp_get_tick_size()
 *
 * @ingroup dsp
 */
/* see macros down */
FTS_API int fts_dsp_get_input_size(fts_dsp_descr_t *descriptor, int number);

/**
 * Get input sample rate
 *
 * Returns the sample rate of the input signal associated to the specified input.
 *
 * Note that in the current implementation all DSP inlets have the same vector size and sample rate.
 *
 * @fn int fts_dsp_get_input_srate(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the input (must be declared as DSP inlet)
 * @return size of input sample rate
 *
 * @see fts_dsp_declare_inlet()
 * @see fts_dsp_get_sample_rate()
 *
 * @ingroup dsp
 */
FTS_API int fts_dsp_get_input_srate(fts_dsp_descr_t *descriptor, int number);

/**
 * Get name of output
 *
 * Returns a symbol associated to the specified output. Using this symbol inputs and outputs can be
 * compared and given as arguments fts_dsp_add_function().
 *
 * @fn int fts_dsp_get_output_name(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the input (must be declared as DSP inlet)
 * @return name of output
 *
 * @see fts_dsp_add_function()
 *
 * @ingroup dsp
 */
FTS_API fts_symbol_t fts_dsp_get_output_name(fts_dsp_descr_t *descriptor, int number);

/**
 * Get output vector size
 *
 * Returns size of an output signal associated to the specified output.
 *
 * Note that in the current implementation all DSP inlets have the same vector size and sample rate.
 *
 * @fn int fts_dsp_get_output_size(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the output (must be declared as DSP outlet)
 * @return size of output signal vector
 *
 * @see fts_dsp_declare_outlet()
 * @see fts_dsp_get_tick_size()
 *
 * @ingroup dsp
 */
FTS_API int fts_dsp_get_output_size(fts_dsp_descr_t *descriptor, int number);

/**
 * Get output sample rate
 *
 * Returns the sample rate of the output signal associated to the specified output.
 *
 * Note that in the current implementation all DSP inlets have the same vector size and sample rate.
 *
 * @fn int fts_dsp_get_output_srate(fts_dsp_descr_t *descriptor, int number)
 * @param descriptor the DSP descriptor (argument of the put method)
 * @param number number of the output (must be declared as DSP outlet)
 * @return size of output sample rate
 *
 * @see fts_dsp_declare_outlet()
 * @see fts_dsp_get_sample_rate()
 *
 * @ingroup dsp
 */
FTS_API int fts_dsp_get_output_srate(fts_dsp_descr_t *descriptor, int number);

/**
 * Insert DSP function to DSP chain
 *
 * @fn void fts_dsp_add_function(fts_symbol_t name, int ac, fts_atom_t *av)
 * @param name the function name declared by fts_dsp_declare_function()
 * @param number of arguments of the DSP function
 * @param array of arguments of the DSP function
 *
 * @see fts_dsp_declare_function()
 * @see fts_dsp_add_object()
 *
 * @ingroup dsp
 */
FTS_API void fts_dsp_add_function(fts_symbol_t name, int ac, fts_atom_t *av);
FTS_API double fts_dsp_get_time(void);

/*@}*/ /* Functions of the put method */

/* macros to get input properties */
#define fts_dsp_get_input_name(DESC, IN) ((DESC)->in[(IN)]->name)
#define fts_dsp_get_input_size(DESC, IN) ((DESC)->in[(IN)]->length)
#define fts_dsp_get_input_srate(DESC, IN) ((DESC)->in[(IN)]->srate)

/* macros to get output properties */
#define fts_dsp_get_output_name(DESC, OUT) ((DESC)->out[(OUT)]->name)
#define fts_dsp_get_output_size(DESC, OUT) ((DESC)->out[(OUT)]->length)
#define fts_dsp_get_output_srate(DESC, OUT) ((DESC)->out[(OUT)]->srate)




/* internal misc */
FTS_API fts_object_t *dsp_get_current_object(void);
FTS_API void dsp_add_signal(fts_symbol_t name, int size);
FTS_API void dsp_chain_post(void);
FTS_API void dsp_chain_post_signals(void);
FTS_API void dsp_chain_fprint(FILE *f);
FTS_API void dsp_chain_fprint_signals(FILE *f);
FTS_API ftl_program_t *dsp_get_current_dsp_chain( void);

FTS_API void fts_dsp_add_function_zero(fts_symbol_t signal, int size);
FTS_API void fts_dsp_add_function_copy(fts_symbol_t in, fts_symbol_t out, int size);

/* old names of user API */
#define dsp_list_insert(o) fts_dsp_add_object(o)
#define dsp_list_remove(o) fts_dsp_remove_object(o)
#define dsp_sig_inlet(c, i) fts_dsp_declare_inlet((c), (i))
#define dsp_sig_outlet(c, i) fts_dsp_declare_outlet((c), (i))
#define dsp_declare_function(n, f) fts_dsp_declare_function((n), (f))
#define dsp_add_funcall(s, n, a) fts_dsp_add_function((s), (n), (a))

