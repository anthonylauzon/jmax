typedef struct{
  int write_advance; /* n_tick if write before read */
  float conv; /* unit to samples conversion */
} vd_ctl_t;

extern void ftl_vd(fts_word_t *argv);
extern void ftl_vd_inplace(fts_word_t *argv);
