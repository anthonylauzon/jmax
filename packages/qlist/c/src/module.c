#include "fts.h"

extern void qlist_config(void);

static void
fts_qlist_init(void)
{
  qlist_config();
}

fts_module_t qlist_module = {"qlist", "qlist que file classes", fts_qlist_init};
