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

#include <string.h>
#include <fts/fts.h>

#define WACOM_REQUEST_TIMEOUT 500
#define WACOM_WAIT_RESET 10
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
  wacom_outlet_fun,
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
  int initializing;

  /* tablet parameters */
  float version;
  int range_x;
  int range_y;
  int range_z;
  int range_tilt_x;
  int range_tilt_y;

  /* state */
  unsigned char bytes[9];
  int wacom;
  int index;
  int x;
  int y;
  int z;
  int tilt_x;
  int tilt_y;
  int b1;
  int b2;
  int b3;
  int fun;
} wacom_t;

/************************************************************
 *
 *  wacom requests & settings
 *
 */

static const unsigned char wacom_string_reset[] = "#\r";
static const unsigned char wacom_string_start[] = "ST\r";
static const unsigned char wacom_string_stop[] = "SP\r";
static const unsigned char wacom_string_request_model[] = "~#\r";
static const unsigned char wacom_string_request_coordinates[] = "~C\r";

static const unsigned char wacom_string_enable_tilt[] = "FM1\r"; /* enable tilt mode */

/* setup commands */
#define WACOM_STRING_ALWAYS_TRANSMIT "AL2\r" /* send 3 coordinate pairs when the pointing device leaves proximity */
#define WACOM_STRING_ABSOLUTE "DE0\r" /* absolute mode */
#define WACOM_STRING_INCREMENT_OFF "IN0\r" /* disable increment mode */
#define WACOM_STRING_RATE_MAX "IT0\r" /* set max transmit rate (unit of 5 ms) */
#define WACOM_STRING_MULTI_MODE_OFF "MU0\r" /* disable multi mode */
#define WACOM_STRING_ORIGIN_UPPER_LEFT "OC1\r" /* origin in upper left corner */
#define WACOM_STRING_STREAM_MODE_ON "SR\r" /* enable continuous mode */
#define WACOM_STRING_ALL_BUTTONS_ON "~M0\r" /* enable all macro buttons */

static const unsigned char wacom_string_setup[] = \
WACOM_STRING_ALWAYS_TRANSMIT \
WACOM_STRING_ABSOLUTE \
WACOM_STRING_INCREMENT_OFF \
WACOM_STRING_RATE_MAX \
WACOM_STRING_MULTI_MODE_OFF \
WACOM_STRING_ORIGIN_UPPER_LEFT \
WACOM_STRING_STREAM_MODE_ON \
WACOM_STRING_ALL_BUTTONS_ON \
;

/*************************************************************************
 *
 *  wacom decoding
 *
 */

#define WACOM_BUTTON 0x08
#define WACOM_POINTER 0x20
#define WACOM_PROXIMITY 0x40
#define WACOM_SYNC_BIT 0x80

static void
wacom_decode_IV(fts_object_t *o, int n, const unsigned char *c)
{
  wacom_t *this = (wacom_t *)o;
  int index = this->index;
  int i;
  
  for(i=0; i<n; i++) 
    { 
      /* interpret data on sync bit set (beginning of next frame) */
      if(c[i] & WACOM_SYNC_BIT)
	{
	  int proximity = this->bytes[0] & WACOM_PROXIMITY;
	  int button_pressed = this->bytes[0] & WACOM_BUTTON;
	  int is_stylus = this->bytes[0] & WACOM_POINTER;
	  
	  if(index < 7)
	    continue;
	  
	  if(!proximity && button_pressed)
	    {
	      /* function/macro button */
	      int button;
	      int fun;
	      
	      fun = this->bytes[6];
	      
	      if(fun != this->fun)
		{
		  this->fun = fun;
		  fts_outlet_int(o, wacom_outlet_fun, fun);
		}
	    }
	  else
	    {
	      /* coordinates */
	      int b1, b2, b3;
	      int x, y;
	      int z = 0;
	      int tilt_x = 0;
	      int tilt_y = 0;
	      
	      if(button_pressed)
		{
		  b1 = (this->bytes[3] & 0x08) >> 3;
		  b2 = (this->bytes[3] & 0x10) >> 4;
		  b3 = (this->bytes[3] & 0x20) >> 5;
		}
	      else
		b1 = b2 = b3 = 0;
	      
	      x = (((this->bytes[0] & 0x3) << 14) + (this->bytes[1] << 7) + this->bytes[2]);
	      y = (((this->bytes[3] & 0x3) << 14) + (this->bytes[4] << 7) + this->bytes[5]);
	      
	      if(is_stylus)
		{
		  z = ((this->bytes[6] & 0x3F) + 0x40 - (this->bytes[6] & 0x40)) * 2 + ((this->bytes[3] & 0x04) >> 2);

		  if(z == 128 && b1 == 0)
		    z = 0;

		  if(index >= 8)
		    {
		      tilt_x = (this->bytes[7] & 0x7f);
		      tilt_y = (this->bytes[8] & 0x7f);
		      
		      if (this->bytes[7] & 0x40)
			tilt_x = -(~(tilt_x - 1) & 0x7f);
		      if (this->bytes[8] & 0x40)
			tilt_y = -(~(tilt_y - 1) & 0x7f);
		    }
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

	  /* start new protocol frame */
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
wacom_decode_V(fts_object_t *o, int n, const unsigned char *c)
{
  wacom_t *this = (wacom_t *)o;
  int index = this->index;
  int i;
  
  for(i=0; i<n; i++) 
    { 
      /* interpret data on sync bit set (beginning of next frame) */
      if(c[i] & WACOM_SYNC_BIT)
	{
	  int proximity = this->bytes[0] & WACOM_PROXIMITY;
	  int button_pressed = this->bytes[0] & WACOM_BUTTON;
	  int is_stylus = this->bytes[0] & WACOM_POINTER;
	  int device;
	  
	  if(index < 7)
	    continue;
	  
	  if ((this->bytes[0] & 0xfc) == 0xc0) 
	    {
	      /* device ID packet */
	      device = (((this->bytes[1] & 0x7f) << 5) + ((this->bytes[2] & 0x7c) >> 2));
	    }
	  else if ((this->bytes[0] & 0xfe) == 0x80) 
	    {
	      /* out of proximity packet */
	    }
	  else if ((this->bytes[0] & 0xb8) == 0xa0)
	    {
	      /* stylus packet with button and pressure */
	      int x = (((this->bytes[1] & 0x7f) << 9) + ((this->bytes[2] & 0x7f) << 2) + ((this->bytes[3] & 0x60) >> 5));
	      int y = (((this->bytes[3] & 0x1f) << 11) + ((this->bytes[4] & 0x7f) << 4) + ((this->bytes[5] & 0x78) >> 3));
	      int z = (((this->bytes[5] & 0x07) << 7) + (this->bytes[6] & 0x7f));
	      int tilt_x = (this->bytes[7] & 0x3f);
	      int tilt_y = (this->bytes[8] & 0x3f);
	      int b1 = (z > 200);
	      int b2 = (this->bytes[0] & 0x02) >> 1;
	      
	      if (this->bytes[7] & 0x40)
		tilt_x -= (0x3f + 1);
	      
	      if (this->bytes[8] & 0x40)
		tilt_y -= (0x3f + 1);
	      
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
	  else if ((this->bytes[0] & 0xbe) == 0xb4)
	    {
	      /* stylus packet with wheel */
	      int x = (((this->bytes[1] & 0x7f) << 9) + ((this->bytes[2] & 0x7f) << 2) + ((this->bytes[3] & 0x60) >> 5));
	      int y = (((this->bytes[3] & 0x1f) << 11) + ((this->bytes[4] & 0x7f) << 4) + ((this->bytes[5] & 0x78) >> 3));
	      int tilt_x = (this->bytes[7] & 0x3f);
	      int tilt_y = (this->bytes[8] & 0x3f);
	      int b3 = (((this->bytes[5] & 0x07) << 7) + (this->bytes[6] & 0x7f));
	      
	      if (this->bytes[7] & 0x40)
		tilt_x -= (0x3f + 1);
	      
	      if (this->bytes[8] & 0x40)
		tilt_y -= (0x3f + 1);

	      if(b3 != this->b3)
		{
		  this->b3 = b3;
		  fts_outlet_float(o, wacom_outlet_b3, (float)b3 / (float)1023.0);
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
	  else if (((this->bytes[0] & 0xbe) == 0xa8) || ((this->bytes[0] & 0xbe) == 0xb0)) 
	    {
	      /* lens cursor packet */
	      int x = (((this->bytes[1] & 0x7f) << 9) + ((this->bytes[2] & 0x7f) << 2) + ((this->bytes[3] & 0x60) >> 5));
	      int y = (((this->bytes[3] & 0x1f) << 11) + ((this->bytes[4] & 0x7f) << 4) + ((this->bytes[5] & 0x78) >> 3));
	      int z = (((this->bytes[5] & 0x07) << 7) + (this->bytes[6] & 0x7f));
	      int b1 = this->bytes[8] & 0x01;
	      int b2 = (this->bytes[8] & 0x02) >> 1;
	      int b3 = (this->bytes[8] & 0x04) >> 2;
	      
	      if (this->bytes[8] & 0x08) 
		z = -z;
	      
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
	  
	  /* start new protocol frame */
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
wacom_reset_buttons(wacom_t *this)
{
  fts_object_t *o = (fts_object_t *)this;

  if(this->fun != 0)
    {
      this->fun = 0;
      fts_outlet_int(o, wacom_outlet_fun, 0);
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
  if(this->initializing)
    fts_timebase_remove_object(fts_get_timebase(), (fts_object_t *)this);

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
		    /* wacom sends version */
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

		    if(string[2] == 'G' && string[3] == 'D')
		      {
			/* Wacom V protocol (Intous) */
			this->wacom = 5;
			this->range_z = 1023;
		      }
		    else
		      {
			/* Wacom IV protocol */
			this->wacom = 4;
			this->range_z = 255;

			/* set tilt mode if for tablets from version 1.4 */
			if(this->version >= (float)1.4)
			  wacom_init_send_string(this, wacom_string_enable_tilt);
		      }

		    /* request coordinate range */
		    wacom_init_send_string(this, wacom_string_request_coordinates);

		    break;
		  }
		case 'R':
		  {
		    /* wacom sends resolution */
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

		    break;
		  }
		case 'C':
		  {
		    /* wacom sends coordinate range */
		    unsigned char *maxima_string = string + 2;
		    fts_atom_t a[5];
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

		    this->range_tilt_x = 64;
		    this->range_tilt_y = 64;

		    wacom_init_send_string(this, wacom_string_setup);

		    /* install wacom decode callback */
		    if(this->wacom == 4)
		      {
			fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
			fts_bytestream_add_listener(this->stream, (fts_object_t *)this, wacom_decode_IV);
		      }
		    else if(this->wacom == 5)
		      {
			fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
			fts_bytestream_add_listener(this->stream, (fts_object_t *)this, wacom_decode_V);
		      }		      

		    wacom_init_send_string(this, wacom_string_start);

		    fts_set_int(a + 0, this->range_x);
		    fts_set_int(a + 1, this->range_y);
		    fts_set_int(a + 2, this->range_z);
		    fts_set_int(a + 3, this->range_tilt_x);
		    fts_set_int(a + 4, this->range_tilt_y);
		    fts_outlet_send(o, wacom_outlet_init, fts_s_list, 5, a);
		    
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

static void wacom_init_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void wacom_init_timeout(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
wacom_init_start(wacom_t *this)
{
  this->answer_index = 0;

  /* set byte stream callback for init procedure */
  fts_bytestream_remove_listener(this->stream, (fts_object_t *)this);
  fts_bytestream_add_listener(this->stream, (fts_object_t *)this, wacom_init_callback);

  wacom_init_send_string(this, wacom_string_stop);
  wacom_init_send_string(this, wacom_string_reset);

  /* wait for reset */
  this->initializing = 1;

  fts_timebase_remove_object(fts_get_timebase(), (fts_object_t *)this);
  fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, wacom_init_send, 0, WACOM_WAIT_RESET);
}

static void
wacom_init_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wacom_t *this = (wacom_t *)o;

  /* now request model string */
  wacom_init_send_string(this, wacom_string_request_model);
  
  /* set init timeout (all the rest is handled by the init callback) */
  fts_timebase_add_call(fts_get_timebase(), o, wacom_init_timeout, 0, WACOM_REQUEST_TIMEOUT);
}

static void
wacom_init_timeout(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wacom_t *this = (wacom_t *)o;

  wacom_init_stop(this);
  
  if(this->init_trials-- > 0)
    wacom_init_start(this); /* retry */
  else
    post("wacom: can't init device (timeout after %d trials)\n", WACOM_INIT_TRIALS);
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

static int 
wacom_check(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1))
    {
      fts_object_t *obj = fts_get_object(at + 1);

      if(fts_bytestream_check(obj) && fts_bytestream_is_output((fts_bytestream_t *)obj) && fts_bytestream_is_input((fts_bytestream_t *)obj))
	return 1;
    }
  
  return 0;
}

static void
wacom_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wacom_t *this = (wacom_t *)o;

  ac--;
  at++;

  this->stream = 0;

  if(ac > 0 && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(fts_bytestream_check(obj) && fts_bytestream_is_output((fts_bytestream_t *)obj) && fts_bytestream_is_input((fts_bytestream_t *)obj))
	{
	  this->stream = (fts_bytestream_t *)obj;
	  
	  this->index = 0;
	  
	  this->wacom = 0;
	  
	  this->x = 0;
	  this->y = 0;
	  this->z = 0;
	  this->tilt_x = 0;
	  this->tilt_y = 0;
	  
	  this->b1 = 0;
	  this->b2 = 0;
	  this->b3 = 0;
	  this->fun = 0;

	  this->initializing = 0;
	  this->init_trials = 0;
	  
	  wacom_init_start(this);
	  
	  return;
	}
    }

  fts_object_set_error(o, "First argument of bidirectional bytestream required");
}

static void
wacom_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wacom_t *this = (wacom_t *)o;

  if(this->stream)
    fts_bytestream_remove_listener(this->stream, o);
}

static fts_status_t
wacom_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(wacom_t), 1, wacom_n_outlets, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, wacom_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, wacom_delete);

  fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), wacom_reset);
  
  return fts_Success;
}

void
wacom_config(void)
{
  fts_class_install(fts_new_symbol("wacom"), wacom_instantiate);
}
