#ifndef _INTVEC_H_
#define _INTVEC_H_


struct fts_intvec;

typedef struct fts_intvec fts_intvec_t;


extern fts_intvec_t *fts_intvec_new(int size);
extern void fts_intvec_delete(fts_intvec_t *this);

extern int fts_intvec_get_size(fts_intvec_t *x);
extern int fts_intvec_get_sum(fts_intvec_t *tw, int min, int max);
extern int fts_intvec_get_value(fts_intvec_t *tw, int idx);
extern int fts_intvec_get_min_value(fts_intvec_t *tw);
extern int fts_intvec_get_max_value(fts_intvec_t *tw);
extern int fts_intvec_get_quantile(fts_intvec_t *x, int n);
extern int fts_intvec_get_inv(fts_intvec_t *x, int n);

extern void fts_intvec_set_value(fts_intvec_t *x, int n1, int n2);
extern void fts_intvec_save_bmax(fts_intvec_t *this, fts_bmax_file_t *f);
extern void fts_intvec_set_const(fts_intvec_t *x, int n);
extern void fts_intvec_set_size(fts_intvec_t *x, int n);
extern void fts_intvec_set(fts_intvec_t *x, int onset, int ac, const fts_atom_t *at);


/* Remote call codes */

#define INTEGER_VECTOR_SET    1
#define INTEGER_VECTOR_UPDATE 2

#endif
