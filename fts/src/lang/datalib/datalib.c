#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

extern void fts_intvec_init(void);  
static void fts_datalib_init(void);

fts_module_t fts_datalib_module = {"Datalib", "Message system utilities", fts_datalib_init, 0};



static void
fts_datalib_init(void)
{
  fts_intvec_init();  
}
