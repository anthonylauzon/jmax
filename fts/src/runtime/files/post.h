#ifndef _POST_H_
#define _POST_H_

/* definition file for  logging functions (post and co) */

extern void post_vector(int n, float *fp);
extern void post_atoms(int ac, const fts_atom_t *at);
extern void post(const char *format , ...); 

extern void post_error(fts_object_t *obj, const char *format , ...); 


#endif



