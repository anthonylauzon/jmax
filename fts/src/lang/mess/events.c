/* Object based error handling
   
   First implementation

   Many things missing: an fts_error/event function that
   allow a one line of code signalling of errors, 
   a simple way to generate/install handlers, 
   others may be ???
 */

#include "sys.h"
#include "lang/mess.h"

typedef struct _error_handler
{
  fts_object_t *handler;
  struct _error_handler *next;
} fts_event_handler_t;

static fts_heap_t *handler_heap;

fts_event_handler_t *global_handlers = 0;
fts_event_handler_t *default_handlers = 0;
fts_event_handler_t *handlers_stack  = 0;


void
fts_event_add_handler(fts_object_t *handler)
{
  fts_event_handler_t *hnd;

  hnd = (fts_event_handler_t *) fts_heap_alloc(handler_heap);

  hnd->next = global_handlers;
  hnd->handler = handler;

  global_handlers = hnd;
}


void
fts_event_remove_handler(fts_object_t *handler)
{
  fts_event_handler_t **phnd;

  phnd = &global_handlers;

  while (*phnd)
    {
      if ( (*phnd)->handler == handler)
	{
	  fts_event_handler_t *hnd;

	  hnd = (*phnd);

	  *phnd = ((*phnd)->next);

	  fts_heap_free((char *) hnd, handler_heap);
	}
      else 
	phnd = & ((*phnd)->next);
    }
}


void
fts_event_push_handler(fts_object_t *handler)
{
  fts_event_handler_t *hnd;

  hnd = (fts_event_handler_t *) fts_heap_alloc(handler_heap);

  hnd->next = handlers_stack;
  hnd->handler = handler;

  handlers_stack = hnd;
}


void
fts_event_pop_handler(fts_object_t *handler)
{
  fts_event_handler_t *hnd, *next_hnd;
  fts_object_t *current;

  do
    {
      hnd = handlers_stack;
      current = hnd->handler;

      handlers_stack = hnd->next;
      fts_heap_free((char *) hnd, handler_heap);
    }
  while (current != handler);
}


/* Note that a default handler can't be de-installed;
 */

void
fts_event_add_default_handler(fts_object_t *handler)
{
  fts_event_handler_t *hnd;

  hnd = (fts_event_handler_t *) fts_heap_alloc(handler_heap);

  hnd->next = default_handlers;
  hnd->handler = handler;

  default_handlers = hnd;
}


void
fts_event(fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int global_found;
  fts_event_handler_t *hnd;

  hnd = handlers_stack;
  global_found  = 0;

  /* First, we look in the stack; first handler found executed */

  while (hnd)
    if (fts_send_message(hnd->handler, fts_SystemInlet, s, ac, at) == fts_Success)
      return;
    else
      hnd = hnd->next;

  /* If not found in the stack, we look in the global handlers; all handlers are executed */

  hnd = global_handlers;
  
  while (hnd)
    {
      if (fts_send_message(hnd->handler, fts_SystemInlet, s, ac, at) == fts_Success)
	global_found = 1;
      hnd = hnd->next;
    }

  
  if (! global_found)
    {
      int found = 0;

      /* If not found in the stack or global handlers, we look in the default handlers;
	 all handlers are executed */

      hnd = default_handlers;
  
      while (hnd)
	{
	  if (fts_send_message(hnd->handler, fts_SystemInlet, s, ac, at) == fts_Success)
	    found = 1;

	  hnd = hnd->next;
	}

      /* if still not found, domage, the event/error is lost !!!! */
    }
}

void fts_events_init(void)
{
  handler_heap = fts_heap_new(sizeof(fts_event_handler_t));
}
