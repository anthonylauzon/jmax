#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

extern void fts_intvec_config(void);  
extern void floatvector_config(void);  
extern void fts_objectset_config(void);  

static void fts_datalib_init(void);

fts_module_t fts_datalib_module = {"Datalib", "Message system utilities", fts_datalib_init, 0};

static void
fts_datalib_init(void)
{
  fts_intvec_config();  
  floatvector_config();  
  fts_objectset_config();  
}





