#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

extern void fts_integer_vector_config(void);  
extern void fts_float_vector_config(void);  
extern void fts_object_set_config(void);  
extern void fts_atom_list_config(void);

static void fts_datalib_init(void);

fts_module_t fts_datalib_module = {"Datalib", "Message system utilities", fts_datalib_init, 0};

static void
fts_datalib_init(void)
{
  fts_integer_vector_config();  
  fts_float_vector_config();  
  fts_object_set_config();  
  fts_atom_list_config();
}





