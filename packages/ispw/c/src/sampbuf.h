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
 */

#ifndef _SAMPBUF_H_
#define _SAMPBUF_H_

#define GUARDPTS 4 /* four points more for interpolation */

typedef struct{
  float *samples; /* a floating-point vector of samples */
  long size; /* # of samples (excluding GUARDPTS) */
  long alloc_size; /* allocated size (# of samples excluding GUARDPTS) */
} sampbuf_t;

typedef short filesamp_t; /* type of a sample in a soundfile */

/* names and hash tables */
extern sampbuf_t *sampbuf_get(fts_symbol_t name); /* get buffer by name */
extern int sampbuf_name_already_registed(fts_symbol_t name); /* non zero if name is already a sambuf */
extern void sampbuf_add(fts_symbol_t name, sampbuf_t *buf); /* register a new sampbuf */
extern void sampbuf_remove(fts_symbol_t name); /* remove sampbuf from register list */

/* data */
extern void sampbuf_init(sampbuf_t *buf, long size);
extern void sampbuf_realloc(sampbuf_t *buf, long size);
extern void sampbuf_erase(sampbuf_t *buf);
extern void sampbuf_zero(sampbuf_t *buf);

/* (*&^#(*&^%#_^_#$%_*&%($ */
extern void gensampname(char *buf, const char *base, long int n);


extern int sampbuf_name_already_registered(fts_symbol_t name);

#endif /* _SAMPBUF_H_ */
