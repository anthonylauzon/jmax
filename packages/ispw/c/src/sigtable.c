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
 */

#include <sys/types.h>
#include <string.h>
#include <fts/fts.h>
#include <ftsconfig.h>

#include <sys/types.h>
#include <string.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "sampbuf.h"
#include "sampunit.h"
#include "naming.h"

typedef struct{
  long magic;          /* must be equal to SND_MAGIC */
  long dataLocation;   /* Offset or pointer to the raw data */
  long dataSize;       /* Number of bytes of data in the raw data */
  long dataFormat;     /* The data format code */
  long samplingRate;   /* The sampling rate */
  long channelCount;   /* The number of channels */
  long info;           /* Textual information relating to the sound. */
} SNDSoundStruct;

#define SND_MAGIC ((long)0x2e736e64)
#define SND_FORMAT_LINEAR_16 (3)

#define TEMPBUFSIZE 16384
#define TEMPBUFSAMPS (int)(TEMPBUFSIZE/sizeof(filesamp_t))

/******************************************************************
 *
 *    object
 *
 */

typedef struct{
  fts_object_t  _o;
  fts_symbol_t name; /* its name */
  sampbuf_t buf; /* pointer to the sampbuf */
  float check_size; /* store size here to check it @ dsp start */
  fts_symbol_t unit; /* S, KS, msec or sec */
} sigtable_t;


static void
sigtable_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 2);
  float size = (unit ? fts_get_float_arg(ac, at, 3, 0.0f) : fts_get_float_arg(ac, at, 2, 0.0f));
  int n_samps;
  float *samp_buf;

  this->name = 0;

  if (!unit)
    unit = samples_unit_get_default();
  
  if (sampbuf_name_already_registered(name))
    {
      post("table~: %s: multiply defined (last ignored)\n", name);
      return;
    }
  else
    sampbuf_add(name, (void *)&(this->buf));
  
  if (size < 0)
    size = 0;

  n_samps = samples_unit_convert(unit, size, fts_dsp_get_sample_rate());
  
  sampbuf_init(&this->buf, n_samps);

  this->name = name;

  if (this->name)
    ispw_register_named_object(o, this->name);

  this->check_size = size;
  this->unit = unit;

  dsp_list_insert(o);
}

static void
sigtable_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;

  if (this->name)
    ispw_unregister_named_object(o, this->name);

  if (this->name)
    sampbuf_remove(this->name);

  sampbuf_erase(&this->buf);

  dsp_list_remove(o);
}
 
/******************************************************************
 *
 *    dsp
 *
 */
 
/* this is a pseudo put function to check the table size and may reallocate the table */

static void
put_dsp_check_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  
  if (!this->name)
    post("table~: dead object\n");
  else
    {
      int n_samps = samples_unit_convert(this->unit, this->check_size, fts_dsp_get_sample_rate());

      sampbuf_realloc(&this->buf, n_samps);
    }
}

/**************************************************
 *
 *    user methods
 *
 */ 
 
static void
sigtable_read(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  int onset = fts_get_int_arg(ac, at, 1, 0);
  int size = this->buf.size;
  float *buf_ptr = this->buf.samples;
  int samps_left, samps_to_read, samps_read;
  char tempbuf[TEMPBUFSIZE+320];
  FILE* fd;

  if(!this->name)
    return;

  if ((fd = fts_file_open(file_name, "r")) < 0){
    post("table~: %s: can't open\n", file_name);
    return;
  }

  if (fseek(fd, onset, 0) < 0){
    post("table~: %s: can't seek to beginning\n", file_name);
    fts_file_close(fd);
    return;
  }

  samps_left = samps_to_read = size + GUARDPTS;
  while(samps_left > 0){
    int gimme = samps_left, n_bytes, bytes_read;
    char *rats;
    if (gimme > TEMPBUFSAMPS) gimme = TEMPBUFSAMPS;

    bytes_read = fread(tempbuf, 1, gimme * sizeof(filesamp_t), fd);
    if (bytes_read & 1){
      post("table~: dropping odd byte\n");
      bytes_read &= ~1;
    }

    if (bytes_read < 0) post("table~: error reading file: %s\n", file_name);
    if (bytes_read <= 0) break;
    
    for(n_bytes = bytes_read / sizeof(filesamp_t), rats = tempbuf; n_bytes--; rats += sizeof(filesamp_t)){

      *(buf_ptr++) = (1.0f/32767.0f) * *(filesamp_t *)rats;
    }
    samps_left -= bytes_read / sizeof(filesamp_t);
  }
  
  samps_read = samps_to_read - samps_left;
  if (samps_read > size) samps_read = size;

  post("table~: read %d samples (out of %d in table) from file: %s\n", samps_read, size, file_name);
  
  while(samps_left-- > 0) *buf_ptr++ = 0.0f;

  fts_file_close(fd);
}

static void
sigtable_write(fts_object_t *o, int winlet, fts_symbol_t sym, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  FILE* fd;
  int samps_to_write;
  char tempbuf[TEMPBUFSIZE];
  SNDSoundStruct header;
  float *buf_ptr;
  int bufno = 0;

  if (!this->name)
    return;

  if ((fd = fts_file_open(file_name, "w")) < 0){
    post("table~: can't create file: %s\n", file_name);
    return;
  }
  header.magic = SND_MAGIC;
  header.dataLocation = sizeof(header);
  header.dataSize = 0x10000000;
  header.dataFormat = SND_FORMAT_LINEAR_16;
  header.samplingRate = fts_dsp_get_sample_rate();
  header.channelCount = 1;
  header.info = 0x0;

  if (fwrite((char *)&header, 1, sizeof(header), fd) < (int)sizeof(header)){
    post("table~: write error in header of file: %s\n", file_name);
    fts_file_close(fd);
    return;
  }

  for(buf_ptr = this->buf.samples, samps_to_write = this->buf.size + GUARDPTS; samps_to_write > 0;){
    int writehere = (samps_to_write > TEMPBUFSAMPS ? TEMPBUFSAMPS : samps_to_write), n;
    char *rats;
    for(n = writehere, rats = tempbuf; n--; rats += sizeof(filesamp_t)){
      int printme = (bufno == 24 && n == writehere - 97);
      float foobar;
      int fixsamp = 32767.0f * (foobar = *(buf_ptr++));
      short fixshort;
      

      if (fixsamp > 32767) fixsamp = 32767;
      else if (fixsamp < -32767) fixsamp = -32767;
      fixshort = fixsamp;
      
      *(filesamp_t *)rats = fixshort;

      /* if (bufno == 24 && n == writehere - 97) */
      if (printme){
        short f2;
        char *shit = (char *)(&f2);
        shit[0] = rats[1];
        shit[1] = rats[0];
      }
    }
    if (fwrite(tempbuf, 1, writehere*sizeof(filesamp_t), fd) < (int)(writehere * sizeof(filesamp_t))){
      post("table~: error writing file: %s\n",  file_name);
      break;
    }
    samps_to_write -= writehere;
    bufno++;
  }
  fts_file_close(fd);
  post("table~: wrote file: %s\n",  file_name);
}

static void
sigtable_load(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  float onset = fts_get_float_arg(ac, at, 1, 0.0f);
  float sr = fts_get_float_arg(ac, at, 2, 0.0f);
  fts_symbol_t format = fts_get_symbol_arg(ac, at, 3, 0);
  int size = this->buf.size;
  float *buf = this->buf.samples;
  int n_onset;

  if(!this->name)
    return;

  if(onset > 0)
    n_onset = samples_unit_convert(this->unit, onset, fts_dsp_get_sample_rate());
  else
    n_onset = 0;

  if(file_name)
    {
      /* FIXME: sample rate conversion!!! */
      fts_audiofile_t* af = fts_audiofile_open_read(file_name);
	
      if (!fts_audiofile_valid(af)) 
	{
	  post("table~: %s: can't load samples from file \"%s\"\n", this->name, file_name);
	  return;
	}

      if (fts_audiofile_seek(af, n_onset) != 0) 
	{
	  post("table~: %s: can't seek position in file \"%s\"\n", this->name, file_name);
	  fts_audiofile_delete(af);
	  return;
	}

      if (fts_audiofile_valid(af))
	{
	  int n_samples = fts_audiofile_read(af, &buf, 1, size);
	
	  fts_audiofile_delete(af);
	
	  if(n_samples > 0)
	    fts_outlet_int(o, 0, n_samples);
	  else
	    post("table~: %s: can't load samples from file \"%s\"\n", this->name, file_name);      
	}
      else
	post("table~: %s: can't open soundfile to read \"%s\"\n", this->name, file_name);	
    }
}

static void
sigtable_save(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  float save_size = fts_get_float_arg(ac, at, 1, 0.0f);
  float sr = fts_get_float_arg(ac, at, 2, 0.0f);
  fts_symbol_t format = fts_get_symbol_arg(ac, at, 3, 0);
  int size = this->buf.size;
  float *buf = this->buf.samples;
  int n_save;

  if(!this->name)
    return;

  if(sr <= 0.0f)
    sr = fts_dsp_get_sample_rate();

  if(save_size > 0)
    n_save = samples_unit_convert(this->unit, save_size, sr);
  else
    n_save = size;

  if(n_save > size)
    n_save = size;

  if(file_name)
    {
      fts_audiofile_t *af = fts_audiofile_open_write(file_name, sr, 1, format);
      
      if (fts_audiofile_valid(af))
	{
	  int n_samples = fts_audiofile_write(af, &buf, 1, n_save);
	  fts_audiofile_delete(af);
	  
	  if(n_samples <= 0)
	    post("table~: %s: can't save samples to file \"%s\"\n", this->name , file_name);
	}
      else
	post("table~: %s: can't open soundfile to write \"%s\"\n", this->name, file_name);
    }
}

static void
sigtable_clear(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  int i;

  if(!this->name)
    return;  

  sampbuf_zero(&this->buf);
}

static void
sigtable_realloc(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  float size = fts_get_float_arg(ac, at, 0, 0.0f);

  if(this->name && size > 0)
    {
      int n_samps = samples_unit_convert(this->unit, size, fts_dsp_get_sample_rate());
      
      sampbuf_realloc(&this->buf, n_samps);
      
      this->check_size = size;
    }
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
sigtable_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigtable_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigtable_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigtable_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, put_dsp_check_size);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("read"), sigtable_read);
  fts_method_define_varargs(cl, 0, fts_new_symbol("write"), sigtable_write);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("load"), sigtable_load);
  fts_method_define_varargs(cl, 0, fts_new_symbol("save"), sigtable_save);

  fts_method_define_varargs(cl, 0, fts_new_symbol("realloc"), sigtable_realloc);

  return fts_Success;
}

void
sigtable_config(void)
{
  fts_class_install(fts_new_symbol("table~"),sigtable_instantiate);
}
