#include "fts.h"
#include <sigfpe.h>
#include <signal.h>

static long n_shot = 0;

/* extern user_t my_invalid_handler; */

typedef struct _hfpe
{
  fts_object_t _o;
} hfpe_t;

/**************************************************
 *
 *  handler functions
 *
 */

void
hfpe_all_off(void)
{
  handle_sigfpes(_OFF, _EN_OVERFL | _EN_DIVZERO | _EN_INVALID | _EN_INT_OVERFL, 0, 0, 0);
}

int 
hfpe_handler_print_and_off(int sig, int code, struct sigcontext *sc)
{
  int fpe_trap_type = __fpe_trap_type();

  if(sig == SIGFPE)
    post("floating-point exeption signal: ");
  else
    post("unknown signal: %d", sig);

  switch (fpe_trap_type) {
  case _UNDERFL:
    post("underflow\n");
    break;
  case _OVERFL:
    post("overflow\n");
    break;
  case _DIVZERO:
    post("divide by zero\n");
    break;
  case _INVALID:
    post("invalid operand\n");
    break;
  case _INT_OVERFL:
    post("integer overflow\n");
    break;
  case _NO_EXCEPTION_TYPE:
    post("NO_EXCEPTION_TYPE\n");
    break;
  case _ALL_EXCEPTION_TYPES:
    post("ALL_EXCEPTION_TYPES\n");
    break;
  default:  
    post("unknown exeption code\n");
    break;
  }

  n_shot--;

  if(n_shot <= 0)
    {
      hfpe_all_off();      
      return (1); /* no further handling */
    }
  else
    return (0); /* go on handling */
}

/**************************************************
 *
 *  object
 *
 */

static void
hfpe_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  hfpe_t *this = (hfpe_t *)o;

  n_shot = 0;
}

static void hfpe_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  hfpe_t *this = (hfpe_t *)o;

  hfpe_all_off();
}

/**************************************************
 *
 *  methods
 *
 */

static void hfpe_activate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n = fts_get_number_arg(ac, at, 0, 0);
  
  hfpe_all_off();

  if(n <= 0)
    n_shot = 1;
  else
    n_shot = n;

  handle_sigfpes(_ON, _EN_OVERFL | _EN_DIVZERO | _EN_INVALID | _EN_INT_OVERFL, 0, _USER_HANDLER, (abort_t)hfpe_handler_print_and_off);
}

static void hfpe_cancel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  n_shot = 0;
  hfpe_all_off();
}

static fts_status_t hfpe_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(fts_object_t), 1, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("activate"), hfpe_activate, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("cancel"), hfpe_cancel, 0, 0);

  return fts_Success;
}

void hfpe_config( void)
{
  fts_metaclass_create( fts_new_symbol("hfpe"),hfpe_instantiate, fts_always_equiv);
}
