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

#ifndef _FTS_THREAD_H_
#define _FTS_THREAD_H_

#include "dtd.h"

enum {
  fts_thread_low_priority,
  fts_thread_normal_priority,
  fts_thread_high_priority,
  fts_thread_realtime_priority
} fts_thread_priority_t;

typedef struct _fts_thread_t fts_thread_t;
typedef struct _fts_cond_t fts_cond_t;
typedef struct _fts_mutex_t fts_mutex_t;
typedef int (*fts_thread_fun_t)(void* data);

DTD_API fts_thread_t* fts_thread_new(int priority, fts_thread_fun_t fun, void* data);
DTD_API void fts_thread_delete(fts_thread_t* thread);
DTD_API void fts_thread_join(fts_thread_t* thread);
DTD_API void fts_thread_yield();

DTD_API fts_mutex_t* fts_mutex_new(void);
DTD_API void fts_mutex_delete(fts_mutex_t* mutex);

/**
 *
 * -1=error, 0=no error 
 */
DTD_API int fts_mutex_lock(fts_mutex_t* mutex);
DTD_API int fts_mutex_unlock(fts_mutex_t* mutex);


DTD_API fts_cond_t* fts_cond_new(void);
DTD_API void fts_cond_delete(fts_cond_t* cond);
DTD_API void fts_cond_signal(fts_cond_t* cond);
DTD_API void fts_cond_wait(fts_cond_t* cond, fts_mutex_t* mutex);


#endif /* _FTS_THREAD_H_ */
