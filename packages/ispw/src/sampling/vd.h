
typedef struct{
  float fudge; /* 1/(n-1) */
  long maxspan; /* maximum travel allowed per call */
  float delonset; /* correction to subtract from deltimes */
  float writevecsize; /* number of samps in delwrite per vd call */
  float conv; /* unit to samples conversion */
} vd_ctl_t;

extern void ftl_vd(fts_word_t *argv);
