#ifndef _SAMPWRITE_H_
#define _SAMPWRITE_H_

#include "sampbuf.h"

typedef struct{
  long onset;
  sampbuf_t *buf;
} sampwrite_ctl_t;

extern void ftl_sampwrite(fts_word_t *argv);


#endif /* _SAMPWRITE_H_ */
