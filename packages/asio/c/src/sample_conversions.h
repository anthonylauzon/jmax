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

/* Sample conversion routines prototypes */

/* 
   Convention for 24bits sample formats : 

    - int24P : 24bits signed int packed format               "SXXXXXXXXXXXXXXXXXXXXXXX"
    - int24L : 24bits signed int with padding (32bits total) "SXXXXXXXXXXXXXXXXXXXXXXX00000000"
    - int24R : 24bits signed int with padding (32bits total) "SSSSSSSSSXXXXXXXXXXXXXXXXXXXXXXX"

*/

#ifndef FTS_SAMPLE_CONVERSION_ROUTINES_H
#define FTS_SAMPLE_CONVERSION_ROUTINES_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* One channel, Floating point to Integer */
void fts_convert_float32_to_uint8(long size,float* src,char* dest);
void fts_convert_float32_to_int8(long size,float* src,char* dest);
void fts_convert_float32_to_int16(long size,float* src,short* dest);
void fts_convert_float32_to_int24P(long size,float* src,char* dest);
void fts_convert_float32_to_int24L(long size,float* src,long* dest);
void fts_convert_float32_to_int24R(long size,float* src,long* dest);
void fts_convert_float32_to_int32(long size,float* src,long* dest);

/* One channel, Floating point to cliped Integer */
void fts_convert_float32_to_uint8clip(long size,float* src,char* dest);
void fts_convert_float32_to_int8clip(long size,float* src,char* dest);
void fts_convert_float32_to_int16clip(long size,float* src,short* dest);
void fts_convert_float32_to_int24Pclip(long size,float* src,char* dest);
void fts_convert_float32_to_int24Lclip(long size,float* src,long* dest);
void fts_convert_float32_to_int24Rclip(long size,float* src,long* dest);
void fts_convert_float32_to_int32clip(long size,float* src,long* dest);

/* One channel, Integer to Floating point */
void fts_convert_uint8_to_float32(long size,char* src,float* dest);
void fts_convert_int8_to_float32(long size,char* src,float* dest);
void fts_convert_int16_to_float32(long size,short* src,float* dest);
void fts_convert_int24P_to_float32(long size,char* src,float* dest);
void fts_convert_int24L_to_float32(long size,long* src,float* dest);
void fts_convert_int24R_to_float32(long size,long* src,float* dest);
void fts_convert_int32_to_float32(long size,long* src,float* dest);

#ifdef __cplusplus
}
#endif

#endif