/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#ifndef FTS_SAMPLE_CONVERSION_ROUTINES_H
#include "sample_conversions.h"
#endif

#define CLIP( val, min, max )  { val = ((val) < (min)) ? min : (((val) < (max)) ? (max) : (val)); }

const float fScaler8  = (float)0x7FL;                  /* (2^7  - 1 = 127)        */
const float fScaler16 = (float)0x7FFFL;                /* (2^15 - 1 = 32767)      */
const float fScaler24 = (float)0x7FFFFFL;              /* (2^23 - 1 = 8388607)    */
const float fScaler32 = (float)0x7FFFFFFFL;            /* (2^31 - 1 = 2147483647) */ 

const float fIScaler8  = (float)(1.0f / 0x7FL);        /* 1 / fScaler8  */
const float fIScaler16 = (float)(1.0f / 0x7FFFL);      /* 1 / fScaler16 */
const float fIScaler24 = (float)(1.0f / 0x7FFFFFL);    /* 1 / fScaler24 */
const float fIScaler32 = (float)(1.0f / 0x7FFFFFFFL);  /* 1 / fScaler32 */





/* One channel, Floating point to Integer */


/*****************************************************************************/
void fts_convert_float32_to_uint8(long size,float* src,char* dest)
{
	while(--size >= 0)
		*dest++ = (unsigned char)(128 + ((*src++) * fScaler8));
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_uint8clip(long size,float* src,char* dest)
{
  float tmp;
	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f); 
		*dest++ = (unsigned char)(128 + tmp * fScaler8); /* { 0, +255 } */
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int8(long size,float* src,char* dest)
{
	while(--size >= 0)
		*dest++ = (char)((*src++) * fScaler8);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int8clip(long size,float* src,char* dest)
{
  float tmp;
	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f); 
		*dest++ = (char)(tmp * fScaler8); /* { -128, +127 } */
  } 
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int16(long size,float* src,short* dest)
{
	while(--size >= 0)
		*dest++ = (short)((*src++) * fScaler16);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int16clip(long size,float* src,short* dest)
{
  float tmp;
	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f); 
		*dest++ = (short)(tmp * fScaler16); /* { -32768, +32767 } */
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24P(long size,float* src,char* dest)
{
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
		l = (long)((*src++) * fScaler24);
    
    dest[0] = pl[0];
    dest[1] = pl[1];
    dest[2] = pl[2];

    dest += 3;
  }
 
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24Pclip(long size,float* src,char* dest)
{
  float tmp;
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f);
		l = (long)(tmp * fScaler24);
    
    dest[0] = pl[0];
    dest[1] = pl[1];
    dest[2] = pl[2];

    dest += 3;
  }
 
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24L(long size,float* src,long* dest)
{
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
		l = (long)((*src++) * fScaler24);

    dest[0] = 0;
    dest[1] = pl[0];
    dest[2] = pl[1];
    dest[3] = pl[2];

    dest += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24Lclip(long size,float* src,long* dest)
{
  float tmp;
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f);
		l = (long)(tmp * fScaler24);

    dest[0] = 0;
    dest[1] = pl[0];
    dest[2] = pl[1];
    dest[3] = pl[2];

    dest += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24R(long size,float* src,long* dest)
{
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
		l = (long)((*src++) * fScaler24);

    dest[0] = pl[0];
    dest[1] = pl[1];
    dest[2] = pl[2];
    if(pl[3])
       dest[3] = 0xFF;
    else
       dest[3] = 0;

    dest += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int24Rclip(long size,float* src,long* dest)
{ 
  float tmp;
  long l;
  char* pl = (char*)&l;

	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f);
		l = (long)(tmp * fScaler24);

    dest[0] = pl[0];
    dest[1] = pl[1];
    dest[2] = pl[2];
    if(pl[3])
       dest[3] = 0xFF;
    else
       dest[3] = 0;

    dest += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int32(long size,float* src,long* dest)
{
	while(--size >= 0)
		*dest++ = (long)((*src++) * fScaler32);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_float32_to_int32clip(long size,float* src,long* dest)
{
  float tmp;

	while(--size >= 0)
  {
    tmp = *src++;
    CLIP(tmp, -1.0f, 1.0f);
		*dest++ = (long)(tmp * fScaler32);
  }
}
/*****************************************************************************/


/* One channel, Integer to Floating point */


/*****************************************************************************/
void fts_convert_int8_to_float32(long size,char* src,float* dest)
{
	while(--size >= 0)
    *dest++ = (float)((*src++) * fIScaler8);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_uint8_to_float32(long size,char* src,float* dest)
{
	while(--size >= 0)
    *dest++ = (float)(((*src++) - 128) * fIScaler8);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_int16_to_float32(long size,short* src,float* dest)
{
	while(--size >= 0)
    *dest++ = (float)((*src++) * fIScaler16);
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_int24P_to_float32(long size,char* src,float* dest)
{
  char sc[4];
  long l;

  sc[3] = 0;
 
	while(--size >= 0)
  {
    sc[0] = src[0];
    sc[1] = src[1];
    sc[2] = src[2];

    l = *(long*)(sc);
    *dest++ = (float)(l * fIScaler24);
    src += 3;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_int24L_to_float32(long size,long* src,float* dest)
{
  char sc[4];
  long l; 
  char* csrc = (char*)src;
  sc[3] = 0;

	while(--size >= 0)
  {
    sc[0] = csrc[1];
    sc[1] = csrc[2];
    sc[2] = csrc[3];

    l = *((long*)(sc));
    *dest++ = (float)(l * fIScaler24);
    src += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_int24R_to_float32(long size,long* src,float* dest)
{
  char sc[4];
  long l; 
  char* csrc = (char*)src;
  sc[3] = 0;

	while(--size >= 0)
  {
    sc[0] = csrc[0];
    sc[1] = csrc[1];
    sc[2] = csrc[2];

    l = *(long*)(sc);
    *dest++ = (float)(l * fIScaler24);
    src += 4;
  }
}
/*****************************************************************************/



/*****************************************************************************/
void fts_convert_int32_to_float32(long size,long* src,float* dest)
{
	while(--size >= 0)
    *dest++ = (float)((*src++) * fIScaler32);
}
/*****************************************************************************/



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/