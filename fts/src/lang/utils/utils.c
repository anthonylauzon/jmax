#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

extern void fts_units_init(void);
/* extern void fts_float_functions_init(void); */

static void
fts_utils_init(void)
{
  fts_units_init();  
  /*  fts_float_functions_init(); */
}

fts_module_t fts_utils_module = {"Utils", "Message system utilities", fts_utils_init, 0};
