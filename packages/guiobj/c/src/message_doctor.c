/*
  Message doctor.

  Fix message boxes created with as argument the content of the message.
  Message box like that are present in some binary and tcl patches
  around, saved before 20/5/1998.
 */

#include "fts.h"

static fts_object_t *message_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac >= 1)
    {
      fts_object_t *obj;

      fts_make_object(patcher, 1, at, &obj);

      if (ac > 1)
	{
	  fts_message_send(obj, fts_SystemInlet, fts_s_clear, 0, 0);
	  fts_message_send(obj, fts_SystemInlet, fts_s_append, ac - 1, at + 1);
	}

      return obj;
    }
  else
    return 0;
}


void message_doctor_init()
{
  fts_register_object_doctor(fts_new_symbol("messbox"), message_doctor);
}
    
