/*
   Support for user modules startup messages.
   */


#include "sys.h"


static fts_welcome_t *welcome_list = 0;

void
fts_add_welcome(fts_welcome_t *w)
{
  if (welcome_list)
    {
      fts_welcome_t *p;

      for (p = welcome_list; p->next; p = p->next)
	;
      p->next = w;
    }
  else
    welcome_list = w;
}

/* fts_welcome_apply:
   hw or module dependent welcome should be moved in the proper
   module init function */

void
fts_welcome_apply(void (* f)(const char *))
{
  fts_welcome_t *p;

  for (p = welcome_list; p; p = p->next)
    (* f)(p->msg);
}



