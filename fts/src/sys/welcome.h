#ifndef _WELCOME_H_
#define _WELCOME_H_

typedef struct _welcome
{
  const char *msg;
  struct _welcome *next;
} fts_welcome_t;

extern void fts_add_welcome(fts_welcome_t *w);
extern void fts_welcome_apply(void (* f)(const char *));

#endif
