#ifndef _STATUS_H_
#define _STATUS_H_

/*

   General status return values for FTS.

*/

typedef struct 
{
  const char *description;
} fts_status_description_t;


typedef fts_status_description_t *fts_status_t;

#define fts_Success ((fts_status_t) 0)

#endif
