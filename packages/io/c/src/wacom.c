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

#include "fts.h"

#define WACOM_REQUEST_TIMEOUT 500
#define WACOM_INIT_TRIALS 10
#define WACOM_ANSWER_SIZE 64

enum wacom_outlets 
{
  wacom_outlet_x = 0, 
  wacom_outlet_y, 
  wacom_outlet_z, 
  wacom_outlet_tilt_x, 
  wacom_outlet_tilt_y, 
  wacom_outlet_b1, 
  wacom_outlet_b2, 
  wacom_outlet_b3, 
  wacom_outlet_b4,
  wacom_outlet_init,
  wacom_n_outlets
};

typedef struct _wacom_
{
  fts_object_t o;
  fts_bytestream_t *stream;

  /* requests to tablet */
  unsigned char answer_string[WACOM_ANSWER_SIZE + 1];
  int answer_index;
  int init_trials;
  fts_alarm_t alarm;

  /* tablet parameters */
  float version;
  int resolution_x;
  int resolution_y;
  int range_x;
  int range_y;

  /* state */
  unsigned char bytes[9];
  int index;
  int x;
  int y;
  int z;
  int tilt_x;
  int tilt_y;
  int b1;
  int b2;
  int b3;
  int b4;
} wacom_t;

/************************************************************
 *
 *  wacom requests & settings
 *
 */

static const unsigned char wacom_string_reset[] = "#\r";
static const unsigned char wacom_string_request_model[] = "~#\r";
static const unsigned char wacom_string_request_resolution[] = "~R\r";
static const unsigned char wacom_string_request_coordinates[] = "~C\r";

static const unsigned char wacom_string_enable_tilt[] = "FM1\r"; /* enable extra protocol for tilt management */

#define WACOM_STRING_MULTI "MU1\r" /* multi mode input */
#define WACOM_STRING_UPPER_ORIGIN "OC1\r" /* origin in upper left */
#define WACOM_STRING_SUPPRESS "SU" /* suppress mode */
#define WACOM_STRING_ALL_MACRO "~M0\r" /* enable all macro buttons */
#define WACOM_STRING_NO_MACRO1 "~M1\r" /* disable macro buttons of group 1 */
#define WACOM_STRING_RATE "IT0\r" /* max transmit rate (unit of 5 ms) */
#define WACOM_STRING_NO_INCREMENT "IN0\r" /* do not enable increment mode */
#define WACOM_STRING_STREAM_MODE "SR\r" /* enable continuous mode */
#define WACOM_STRING_PRESSURE_MODE "PH1\r" /* enable pressure mode */
#define WACOM_STRING_START "ST\r" /* start sending coordinates */

static const unsigned char wacom_string_setup[] = \
WACOM_STRING_MULTI \
WACOM_STRING_UPPER_ORIGIN \
WACOM_STRING_ALL_MACRO \
WACOM_STRING_NO_MACRO1 \
WACOM_STRING_RATE \
WACOM_STRING_NO_INCREMENT \
WACOM_STRING_STREAM_MODE;

#define WACOM_SYNC_BIT 0x80
#define WACOM_BUTTON_PRESSSED 0x08

/*************************************************************************
 *
 *  wacom decoding
 *
 */

static void
wacom_decode(fts_object_t *o, int n, const unsigned char *c)
{
  wacom_t *this = (wacom_t *)o;
  int index = this->index;
  int x, y, z;
  int tilt_x = 0;
  int tilt_y = 0;
  int b1, b2, b3, b4;
  int i;

  for(i=0; i<n; i++) 
    { 
      /* reset char count on sync bit set */
      if(c[i] & WACOM_SYNC_BIT)
	{
	  if(index >= 7)
	    {
	      x = (((this->bytes[0] & 0x3) << 14) + (this->bytes[1] << 7) + this->bytes[2]);
	      y = (((this->bytes[3] & 0x3) << 14) + (this->bytes[4] << 7) + this->bytes[5]);
	      z = ((this->bytes[6] & 0x3F) + 0x40 - (this->bytes[6] & 0x40)) * 2 + ((this->bytes[3] & 0x04) >> 2);
	      
	      if(this->bytes[0] & WACOM_BUTTON_PRESSSED)
		{
		  b1 = (this->bytes[3] & 0x08) >> 3;
		  b2 = (this->bytes[3] & 0x10) >> 4;
		  b3 = (this->bytes[3] & 0x20) >> 5;
		  b4 = (this->bytes[3] & 0x40) >> 6;
		}
	      else
		b1 = b2 = b3 = b4 = 0;
	      
	      if(index >= 8)
		{
		  tilt_x = (this->bytes[7] & 0x7f);
		  tilt_y = (this->bytes[8] & 0x7f);
		  
		  if (this->bytes[7] & 0x40)
		    tilt_x = -(~(tilt_x-1) & 0x7f);
		  if (this->bytes[8] & 0x40)
		    tilt_y = -(~(tilt_y-1) & 0x7f);
		}
	      else
		{
		  tilt_x = 0;
		  tilt_y = 0;
		}

	      if(b4 != this->b4)
		{
		  this->b4 = b4;
		  fts_outlet_int(o, wacom_outlet_b4, b4);
		}

	      if(b3 != this->b3)
		{
		  this->b3 = b3;
		  fts_outlet_int(o, wacom_outlet_b3, b3);
		}

	      if(b2 != this->b2)
		{
		  this->b2 = b2;
		  fts_outlet_int(o, wacom_outlet_b2, b2);
		}

	      if(b1 != this->b1)
		{
		  this->b1 = b1;
		  fts_outlet_int(o, wacom_outlet_b1, b1);
		}

	      if(tilt_y != this->tilt_y)
		{
		  this->tilt_y = tilt_y;
		  fts_outlet_int(o, wacom_outlet_tilt_y, tilt_y);
		}

	      if(tilt_x != this->tilt_x)
		{
		  this->tilt_x = tilt_x;
		  fts_outlet_int(o, wacom_outlet_tilt_x, tilt_x);
		}

	      if(z != this->z)
		{
		  this->z = z;
		  fts_outlet_int(o, wacom_outlet_z, z);
		}

	      if(y != this->y)
		{
		  this->y = y;
		  fts_outlet_int(o, wacom_outlet_y, y);
		}

	      if(x != this->x)
		{
		  this->x = x;
		  fts_outlet_int(o, wacom_outlet_x, x);
		}
	    }

	  this->bytes[0] = c[i];	
	  index = 1;
	}
      else if(index < 9)
	{
	  this->bytes[index] = c[i];	
	  index++;
	}
    }

  this->index = index;
}

static void
wacom_reset_buttons(fts_alarm_t *alarm, void *p)
{
  fts_object_t *o = (fts_object_t *)p;
  wacom_t *this = (wacom_t *)p;

  if(this->b4 != 0)
    {
      this->b4 = 0;
      fts_outlet_int(o, wacom_outlet_b4, 0);
    }

  if(this->b3 != 0)
    {
      this->b3 = 0;
      fts_outlet_int(o, wacom_outlet_b3, 0);
    }

  if(this->b2 != 0)
    {
      this->b2 = 0;
      fts_outlet_int(o, wacom_outlet_b2, 0);
    }

  if(this->b1 != 0)
    {
      this->b1 = 0;
      fts_outlet_int(o, wacom_outlet_b1, 0);
    }
}

/*************************************************************************
 *
 *  init procedure
 *
 */

static void
wacom_init_stop(wacom_t *this)
{
  fts_alarm_unarm(&this->alarm);
  fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
}

static void
wacom_init_send_string(wacom_t *this, const unsigned char *string)
{
  fts_bytestream_output(this->stream, strlen(string), string);
  fts_bytestream_flush(this->stream);
}

static void
wacom_init_callback(fts_object_t *o, int n, const unsigned char *c)
{
  wacom_t *this = (wacom_t *)o;
  unsigned char *string = this->answer_string;
  int index = this->answer_index;
  int i;

  for(i=0; i<n; i++)
    {
      if(c[i] == '~')
	{
	  string[0] = '~';
	  index = 1;
	}
      else if(c[i] == '\r')
	{
	  int size = index;

	  string[index] = '\0';
      
	  if(string[0] == '~')
	    {
	      switch(string[1])
		{
		case '#':
		  {
		    unsigned char *version_string;
		    int n_scan = 0;
		    float version;
		    int j;

		    post("wacom tablet detected: %s\n", string + 2);
		    
		    /* extract version number (find 'V' starting from end) */
		    for(j=size; j>=2; j--)
		      if(string[j] == 'V') 
			break;

		    version_string = string + j + 1;

		    for(; j<size; j++)
		      if(string[j] == '-') 
			break;

		    string[j] = '\0';
		    n_scan = sscanf(version_string, "%f", &version);

		    if(n_scan == 1)
		      this->version = version;
		    else
		      {
			this->version = 0.0;
			post("wacom: init failed (invalid version string)\n");
			wacom_init_stop(this);
		      }

		    wacom_init_send_string(this, wacom_string_request_resolution);

		    break;
		  }
		case 'R':
		  {
		    unsigned char *resolution_string;
		    int n_scan;
		    int x, y;
		    int j = size;

		    /* extract version number (count two ',' starting from end */
		    while(j >= 1 && string[j] != ',')
		      j--;

		    j--;

		    while(j >= 1 && string[j] != ',')
		      j--;

		    resolution_string = string + j + 1;

		    n_scan = sscanf(resolution_string, "%d,%d", &x, &y);

		    if(n_scan == 2)
		      {
			this->resolution_x = x;
			this->resolution_y = y;
		      }
		    else
		      {
			this->resolution_x = 1270;
			this->resolution_y = 1270;
			post("wacom: init failed (invalid resolution string)\n");

			wacom_init_stop(this);
		      }

		    wacom_init_send_string(this, wacom_string_request_coordinates);

		    break;
		  }
		case 'C':
		  {
		    unsigned char *maxima_string = string + 2;
		    fts_atom_t a[4];
		    int n_scan;
		    int x, y;

		    wacom_init_stop(this);

		    n_scan = sscanf(maxima_string, "%d,%d", &x, &y);
		    
		    if(n_scan == 2)
		      {
			this->range_x = x;
			this->range_y = y;
		      }
		    else
		      {
			this->range_x = 15240;
			this->range_y = 15240;
			post("wacom: init failed (invalid coordinates string)\n");
		      }

		    wacom_init_send_string(this, wacom_string_setup);

		    /* set tilt mode if for tablets from version 1.4 */
		    if(this->version >= (float)1.4)
		      wacom_init_send_string(this, wacom_string_enable_tilt);

		    fts_set_int(a + 0, this->range_x);
		    fts_set_int(a + 1, this->range_y);
		    fts_set_int(a + 2, this->resolution_x);
		    fts_set_int(a + 3, this->resolution_y);
		    fts_outlet_send(o, wacom_outlet_init, fts_s_list, 4, a);
		    
		    /* install wacom decode callback */
		    fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
		    fts_bytestream_add_listener(this->stream, (fts_object_t *)this, wacom_decode);

		    break;
		  }
		  default:
		}
	    }
	}
      else if(index < WACOM_ANSWER_SIZE)
	{
	  string[index] = c[i];
	  index++;
	}
    }

  this->answer_index = index;
}

static void
wacom_init_start(wacom_t *this)
{
  this->answer_index = 0;

  /* set alarm for init timeout */
  fts_alarm_set_delay(&this->alarm, WACOM_REQUEST_TIMEOUT);
  fts_alarm_arm(&this->alarm);

  /* set byte stream callback for init procedure */
  fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
  fts_bytestream_add_listener(this->stream, (fts_object_t *)this, wacom_init_callback);

  wacom_init_send_string(this, wacom_string_reset);
  wacom_init_send_string(this, wacom_string_request_model);

  /* all the rest is handled by wacom_init_callback() */
}

static void
wacom_init_timeout(fts_alarm_t *alarm, void *o)
{
  wacom_t *this = (wacom_t *)o;

  wacom_init_stop(this);

  if(this->init_trials-- > 0)
    wacom_init_start(this);
  else
    post("wacom: couldn't init device (request timeout)\n");
}

/************************************************************
 *
 *  methods
 *
 */

static void
wacom_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wacom_t *this = (wacom_t *)o;

  wacom_init_stop(this);

  this->init_trials = WACOM_INIT_TRIALS;
  wacom_init_start(this);
}

/************************************************************
 *
 *  class
 *
 */

static void
wacom_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wacom_t *this = (wacom_t *)o;

  this->stream = (fts_bytestream_t *)fts_get_object(at + 1);

  this->index = 0;

  this->x = 0;
  this->y = 0;
  this->z = 0;
  this->tilt_x = 0;
  this->tilt_y = 0;

  this->b1 = 0;
  this->b2 = 0;
  this->b3 = 0;
  this->b4 = 0;

  fts_alarm_init(&this->alarm, 0, wacom_init_timeout, (void *)this);
  this->init_trials = WACOM_INIT_TRIALS;
  wacom_init_start(this);
}

static void
wacom_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wacom_t *this = (wacom_t *)o;

  fts_alarm_unarm(&this->alarm);
  fts_bytestream_remove_listener(this->stream, o);
}

static int 
wacom_check(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1))
    {
      fts_object_t *obj = fts_get_object(at + 1);

      if(fts_bytestream_has_superclass(obj) && fts_bytestream_is_output((fts_bytestream_t *)obj) && fts_bytestream_is_input((fts_bytestream_t *)obj))
	return 1;
    }
  
  return 0;
}

static fts_status_t
wacom_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(wacom_check(ac, at))
    {
      fts_class_init(cl, sizeof(wacom_t), 1, wacom_n_outlets, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, wacom_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, wacom_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), wacom_reset);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int 
wacom_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  return wacom_check(ac1, at1);
}

void
wacom_config(void)
{
  fts_metaclass_install(fts_new_symbol("wacom"), wacom_instantiate, wacom_equiv);
}
