/* This file include all the kernel provided post like
   (client printing) functions.
*/

#include "sys.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lang.h"
#include "runtime/time.h"
#include "runtime/devices.h"
#include "runtime/client.h"
#include "runtime/files/post.h"


/******************************************************************************/
/*                                                                            */
/*              Basic Post Functions                                          */
/*                                                                            */
/******************************************************************************/

/*
   Due to limitation in the handling of  string type argument,
   post cannot be buffer events, must be immediate events.
*/

void
postvector(int n, float *fp)
{
  int i;
  
  for (i = 0; i < n; i+=4)
    post("%4d: %f %f %f %f\n", i, fp[i], fp[i+1], fp[i+2], fp[i+3]);

  post("\n");
}


void
postatoms(int ac, const fts_atom_t *at)
{
  int i;

  for (i = 0; i < ac; i++)
    {
      char *ps;

      if (i == (ac-1))
	ps = "";
      else
	ps = " ";

      if (fts_is_symbol(&at[i]))
	post("%s%s", fts_symbol_name(fts_get_symbol(&at[i])), ps);
      else if (fts_is_long(&at[i]))
	post("%d%s", fts_get_long(&at[i]), ps);
      else if (fts_is_float(&at[i]))
	post("%f%s", fts_get_float(&at[i]), ps);
      else if (fts_is_ptr(&at[i]) )
	post("%lx%s", (unsigned long) fts_get_ptr( &at[i]), ps);
      else if (fts_is_void(&at[i]))
	post("(void)%s", ps);
      else
	post("???%s", ps);
    }
}


void post( const char *format, ...)
{
  va_list ap;
  char buf[512];

  va_start( ap, format);

  vsprintf(buf, format, ap);

  va_end(ap);

  fts_client_mess_start_msg(POST_CODE);
  fts_client_mess_add_string(buf);
  fts_client_mess_send_msg();
}



/* Convenience functions to post status values (errors)
   temporary stuff, will go away with the new error system.
   */

void
post_error(fts_status_t error)
{
  post("Error: %s\n", error->description);
}

/*
   Clobber now use a global clobber time gate, with a fixed two second window.
   no more time gate in the error structure.
*/

static fts_time_gate_t clobber_time_gate;
static int time_gate_initted = 0;

void
post_error_noclobber(fts_status_t error)
{
  if (! time_gate_initted)
    {
      time_gate_initted = 1;
      fts_time_gate_init(&clobber_time_gate, 0);
    }

  if (fts_time_gate_close(&clobber_time_gate, 2000.0))
    post_error(error);
}


