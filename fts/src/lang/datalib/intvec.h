#ifndef _INTVEC_H_
#define _INTVEC_H_


struct fts_integer_vector;
typedef struct fts_integer_vector fts_integer_vector_t;


extern fts_integer_vector_t *fts_integer_vector_new(int size);
extern void fts_integer_vector_delete(fts_integer_vector_t *this);

extern int fts_integer_vector_get_size(fts_integer_vector_t *x);
extern int fts_integer_vector_get_sum(fts_integer_vector_t *tw, int min, int max);
extern int fts_integer_vector_get_value(fts_integer_vector_t *tw, int idx);
extern int fts_integer_vector_get_min_value(fts_integer_vector_t *tw);
extern int fts_integer_vector_get_max_value(fts_integer_vector_t *tw);
extern int fts_integer_vector_get_quantile(fts_integer_vector_t *x, int n);
extern int fts_integer_vector_get_inv(fts_integer_vector_t *x, int n);

extern void fts_integer_vector_set_value(fts_integer_vector_t *x, int n1, int n2);
extern void fts_integer_vector_save_bmax(fts_integer_vector_t *this, fts_bmax_file_t *f);
extern void fts_integer_vector_set_const(fts_integer_vector_t *x, int n);
extern void fts_integer_vector_set_size(fts_integer_vector_t *x, int n);
extern void fts_integer_vector_set(fts_integer_vector_t *x, int onset, int ac, const fts_atom_t *at);



#endif
