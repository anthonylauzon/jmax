/*
 * jMax
 * Copyright (C) 1999 by IRCAM
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
