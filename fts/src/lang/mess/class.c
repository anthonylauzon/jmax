/* first test implementation; it should actually be more integrated
   with the kernel, load by need, having options of different type,
   properties that are put to the object and can be obtained from Ermes,
   and so on ...
 */

#include "sys.h"
#include "lang/mess.h"


typedef struct class_t
{
  fts_object_t _o;
  
  fts_symbol_t class_name;
  fts_symbol_t module_name;
  fts_symbol_t lib_name;

  int loaded;

  struct class_t *next;
} class_t;



static void
class_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  class_t *this = (class_t *) o;

  this->class_name  = fts_get_symbol_arg(ac, at, 1, 0);
  this->module_name = fts_get_symbol_arg(ac, at, 2, 0);
  this->lib_name    = fts_get_symbol_arg(ac, at, 3, 0);
  this->loaded = 0;
  this->next = 0;

  fts_metaclass_declare_dynamic(this->class_name, o);
}


static void
class_load(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  class_t *this = (class_t *) o;

  if (! this->loaded)
    {
      if (fts_module_load(fts_symbol_name(this->module_name),
			  this->lib_name ? fts_symbol_name(this->lib_name) : 0))
	this->loaded = 1;
    }

  if (this->next)
    fts_message_send((fts_object_t *)this->next, fts_SystemInlet, s, ac, at);
}


static void
class_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  class_t *this = (class_t *) o;

  if (this->next)
    fts_message_send((fts_object_t *)this->next, fts_SystemInlet, s, ac, at);
  else
    this->next = (class_t *) fts_get_ptr(&at[0]);
}


static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[12];

  fts_class_init(cl, sizeof(class_t), 0, 0, 0);

  a[0] = fts_s_symbol; /* the "class" name */
  a[1] = fts_s_symbol; /* the class name */
  a[2] = fts_s_symbol; /* the module name */
  a[3] = fts_s_symbol; /* the module file name */

  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, class_init, 4, a, 3);

   /* Add  the declaration property */

  {
    fts_atom_t av;

    fts_set_int(&av, 1);

    fts_class_put_prop(cl, fts_new_symbol("declaration"), &av);
  }

  /* may be should be an inlet 0 method ?? */

  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("load"), class_load, 0, 0);

  return fts_Success;
}

void
class_config(void)
{
  fts_metaclass_create(fts_new_symbol("class"), class_instantiate, fts_always_equiv);
}
