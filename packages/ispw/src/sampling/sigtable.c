#include "fts.h"

#include "sampbuf.h"

#include <string.h>

#define CLASS_NAME "table~"


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
#define TEMPBUFSAMPS (TEMPBUFSIZE/sizeof(filesamp_t))

static fts_symbol_t fts_s_read, fts_s_write;

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
  fts_symbol_t volume; /* volume (i.e. directory) */
} sigtable_t;


static void
sigtable_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);
  float size = (unit)? (float)fts_get_number_arg(ac, at, 3, 0.0f): (float)fts_get_number_arg(ac, at, 2, 0.0f);
  long n_samps;
  float *samp_buf;
  float sr;

  this->name = 0;

  if (!unit)
    unit = fts_s_msec; /* default */
  
  if (sampbuf_name_already_registered(name))
    {
      post("%s: %s: multiply defined (last ignored)\n", CLASS_NAME, fts_symbol_name(name));
      return;
    }
  else
    sampbuf_add(name, (void *)&(this->buf));
  
  if (size < 0)
    size = 0;

  sr = fts_param_get_float(fts_s_sampling_rate, 44100.);
  n_samps = fts_unit_convert_to_base(unit, size, &sr);
  
  sampbuf_init(&this->buf, n_samps);

  this->name = name;

  if (this->name)
    fts_register_named_object(o, this->name);

  this->check_size = size;
  this->unit = unit;
  this->volume = fts_get_default_directory();

  dsp_list_insert(o);
}

static void
sigtable_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;

  if (this->name)
    fts_unregister_named_object(o, this->name);

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
    post("warning: table~: %s: dead object\n", CLASS_NAME);
  else
    {
      float sr;
      long n_samps;

      sr = fts_param_get_float(fts_s_sampling_rate, 44100.);
      n_samps = fts_unit_convert_to_base(this->unit, this->check_size, &sr);

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
  long int onset = fts_get_long_arg(ac, at, 1, 0);
  long size = this->buf.size;
  float *buf_ptr = this->buf.samples;
  long samps_left, samps_to_read, samps_read;
  char tempbuf[TEMPBUFSIZE+320];
  int fd;

  if (!this->name) return;

  if ((fd = fts_file_open(fts_symbol_name(file_name), this->volume, "r")) < 0){
    post("%s: %s: can't open\n", CLASS_NAME, fts_symbol_name(file_name));
    return;
  }

  if (lseek(fd, onset, 0) < 0){
    post("%s: %s: can't seek to beginning\n", CLASS_NAME, fts_symbol_name(file_name));
    fts_file_close(fd);
    return;
  }

  samps_left = samps_to_read = size + GUARDPTS;
  while(samps_left > 0){
    long gimme = samps_left, n_bytes, bytes_read;
    char *rats;
    if (gimme > TEMPBUFSAMPS) gimme = TEMPBUFSAMPS;

    bytes_read = read(fd, tempbuf, gimme * sizeof(filesamp_t));
    if (bytes_read & 1){
      post("warning: dropping odd byte\n");
      bytes_read &= ~1;
    }

    if (bytes_read < 0) post("%s: error reading file: %s\n", CLASS_NAME, fts_symbol_name(file_name));
    if (bytes_read <= 0) break;
    
    for(n_bytes = bytes_read / sizeof(filesamp_t), rats = tempbuf; n_bytes--; rats += sizeof(filesamp_t)){

      *(buf_ptr++) = (1.0f/32767.0f) * *(filesamp_t *)rats;
    }
    samps_left -= bytes_read / sizeof(filesamp_t);
  }
  
  samps_read = samps_to_read - samps_left;
  if (samps_read > size) samps_read = size;

  post("%s: read %d samples (out of %d in table) from file: %s\n",
    CLASS_NAME, samps_read, size, fts_symbol_name(file_name)
  );
  
  while(samps_left-- > 0) *buf_ptr++ = 0.0f;

  fts_file_close(fd);
}

static void
sigtable_write(fts_object_t *o, int winlet, fts_symbol_t sym, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  int fd;
  long samps_to_write;
  char tempbuf[TEMPBUFSIZE];
  SNDSoundStruct header;
  float *buf_ptr;
  int bufno = 0;

  if (!this->name) return;

  /* fts_io_creat(fts_symbol_name(file_name), 0666)) */

  if ((fd = fts_file_open(fts_symbol_name(file_name), this->volume, "w")) < 0){
    post("%s: can't create file: %s\n", CLASS_NAME, fts_symbol_name(file_name));
    return;
  }
  header.magic = SND_MAGIC;
  header.dataLocation = sizeof(header);
  header.dataSize = 0x10000000;
  header.dataFormat = SND_FORMAT_LINEAR_16;
  header.samplingRate = (long)fts_param_get_float(fts_s_sampling_rate, 44100.);
  header.channelCount = 1;
  header.info = 0x0;

  if (write(fd, (char *)&header, sizeof(header)) < sizeof(header)){
    post("%s: write error in header of file: %s\n", CLASS_NAME, fts_symbol_name(file_name));
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
    if (write(fd, tempbuf, writehere*sizeof(filesamp_t)) < writehere*sizeof(filesamp_t)){
      post("%s: error writing file: %s\n", CLASS_NAME,  fts_symbol_name(file_name));
      break;
    }
    samps_to_write -= writehere;
    bufno++;
  }
  fts_file_close(fd);
  post("%s: wrote file: %s\n", CLASS_NAME,  fts_symbol_name(file_name));
}

static void
sigtable_clear(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  long i;

  if(!this->name) return;  

  sampbuf_zero(&this->buf);
}

static void
sigtable_realloc(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtable_t *this = (sigtable_t *)o;
  float size = (float)fts_get_number_arg(ac, at, 0, 0.0f);

  if(this->name && size > 0)
    {
      float sr = fts_param_get_float(fts_s_sampling_rate, 44100.);
      long n_samps = fts_unit_convert_to_base(this->unit, size, &sr);
      
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
  fts_symbol_t a[4];

  fts_class_init(cl, sizeof(sigtable_t), 1, 0, 0);

  a[0] = fts_s_symbol; /* class */
  a[1] = fts_s_symbol; /* name */
  a[2] = fts_s_anything; /* unit or size or no */
  a[3] = fts_s_number; /* size or no */
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigtable_init, 4, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigtable_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, put_dsp_check_size, 1, a);
  
  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_s_read, sigtable_read, 2, a, 1);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_write, sigtable_write, 1, a);
  
  a[1] = fts_s_number; /* size */
  fts_method_define(cl, 0, fts_new_symbol("realloc"), sigtable_realloc, 1, a);

  return fts_Success;
}

void
sigtable_config(void)
{
  fts_metaclass_create(fts_new_symbol(CLASS_NAME),sigtable_instantiate, fts_always_equiv);

  fts_s_read  = fts_new_symbol("read");
  fts_s_write = fts_new_symbol("write");
}
