#ifndef _SAMPBUF_H_
#define _SAMPBUF_H_

#define GUARDPTS 4 /* four points more for interpolation */

typedef struct{
  fts_symbol_t name;
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
