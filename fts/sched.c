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

#include <fts/fts.h>
#include <ftsconfig.h>
#include <ftsprivate/sched.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H 
#include <sys/types.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if HAVE_TIME_H
#include <time.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
/* included for the select() function */
#include <windows.h>
#endif

typedef struct sched_callback
{
  fts_object_t *object;
  int flags;
  int fd;
  fts_method_t ready_mth;
  fts_method_t error_mth;
  struct sched_callback *next;
} sched_callback_t;

typedef struct fts_sched
{
  sched_callback_t *callback_head; 
  enum {sched_ready, sched_halted} status;
} fts_sched_t;

/* the global scheduler */
static fts_sched_t main_sched;

/* running sched param */
static fts_param_t* sched_running_param = 0;

static fts_sched_t *
fts_sched_get_current(void)
{
  return &main_sched;
}

/*****************************************************************************
 *
 *  select and file descrpitors
 *
 */

int fts_sched_add( fts_object_t *obj, int flags, ...)
{
  va_list ap;
  fts_sched_t *sched = fts_sched_get_current();
  sched_callback_t *callback;
  int fd = -1;
  fts_method_t mth;

  va_start( ap, flags);
  if (flags == FTS_SCHED_READ || flags == FTS_SCHED_WRITE)
    fd = va_arg( ap, int);

  mth = fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_sched_ready);
  if(!mth)
  {
    fprintf( stderr, "[sched] object %s does not define a method for \"sched_ready\"\n", fts_class_get_name(fts_object_get_class(obj)));
    return -1;
  }

  callback = (sched_callback_t *)fts_malloc( sizeof( sched_callback_t));
  callback->object = obj;
  callback->flags = flags;
  callback->fd = fd;
  callback->ready_mth = mth;
  callback->error_mth = fts_class_get_method_varargs( fts_object_get_class(obj), fts_s_sched_error);

  callback->next = sched->callback_head;
  sched->callback_head = callback;

  va_end( ap);

  return 0;
}

int fts_sched_remove( fts_object_t *obj)
{
  sched_callback_t **p;
  fts_sched_t *sched = fts_sched_get_current();

  p = &sched->callback_head;

  while (*p)
  {
    if ( (*p)->object == obj)
    {
      sched_callback_t *to_remove;

      to_remove = *p;
      *p = (*p)->next;

      fts_free( to_remove);
    }
    else
      p = &(*p)->next;
  }

  return -1;
}

static int compute_fds( fts_sched_t *sched, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
  sched_callback_t *callback;
  int n_fd = 0;

  FD_ZERO( readfds);
  FD_ZERO( writefds);
  FD_ZERO( exceptfds);

  n_fd = 0;
  for ( callback = sched->callback_head; callback; callback = callback->next)
  {
    if (callback->flags == FTS_SCHED_ALWAYS)
      continue;

    if ( callback->fd > n_fd)
      n_fd = callback->fd;

    if (callback->flags == FTS_SCHED_READ)
      FD_SET( callback->fd, readfds);
    else
      FD_SET( callback->fd, writefds);

    FD_SET( callback->fd, exceptfds);
  }

  return n_fd;
}

static void run_select( fts_sched_t *sched, int n_fd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
  struct timeval tv;
  sched_callback_t *callback, *next;
  int r;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  r = select( n_fd+1, readfds, writefds, exceptfds, &tv);

  if (r < 0)
  {
    fprintf( stderr, "[FTS] select error (%s)\n", strerror( errno));
    return;
  }
  else if ( r == 0)
    return;

  /* Attention:
     the callback method ***can*** call fts_sched_remove, thus removing
     the entry in the scheduler callback list.
  */
  for ( callback = sched->callback_head; callback; callback = next)
  {
    int fd = callback->fd;
    fts_atom_t a;

    next = callback->next;

    fts_set_int( &a, fd);

    if ( callback->error_mth && FD_ISSET( fd, exceptfds))
      (*callback->error_mth)( callback->object, fts_system_inlet, fts_s_sched_error, 1, &a);
    else
    {
      if ( (callback->flags == FTS_SCHED_READ && FD_ISSET( fd, readfds))
	   || (callback->flags == FTS_SCHED_WRITE && FD_ISSET( fd, writefds)) )
	(*callback->ready_mth)( callback->object, fts_system_inlet, fts_s_sched_ready, 1, &a);
    }
  }
}

static void run_always( fts_sched_t *sched)
{
  sched_callback_t *callback, *next;

  /* Attention (idem run_select):
     the callback method ***can*** call fts_sched_remove, thus removing
     the entry in the scheduler callback list.
  */
  for ( callback = sched->callback_head; callback; callback = next)
  {
    next = callback->next;

    if ( callback->flags == FTS_SCHED_ALWAYS)
      (*callback->ready_mth)( callback->object, fts_system_inlet, fts_s_sched_ready, 0, 0);
  }
}


static void fts_sched_do_select(fts_sched_t *sched)
{
#if !defined(__POWERPC__) || (defined(__APPLE__) && defined(__MACH__))
  fd_set readfds, writefds, exceptfds;
  int n_fd;

  n_fd = compute_fds( sched, &readfds, &writefds, &exceptfds);

  if (n_fd != 0)
    run_select( sched, n_fd, &readfds, &writefds, &exceptfds);

#endif	
  run_always( sched);
}

void fts_sched_run_one_tick(void)
{
  fts_sched_t* sched = fts_sched_get_current();
  fts_sched_do_select(sched);
}

void
fts_sched_init(fts_sched_t *sched)
{
  fts_param_set_int(sched_running_param, 1);
  sched->callback_head = 0;
  sched->status = sched_ready;
}

/* run the scheduler in a loop */
void 
fts_sched_run(void)
{
  /* poll file descriptors and run functions inserted to the scheduler */
  while(main_sched.status != sched_halted)
    fts_sched_do_select(&main_sched);
}

void 
fts_sched_halt(void)
{
  main_sched.status = sched_halted;
  fts_param_set_int(sched_running_param, 0);
}

int fts_sched_is_running(void)
{
  return main_sched.status != sched_halted;
}

void
fts_sched_running_add_listener(fts_object_t* object, fts_method_t method)
{
  fts_param_add_listener(sched_running_param, object, method);
}

void 
fts_sched_running_remove_listener(fts_object_t* object)
{
  fts_param_remove_listener(sched_running_param, object);
}
/************************************************************
 *
 *  platform dependent implementation of
 *  fts_sleep()
 *
 */
#ifdef WIN32
#define DEBUG_SLEEP 0

static struct _sleeper_
{
  int count;
  DWORD sysstart;
  double ftsstart;
} sleeper;

static void
sleep_init(void)
{
  sleeper.count = 0;
  sleeper.sysstart = GetTickCount();
  sleeper.ftsstart = 0.0;
}

void
fts_sleep(void)
{
  if ( ++sleeper.count == 5) 
  {
    double ftstime = fts_get_time() - sleeper.ftsstart;
    double systime = GetTickCount() - sleeper.sysstart;
    double delta = ftstime - systime;
      
#if DEBUG_SLEEP
    FILE* log = fopen("C:\\nullaudiolog.txt", "a");
    fprintf(log, "fts time=%f, sys time=%f, delta=%f\n", ftstime, systime, delta);
    fclose(log);
#endif
      
    sleeper.count = 0;
      
    if (delta > 0)
      Sleep((DWORD) delta);
  }
}

#else

static double last_sleep = 0.0;

static void
sleep_init(void)
{
  last_sleep = 0.0;
}

void
fts_sleep(void)
{
  double now;

  now = fts_get_time();
  
  if (now - last_sleep >= (double)100.0)
  {
    struct timespec pause_time;

    pause_time.tv_sec = 0;
    pause_time.tv_nsec = 100000000L;

    nanosleep( &pause_time, 0);

    last_sleep = now;
  }
}
#endif


/*****************************************************************************
 *
 *  Initialization
 *
 */

void fts_kernel_sched_init(void)
{
  sleep_init();

  sched_running_param = (fts_param_t*)fts_object_create(fts_param_class, 0, 0);
  fts_sched_init(&main_sched);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
