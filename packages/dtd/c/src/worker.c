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

#include "worker.h"
#include "thread.h"

struct _fts_worker_task_t {
  fts_object_t* obj;
  int winlet;
  fts_symbol_t s;
  int ac;
  const fts_atom_t *at;
};

typedef struct _fts_worker_task_t fts_worker_task_t;


struct _fts_worker_t {
  fts_worker_task_t* task_list;
  int task_list_size;
  int task_head, task_tail;
  fts_thread_t* thread;
  fts_cond_t* task_cond;
  fts_mutex_t* task_mutex;
  int state;
};

int fts_worker_execute(void* data);


#define task_list_full(_this)  ((_this->task_head == _this->task_tail - 1) || ((_this->task_head == _this->task_list_size - 1) && (_this->task_tail == 0)))

#define task_list_available(_this)  (_this->task_head != _this->task_tail)
#define task_list_empty(_this)  (_this->task_head == _this->task_tail)


fts_worker_t* 
fts_worker_new(int size)
{
  fts_worker_t* worker = NULL;
  int i;

  worker = (fts_worker_t*) fts_calloc(sizeof(fts_worker_t));
  if (worker == NULL) {
    return NULL;
  }

  worker->task_head = 0;
  worker->task_tail = 0;
  worker->task_cond = fts_cond_new();
  worker->task_mutex = fts_mutex_new();
  worker->state = 0;

  if ((worker->task_cond == NULL) || (worker->task_mutex == NULL)) {
    goto error_recovery;
  }

  if (size <= 0) {
    size = 16;
  } else if (size > 32768) {
    size = 32768;
  }

  worker->task_list_size = size;

  worker->task_list = (fts_worker_task_t*) fts_malloc(worker->task_list_size * sizeof(fts_worker_task_t));
  if (worker->task_list == NULL) {
    goto error_recovery;
  }

  for (i = 0; i < worker->task_list_size; i++) {
    worker->task_list[i].obj = NULL;
    worker->task_list[i].winlet = 0;
    worker->task_list[i].s = NULL;
    worker->task_list[i].ac = 0;
    worker->task_list[i].at = NULL;
  }

  worker->thread = fts_thread_new(fts_thread_high_priority, fts_worker_execute, worker);
  if (worker->thread == NULL) {
    goto error_recovery;
  }

  return worker;

 error_recovery:
  fts_worker_delete(worker);
  return NULL;
}

void 
fts_worker_delete(fts_worker_t* worker)
{
  if (worker == NULL) {
    return;
  }

  /* set the state to finished */
  worker->state = 1;

  /* signal the worker thread */
  if (worker->task_mutex != NULL) { 
    fts_mutex_lock(worker->task_mutex);
  }
  if (worker->task_cond != NULL) { 
    fts_cond_signal(worker->task_cond);
  }
  if (worker->task_mutex != NULL) { 
    fts_mutex_unlock(worker->task_mutex);
  }

  /* wait for the worker thread to finish, then destroy it */
  if (worker->thread != NULL) {
    fts_thread_join(worker->thread);
    fts_thread_delete(worker->thread);
  }

  /* clean up all the rest */

  if (worker->task_cond != NULL) { 
    fts_cond_delete(worker->task_cond);
  }

  if (worker->task_mutex != NULL) { 
    fts_mutex_delete(worker->task_mutex);
  }

  if (worker->task_list != NULL) {
    fts_free((void*) worker->task_list);
  }

  fts_free((void*) worker);
}

int 
fts_worker_add_task(fts_worker_t* worker, fts_object_t* obj, int winlet, 
		    fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int head, i;

  if (!task_list_full(worker)) {

    head = worker->task_head++;

    if (worker->task_head == worker->task_list_size) {
      worker->task_head = 0;
    }

    worker->task_list[head].obj = obj;
    worker->task_list[head].winlet = winlet;
    worker->task_list[head].s = s;
    worker->task_list[head].ac = ac;
    
    if (ac == 0) {
      worker->task_list[head].at = NULL;
    } else {

      /* FIXME!! Should avoid allocating memory here! This is a
         real-time thread! */

      fts_atom_t* new_at = (fts_atom_t*) fts_malloc(ac * sizeof(fts_atom_t));
      for (i = 0; i < ac; i++) {
	new_at[i] = at[i];
      }

      worker->task_list[head].at = new_at;
    }

    fts_mutex_lock(worker->task_mutex);
    fts_cond_signal(worker->task_cond);
    fts_mutex_unlock(worker->task_mutex);

    return 0;
  }

  return -1;
}


int 
fts_worker_execute(void* data)
{
  fts_worker_t* worker = (fts_worker_t*) data;
  fts_worker_task_t task;

  while (1) {

    fts_mutex_lock(worker->task_mutex);

    while (task_list_empty(worker)) {

      fts_cond_wait(worker->task_cond, worker->task_mutex);

      if (worker->state == 1) {
	fts_mutex_unlock(worker->task_mutex);
	return 0;
      }
    }

    task = worker->task_list[worker->task_tail++];

    if (worker->task_tail == worker->task_list_size) {
      worker->task_tail = 0;
    }

    fts_mutex_unlock(worker->task_mutex);


    fts_send_message(task.obj, task.winlet, task.s, task.ac, task.at);

    if (task.at != NULL) {
      fts_free((void*) task.at);
    }
    
  }

  return 0;
}

