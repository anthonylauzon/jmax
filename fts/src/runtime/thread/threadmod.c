/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"

  /* Thread module
     This is temporary: the thread module declares a scheduled function
     that polls the file descriptors set of the thread.
     Evolution: this function will become the only function called by the thread
     and the current scheduler will disappear
  */
static void thread_init(void);
static void thread_fd_set_poll(void);

fts_module_t fts_thread_module = {"Thread", "Thread scheduling (temporary)", thread_init, 0, 0, 0};

static void thread_init(void)
{
  /* schedule declaration */

  fts_sched_declare( thread_fd_set_poll, provide, fts_new_symbol("control"), "thread_fd_set_poll");
}

static void thread_fd_set_poll(void)
{
    fts_thread_do_select( fts_thread_get_current());
}
