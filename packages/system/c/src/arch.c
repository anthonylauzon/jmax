

#include "fts.h"
#include <string.h>

static fts_symbol_t arch_sym;

typedef struct 
{
  fts_object_t _o;

} arch_t;


static void
arch_bang_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 0, arch_sym, 0, 0);
}

static fts_status_t
arch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* arch control object */

  fts_class_init(cl, sizeof(arch_t), 1, 1, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, arch_bang_mth);
  
  return fts_Success;
}

void
arch_config(void)
{
  arch_sym = fts_new_symbol(FTS_ARCH_NAME);

  fts_metaclass_create(fts_new_symbol("arch"),arch_instantiate, fts_always_equiv);
}






