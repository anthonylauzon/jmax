/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 */

#include <windows.h>
#include "thread.h"

struct _fts_thread_t {
  HANDLE thread;
  fts_thread_fun_t fun;
  void* data;
};

struct _fts_mutex_t {
  HANDLE mutex;
};

struct _fts_cond_t {
  HANDLE event;
};


static DWORD WINAPI fts_thread_run(LPVOID lpParameter)
{
  fts_thread_t* thread = (fts_thread_t*) lpParameter;
  return (*thread->fun)(thread->data);
}

fts_thread_t* 
fts_thread_new(int priority, fts_thread_fun_t fun, void* data)
{
  fts_thread_t* thread = NULL;
  int id;

  thread = (fts_thread_t*) fts_malloc(sizeof(fts_thread_t));
  if (thread == NULL) {
    fts_log("[thread]: Out of memory\n");
    return NULL;
  }

  thread->fun = fun;
  thread->data = data;

  thread->thread = CreateThread(NULL, 0, fts_thread_run, thread, 0, &id);
  if (thread->thread == NULL) {
    fts_log("[thread]: Failed to create the thread\n");
    fts_free(thread);
    return NULL;
  }

  switch (priority) {
  case fts_thread_low_priority:
    SetThreadPriority(thread->thread, THREAD_PRIORITY_LOWEST);
    break;

  case fts_thread_normal_priority:
    SetThreadPriority(thread->thread, THREAD_PRIORITY_NORMAL);
    break;

  case fts_thread_high_priority:
    SetThreadPriority(thread->thread, THREAD_PRIORITY_HIGHEST);
    break;

  case fts_thread_realtime_priority:
    SetThreadPriority(thread->thread, THREAD_PRIORITY_TIME_CRITICAL);
    break;
  }

  return thread;
}

void 
fts_thread_delete(fts_thread_t* thread)
{
  fts_thread_join(thread);
  fts_free(thread);
}

void 
fts_thread_join(fts_thread_t* thread)
{
  DWORD r;
  if (thread->thread) {
    r = WaitForSingleObject(thread->thread, 10000); /* ten seconds time out */
    if (r == WAIT_TIMEOUT) {
      TerminateThread(thread->thread, 0);
    }
  }
}

void 
fts_thread_yield()
{
  Sleep(0);
}

fts_mutex_t* 
fts_mutex_new(void)
{
  fts_mutex_t* mutex;

  mutex = (fts_mutex_t*) fts_malloc(sizeof(fts_mutex_t));
  if (mutex == NULL) {
    fts_log("[thread]: Out of memory\n");
    return NULL;
  }

  mutex->mutex = CreateMutex(NULL, FALSE, NULL);
  if (mutex->mutex == NULL) {
    fts_log("[thread]: Falied to create the mutex\n");
    fts_free(mutex);
    return NULL;
  }

  return mutex;
}

void 
fts_mutex_delete(fts_mutex_t* mutex)
{
  fts_free(mutex);  
}

int 
fts_mutex_lock(fts_mutex_t* mutex)
{
  DWORD r;
  r = WaitForSingleObject(mutex->mutex, INFINITE);  
  return (r == WAIT_OBJECT_0)? 0 : -1;
}

int 
fts_mutex_unlock(fts_mutex_t* mutex)
{
  return (ReleaseMutex(mutex->mutex) == 0)? -1 : 0;
}

fts_cond_t* 
fts_cond_new(void)
{
  fts_cond_t* cond;

  cond = (fts_cond_t*) fts_malloc(sizeof(fts_cond_t));
  if (cond == NULL) {
    fts_log("[thread]: Out of memory\n");
    return NULL;
  }

  cond->event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (cond->event == NULL) {
    fts_log("[thread]: Falied to create the conditional event\n");
    fts_free(cond);
    return NULL;
  }

  return cond;
}

void 
fts_cond_delete(fts_cond_t* cond)
{
  fts_free(cond);
}

void 
fts_cond_signal(fts_cond_t* cond)
{
  SetEvent(cond->event);
}

void 
fts_cond_wait(fts_cond_t* cond, fts_mutex_t* mutex)
{
  fts_mutex_unlock(mutex);
  WaitForSingleObject(cond->event, INFINITE);
  fts_mutex_lock(mutex);
}
