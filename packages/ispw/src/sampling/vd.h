typedef struct{
  float write_advance; /* delwrite~'s tick size if write before read (if not 0) */
  float conv; /* unit to samples conversion */
  float max_span; /* maximum travel during one vector (max_span / (<vector size> - 1) = <max. increment> = <max. transp.> + 1 */
} ftl_vd_t;

extern void ftl_vd(fts_word_t *argv);
extern void ftl_vd_miller(fts_word_t *argv);
