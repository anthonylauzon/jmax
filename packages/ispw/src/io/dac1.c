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
#include <stdlib.h>
#include <string.h>

#include "fts.h"



typedef struct
{
  fts_object_t _o;
  fts_audio_output_logical_device_t *ldev;
  int nchans;
  int *out_index;
} sigdac_t;


static void
sigdac_parse(sigdac_t *this, int ac, const fts_atom_t *av)
{
  if (ac == 0)
    {
      this->nchans = 2;
      this->out_index = (int *) fts_malloc(sizeof(int) * this->nchans);

      this->out_index[0] = 0;
      this->out_index[1] = 1;
    }
  else
    {
      int i;

      this->nchans = ac;
      this->out_index = (int *) fts_malloc(sizeof(int) * this->nchans);

      for(i = 0; i < ac; i++)
	this->out_index[i] = fts_get_long(&av[i]) - 1; /* for backward compatibility, set the idx from 1 to n */
    }
}


static void
dac_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigdac_t *this = (sigdac_t *)o;

  this->ldev = 0;
  this->out_index = 0;

  if (fts_get_symbol(at) == fts_new_symbol("dac~"))
    {
      this->ldev = fts_audio_get_output_logical_device((fts_symbol_t )0);
      sigdac_parse((sigdac_t *)o, ac-1, at+1);
    }
  else if ((fts_get_symbol(at) == fts_new_symbol("out~")) && fts_is_symbol(at+1))
    {
      this->ldev = fts_audio_get_output_logical_device(fts_get_symbol(at+1));
      sigdac_parse((sigdac_t *)o, ac - 2, at + 2);
    }
  else
    {
      post("Wrong arguments in a out~ object \n");
      return;
    }

  fts_audio_add_output_object(this->ldev);
  dsp_list_insert(o); /* just put object in list */
}


static void
dac_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigdac_t *this = (sigdac_t *)o;

  if(this->out_index)
    fts_free(this->out_index);
  if(this->ldev)
    fts_audio_remove_output_object(this->ldev);

  dsp_list_remove(o);
}


static void
dac_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;

  if (ac > 0)
    fts_param_set(fts_s_vector_size, at);

  /* Switch off if already on, before switching on again */

  if (fts_param_get_int(fts_s_dsp_on, 0))
    fts_param_set_int(fts_s_dsp_on, 0);

  fts_param_set_int(fts_s_dsp_on, 1);
}

static void
dac_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}


static void
dac_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigdac_t *this = (sigdac_t *)o;
  int i;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);


  /* If the declared device do not exists (is not active) , we don't generate any code for 
     the dacs
   */

  if (! fts_audio_output_device_is_active(this->ldev))
    return;

  fts_audio_add_scheduled_output_object(this->ldev);

  if (fts_audio_get_output_object_count(this->ldev) == 1)
    {
      /* we test if there is only one dac or output object; if it is the case,
	 generate directly the code to access the device.
	 */

      fts_atom_t *argv;

      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (2 + fts_audio_get_output_channels(this->ldev)));

      fts_set_ptr (argv + 0, fts_audio_get_output_device(this->ldev));
      fts_set_long(argv + 1, fts_dsp_get_input_size(dsp, 0));

      for (i = 0; i < fts_audio_get_output_channels(this->ldev); i++)
	{
	  int j, input;
	  
	  input = -1;

	  for (j = 0; j < this->nchans ; j++)
	    if (this->out_index[j] == i)
	      input = j;

	  if (input == -1)
	    fts_set_symbol(argv + 2 + i, fts_s_sig_zero);
	  else
	    fts_set_symbol(argv + 2 + i, fts_dsp_get_input_name(dsp, input));
		
	}

      dsp_add_funcall(fts_audio_get_output_ftl_function(this->ldev),
		      2 + fts_audio_get_output_channels(this->ldev),
		      argv);	  

      fts_free(argv);
    }
  else if (fts_audio_get_output_object_count(this->ldev) ==
	   fts_audio_get_scheduled_output_object_count(this->ldev))
    {
      /* else, if this is the last DAC to be scheduled, 
	 generate for each stream that was already used an add
	 instruction, and then generate the dev call with the

	 add buffer for the already scheduled buffers, and for  
	 every new stream just put the sig data directly
	 */

      for (i = 0; i < this->nchans; i++)
	if (fts_audio_output_stream_is_installed(this->ldev, this->out_index[i]))
	  if (fts_audio_is_output_buffer_used(this->ldev, this->out_index[i]))
	    {
	      float *buf;
	      fts_atom_t argv[4];

	      buf = fts_audio_get_output_buffer(this->ldev, this->out_index[i]);

	      /* Buffer used, do an add to the buffer */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, i));
	      fts_set_ptr   (argv + 1, buf);
	      fts_set_ptr   (argv + 2, buf);
	      fts_set_long  (argv + 3, fts_dsp_get_input_size(dsp, i));
		
	      dsp_add_funcall(ftl_sym.add.f.vec.vec, 4, argv);
	    }
	  else
	    {
	      /* Buffer not used, do nothing, we will put the buffer directly
		 in the dac call */
	    }

      /* then generate the dev call */

      {
	fts_atom_t *argv;

	argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (2 + fts_audio_get_output_channels(this->ldev)));

	fts_set_ptr (argv + 0, fts_audio_get_output_device(this->ldev));
	fts_set_long(argv + 1, fts_dsp_get_input_size(dsp, 0));

	for (i = 0; i < fts_audio_get_output_channels(this->ldev); i++)
	  {
	    if (fts_audio_is_output_buffer_used(this->ldev, i))
	      {
		/* stream used, take the ftsaudio buffer as argument */
		
		fts_set_ptr(argv + 2 + i, fts_audio_get_output_buffer(this->ldev, i));
	      }
	    else
	    {
	      /* look if the output channel correspond to an input, 
		 (that, since we are here, has not been used before)
		 and put it as argument, or put a sig zero otherwise.
		 */
	      
	      int j, input;
	  
	      input = -1;
	      
	      for (j = 0; j < this->nchans ; j++)
		if (this->out_index[j] == i)
		  input = j;

	      if (input == -1)
		fts_set_symbol(argv + 2 + i, fts_s_sig_zero);
	      else
		fts_set_symbol(argv + 2 + i, fts_dsp_get_input_name(dsp, input));
	    }
	  }

	dsp_add_funcall(fts_audio_get_output_ftl_function(this->ldev),
			2 + fts_audio_get_output_channels(this->ldev), argv);

	fts_free(argv);
      }
    }
  else
    {

      /* Else, we generate the standard code: a move or add for each channel,
	 depending if this was the first object using the stream or not.
	 */

      for (i = 0; i < this->nchans; i++)
	if (fts_audio_output_stream_is_installed(this->ldev, this->out_index[i]))
	  {
	    float *buf;

	    buf = fts_audio_get_output_buffer(this->ldev, this->out_index[i]);

	    if (fts_audio_is_output_buffer_used(this->ldev, this->out_index[i]))
	      {
		fts_atom_t argv[4];

		/* Buffer used, do an add to the buffer */

		fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, i));
		fts_set_ptr   (argv + 1, buf);
		fts_set_ptr   (argv + 2, buf);
		fts_set_long  (argv + 3, fts_dsp_get_input_size(dsp, i));
		
		dsp_add_funcall(ftl_sym.add.f.vec.vec, 4, argv);
	      }
	    else
	      {
		fts_atom_t argv[3];

		/* Buffer not used, do an vmov to the buffer,
		   and set the buffer used */

		fts_audio_set_output_buffer_used(this->ldev, this->out_index[i]);
		fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, i));
		fts_set_ptr   (argv + 1, buf);
		fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, i));

		dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
	      }
	  }
    }
}


static fts_status_t
dac_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i;
  int ninlets;

  if (fts_get_symbol(at) == fts_new_symbol("out~"))
    {
      if (ac <= 1)
	{
	  post("out~: not enough arguments\n");
	  return &fts_CannotInstantiate;
	}
      else if (fts_is_symbol(at+1))
	ninlets = ac - 2;
      else
	{
	  post("out~: first argument must be a symbol\n");
	  return &fts_CannotInstantiate;
	}
    }
  else if (fts_get_symbol(at) == fts_new_symbol("dac~"))
    {
      if (ac == 1)
	ninlets = 2;
      else
	ninlets = ac - 1;
    }

  fts_class_init(cl, sizeof(sigdac_t), ninlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dac_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, dac_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, dac_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("start"), dac_start, 1, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("stop"), dac_stop, 0, 0);

  /*   fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */

  for (i = 0; i < ninlets; i++)
    dsp_sig_inlet(cl, i);

  return fts_Success;
}

void
dac_config(void)
{
  fts_metaclass_create(fts_new_symbol("dac~"), dac_instantiate, fts_narg_equiv);
  fts_metaclass_alias(fts_new_symbol("out~"), fts_new_symbol("dac~"));
}


/* The adc~ Object */

typedef struct 
{
  fts_object_t _o;

  fts_audio_input_logical_device_t *ldev;
  int nchans;
  int *in_index;
} sigadc_t;



static void
sigadc_parse(sigadc_t *this, int ac, const fts_atom_t *av)
{
  int i;

  if (ac == 0)
    {
      this->nchans = 2;
      this->in_index = (int *) fts_malloc(sizeof(int) * this->nchans);
      this->in_index[0] = 0;
      this->in_index[1] = 1;
    }
  else
    {
      this->nchans = ac;
      this->in_index = (int *) fts_malloc(sizeof(int) * this->nchans);

      for(i = 0; i < ac; i++)
	this->in_index[i] = fts_get_long(&av[i]) - 1; /* for backward compatibility, set the idx from 1 to n */
    }
}



static void
adc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigadc_t *this = (sigadc_t *)o;

  this->ldev = 0;
  this->in_index = 0;

  if (fts_get_symbol(at) == fts_new_symbol("adc~"))
    {
      this->ldev = fts_audio_get_input_logical_device((fts_symbol_t )0);
      sigadc_parse((sigadc_t *)o, ac-1, at+1);
    }
  else if ((fts_get_symbol(at) == fts_new_symbol("in~")) && fts_is_symbol(at+1))
    {
      this->ldev = fts_audio_get_input_logical_device(fts_get_symbol(at+1));
      sigadc_parse((sigadc_t *)o, ac - 2, at + 2);
    }
  else
    {
      post("Wrong arguments in a in~ object \n");
      return;
    }

  fts_audio_add_input_object(this->ldev);

  dsp_list_insert(o); /* just put object in list */
}

static void
adc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigadc_t *this = (sigadc_t *)o;

  if(this->in_index)
    fts_free(this->in_index);
  if(this->ldev)
    fts_audio_remove_input_object(this->ldev);

  dsp_list_remove(o);
}

static void
adc_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 0)
    fts_param_set(fts_s_vector_size, at);

  /* Switch off if already on, before switching on again */

  if (fts_param_get_int(fts_s_dsp_on, 0))
    fts_param_set_int(fts_s_dsp_on, 0);

  fts_param_set_int(fts_s_dsp_on, 1);
}

static void
adc_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}


static void
adc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigadc_t *this = (sigadc_t *)o;
  int i;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);


  if (! fts_audio_input_device_is_active(this->ldev))
    {
      /* generate zero fill for all the output buffers  */

      for (i = 0; i < this->nchans; i++)
	{
	  fts_atom_t argv[2];

	  fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, i));
	  fts_set_long  (argv + 1, fts_dsp_get_output_size(dsp, i));

	  dsp_add_funcall(ftl_sym.zero.f, 2, argv); /* why don't use the veclib ftl version@@@@ ??? */
	}
    }
  else if (fts_audio_get_input_object_count(this->ldev) == 1)
    {
      /* if this is the only adc object, do a direct adc operation
	 on the arguments, putting the ftsaudio buffer as
	 fake argument for unused args
	 */

      fts_atom_t *argv;

      fts_audio_add_scheduled_input_object(this->ldev);


      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (2 + fts_audio_get_input_channels(this->ldev)));
      
      fts_set_ptr (argv + 0, fts_audio_get_input_device(this->ldev));
      fts_set_long(argv + 1, fts_dsp_get_output_size(dsp, 0));

      for (i = 0; i < fts_audio_get_input_channels(this->ldev); i++)
	{
	  int j, output;
	  
	  output = -1;

	  for (j = 0; j < this->nchans ; j++)
	    if (this->in_index[j] == i)
	      output = j;

	  if (output == -1)
	    fts_set_ptr(argv + 2 + i, fts_audio_get_input_buffer(this->ldev, i));
	  else
	    fts_set_symbol(argv + 2 + i, fts_dsp_get_output_name(dsp, output));
	}

      dsp_add_funcall(fts_audio_get_input_ftl_function(this->ldev), 2 + fts_audio_get_input_channels(this->ldev), argv);

      fts_free(argv);

      /* also, generate zero fill for the outputs not included in the adc channels */

      for (i = 0; i < this->nchans; i++)
	if (this->in_index[i] > fts_audio_get_input_channels(this->ldev))
	  {
	    fts_atom_t argv[2];

	    fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, i));
	    fts_set_long  (argv + 1, fts_dsp_get_output_size(dsp, i));

	    dsp_add_funcall(ftl_sym.zero.f, 2, argv); /* why don't use the veclib ftl version@@@@ ??? */
	  }
    }
  else
    {
      fts_audio_add_scheduled_input_object(this->ldev);

      if ((fts_audio_get_scheduled_input_object_count(this->ldev) == 1) && fts_audio_get_input_device(this->ldev))
	{
	  /* otherwise, if we are the first adc object, generate the code
	     to read all the input in the ftsaudio buffer, and then
	     generate the copy for the local outputs; we don't know
	     if other adc use the outputs, so we cannot optimize this;
	     we could by doing the used buffer count in dac style
	     at object creation time.
	     */
	  fts_atom_t *argv;

	  argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (2 + fts_audio_get_input_channels(this->ldev)));

	  fts_set_ptr (argv + 0, fts_audio_get_input_device(this->ldev));
	  fts_set_long(argv + 1, fts_dsp_get_output_size(dsp, 0));

	  for (i = 0; i < fts_audio_get_input_channels(this->ldev); i++)
	    fts_set_ptr(argv + 2 + i, fts_audio_get_input_buffer(this->ldev, i));

	  dsp_add_funcall(fts_audio_get_input_ftl_function(this->ldev), 2 + fts_audio_get_input_channels(this->ldev), argv);	  

	  fts_free(argv);
	}

      /* then, we just generate the copies or zero fill for not installed ch*/

      for (i = 0; i < this->nchans; i++)
	if (fts_audio_input_stream_is_installed(this->ldev, this->in_index[i]))
	  {
	    fts_atom_t argv[3];

	    fts_set_ptr   (argv + 0, fts_audio_get_input_buffer(this->ldev, this->in_index[i]));
	    fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, i));
	    fts_set_long  (argv + 2, fts_dsp_get_output_size(dsp, i));
	    dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
	  }
	else
	  {
	    fts_atom_t argv[2];

	    fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, i));
	    fts_set_long  (argv + 1, fts_dsp_get_output_size(dsp, i));

	    dsp_add_funcall(ftl_sym.zero.f, 2, argv);
	  }
    }
}

static fts_status_t
adc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i;
  int noutlets;

  if (fts_get_symbol(at) == fts_new_symbol("in~"))
    {
      if (fts_is_symbol(at+1))
	noutlets = ac - 2;
      else
	{
	  post("in~: first argument must be a symbol\n");
	  return &fts_CannotInstantiate;
	}
    }
  else if (fts_get_symbol(at) == fts_new_symbol("adc~"))
    {
      if (ac == 1)
	noutlets = 2;
      else
	noutlets = ac - 1;
    }

  fts_class_init(cl, sizeof(sigadc_t), 1, noutlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, adc_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, adc_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, adc_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("start"), adc_start, 1, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("stop"), (fts_method_t)adc_stop, 0, a);

  for (i = 0; i < noutlets; i++)
    dsp_sig_outlet(cl, i);

  return fts_Success;
}

void
adc_config(void)
{
  fts_metaclass_create(fts_new_symbol("adc~"),adc_instantiate, fts_narg_equiv);
  fts_metaclass_alias(fts_new_symbol("in~"), fts_new_symbol("adc~"));
}
