/* 
   The clipboard; it have no methods, no inlets, not outlets;
   it is created by the patparser when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).
*/

#include "sys.h"
#include "lang/mess.h"

#include <sys/types.h>
#include <unistd.h>

typedef struct 
{
  fts_object_t ob;

  fts_symbol_t file;

} clipboard_t;


static void
clipboard_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  char *name;
  char buf[1024];
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (at == 1)
    name = "clipboard";
  else
    name = fts_symbol_name(fts_get_symbol(at));

  sprintf(buf, "/tmp/jmax.%s.%d", name, getpid());
  this->file = fts_new_symbol_copy(buf);
}


static void
clipboard_paste_in(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  fts_binary_file_load(fts_symbol_file(this->file), fts_get_object(at));
}



static void
clipboard_copy_selection(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;
  
  fts_save_selection_as_bmax(this->file, fts_get_object(at));
}


static void
clipboard_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  unlink(fts_symbol_name(this->file));
}

static fts_status_t
clipboard_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(clipboard_t), 0, 0, 0); 

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init,  clipboard_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete,  clipboard_delete, 0, 0);

  a[1] = fts_s_object;
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("paste"),  clipboard_paste_in, 2, a);

  a[1] = fts_s_object;
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("copy"),  clipboard_copy_selection, 2, a);

  return fts_Success;
}



void fts_clipboard_config(void)
{
  fts_metaclass_create(fts_new_symbol("__clipboard"), clipboard_instantiate, fts_always_equiv);
}

