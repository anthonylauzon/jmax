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

#include "task_manager.h"
#include "thread.h"

struct _fts_task_t {
  fts_object_t* obj;
  int winlet;
  fts_symbol_t s;
  int ac;
  const fts_atom_t *at;
};

typedef struct _fts_task_t fts_task_t;


#define FTS_TASK_LIST_SIZE 1024

struct _fts_task_manager_t {
  fts_thread_t** worker;
  int num_workers;
  fts_task_t task[FTS_TASK_LIST_SIZE];
  int task_head, task_tail;
  fts_cond_t* task_cond;
  fts_mutex_t* task_mutex;
  int state;
};

int fts_worker_run(void* data);


#define task_list_full(_this)  ((_this->task_head == _this->task_tail - 1) || ((_this->task_head == FTS_TASK_LIST_SIZE - 1) && (_this->task_tail == 0)))

#define task_list_available(_this)  (_this->task_head != _this->task_tail)
#define task_list_empty(_this)  (_this->task_head == _this->task_tail)


fts_task_manager_t* 
fts_task_manager_new(int num_threads)
{
  fts_task_manager_t* manager = NULL;
  int i;

  manager = (fts_task_manager_t*) fts_calloc(sizeof(fts_task_manager_t));
  if (manager == NULL) {
    return NULL;
  }

  manager->num_workers = num_threads;
  manager->task_head = 0;
  manager->task_tail = 0;
  manager->task_cond = fts_cond_new();
  manager->task_mutex = fts_mutex_new();
  manager->state = 0;

  for (i = 0; i < FTS_TASK_LIST_SIZE; i++) {
    manager->task[i].obj = NULL;
    manager->task[i].winlet = 0;
    manager->task[i].s = NULL;
    manager->task[i].ac = 0;
    manager->task[i].at = NULL;
  }

  manager->worker = (fts_thread_t**) fts_calloc(manager->num_workers * sizeof(fts_thread_t*));

  for (i = 0; i < manager->num_workers; i++) {
    manager->worker[i] = fts_thread_new(fts_thread_high_priority, fts_worker_run, manager);
  }

  return manager;
}

void 
fts_task_manager_delete(fts_task_manager_t* manager)
{
  int i;

  /* set the state to finished */
  manager->state = 1;

  /* send enough signal to resume all worker threads */
  for (i = 0; i < manager->num_workers; i++) {

    fts_mutex_lock(manager->task_mutex);
    fts_cond_signal(manager->task_cond);
    fts_mutex_unlock(manager->task_mutex);

    fts_thread_yield();
  }

  /* the worker threads should now be finished so they can be deleted */
  for (i = 0; i < manager->num_workers; i++) {
    fts_thread_delete(manager->worker[i]);
  }

  /* deconstruct the manager object */
  fts_cond_delete(manager->task_cond);
  fts_mutex_delete(manager->task_mutex);
  fts_free((void*) manager->worker);
  fts_free((void*) manager);
}

int 
fts_task_manager_add_task(fts_task_manager_t* manager, fts_object_t* obj, int winlet, 
			  fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int head, i;

  if (!task_list_full(manager)) {

    head = manager->task_head++;

    if (manager->task_head == FTS_TASK_LIST_SIZE) {
      manager->task_head = 0;
    }

    manager->task[head].obj = obj;
    manager->task[head].winlet = winlet;
    manager->task[head].s = s;
    manager->task[head].ac = ac;
    
    if (ac == 0) {
      manager->task[head].at = NULL;
    } else {

      /* FIXME!! Should avoid allocating memory here! This is a
         real-time thread! */

      fts_atom_t* new_at = (fts_atom_t*) fts_malloc(ac * sizeof(fts_atom_t));
      for (i = 0; i < ac; i++) {
	new_at[i] = at[i];
      }

      manager->task[head].at = new_at;
    }

    fts_mutex_lock(manager->task_mutex);
    fts_cond_signal(manager->task_cond);
    fts_mutex_unlock(manager->task_mutex);

    return 0;
  }

  return -1;
}


int 
fts_worker_run(void* data)
{
  fts_task_manager_t* manager = (fts_task_manager_t*) data;
  fts_task_t task;

  while (1) {

    fts_mutex_lock(manager->task_mutex);

    while (task_list_empty(manager)) {

      fts_cond_wait(manager->task_cond, manager->task_mutex);

      if (manager->state == 1) {
	fts_mutex_unlock(manager->task_mutex);
	return 0;
      }
    }

    task = manager->task[manager->task_tail++];

    if (manager->task_tail == FTS_TASK_LIST_SIZE) {
      manager->task_tail = 0;
    }

    fts_mutex_unlock(manager->task_mutex);


    fts_send_message(task.obj, task.winlet, task.s, task.ac, task.at);

    if (task.at != NULL) {
      fts_free((void*) task.at);
    }
    
  }

  return 0;
}

