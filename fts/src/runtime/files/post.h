#ifndef _POST_H_
#define _POST_H_

/* definition file for  logging functions (post and co) */

extern void postvector(int n, float *fp);
extern void postatoms(int ac, const fts_atom_t *at);
extern void post(const char *format , ...); 

extern void post_error(fts_status_t error);
extern void post_error_noclobber(fts_status_t error);

#endif



