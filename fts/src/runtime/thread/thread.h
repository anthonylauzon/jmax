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
#ifndef _THREAD_H_
#define _THREAD_H_

/* 
   Description:
     The fts_thread_t abstraction contains all the data that are relevant for the
     scheduling.
     The first implementation keeps only the file descriptor set.
*/
typedef struct _fts_thread *fts_thread_t;

/*
   Function: fts_thread_get_current
   Description:
     Returns an identifier of the thread calling it.
   Returns: a fts_thread_t instance that identifies the thread calling this function 
     This identifier is the same that the creating thread gets as return value
     from fts_thread_create()
*/
/* 
   NOTE:
     This is the only function that is implemented now. The first implementation
     returns a pointer to a static structure.
*/
extern fts_thread_t fts_thread_get_current( void);

/* 
   Function: fts_thread_new
   Description:
     Creates a thread and set its execution function.
   Arguments:
     start: the execution function of the thread
   Returns: a fts_thread_t instance that identifies the created thread 
*/
/* 
   NOTE:
     This function is not implemented yet. It will return NULL.
*/
extern fts_thread_t fts_thread_new( void (*start)( void *));

/* 
   Function: fts_thread_start
   Description:
     Start a thread
   Arguments:
     arg: the argument that is passed to the thread's execution function
   Returns: nothing
*/
/* 
   NOTE:
     This function is not implemented yet.
*/
extern void fts_thread_start( fts_thread_t thread, void *arg);


typedef void (*fts_thread_fd_fun_t)( int fd, void *data);

/*
  Function: fts_thread_add_fd
  Description:
    Add a file descriptor to the file descriptor set of the thread and declare a function
    that will be called when the file descriptor is set (i.e. a read() or write() call will not
    block).
  Arguments:
    thread: the thread owning the file descriptor set to which the file descriptor will be added
    fd: the file descriptor
    fun: the function to call when file descriptor is set
    data: a pointer passed as argument to the function
  Returns: nothing.
*/
extern void fts_thread_add_fd( fts_thread_t thread, int fd, fts_thread_fd_fun_t fun, void *data);

/*
  Function: fts_thread_remove_fd
  Description:
    Remove a file descriptor from the file descriptor set of the thread.
  Arguments:
    thread: the thread owning the file descriptor set to which the file descriptor will be removed
    fd: the file descriptor
  Returns: nothing.
*/
extern void fts_thread_remove_fd( fts_thread_t thread, int fd);


/*
  Function: fts_thread_do_select
  Description:
    Do a single select() on all the files in the file descriptor set of the thread.
  Arguments:
    thread: the thread owning the file descriptor set
  Returns: nothing.
*/
extern void fts_thread_do_select( fts_thread_t thread);

#endif

