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

#include <string.h>
#include <fcntl.h>


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

/***********************************************************************
 *
 * Generic bytestream implementation
 *
 */

static fts_symbol_t fts_s__superclass = 0;
static fts_symbol_t fts_s_bytestream = 0;

fts_class_t *fts_pipestream_class = 0;
fts_class_t *fts_memorystream_class = 0;

void fts_bytestream_init(fts_bytestream_t *stream)
{
  stream->input = 0;
  stream->listeners = 0;

  stream->output = 0;
  stream->output_char = 0;
  stream->flush = 0;
}

void fts_bytestream_destroy(fts_bytestream_t *stream)
{
  fts_bytestream_listener_t *l = stream->listeners;
  fts_bytestream_listener_t *n;

  while (l) {
    n = l->next;
    fts_free(l);
    l = n;
  }
}

void fts_bytestream_set_input(fts_bytestream_t *stream)
{
  stream->input = 1;
}

void fts_bytestream_set_output(fts_bytestream_t *stream, fts_bytestream_output_t write, fts_bytestream_output_char_t put, fts_bytestream_flush_t flush)
{
  stream->output = write;
  stream->output_char = put;
  stream->flush = flush;
}

void fts_bytestream_class_init(fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_s_bytestream);

  fts_class_put_prop(cl, fts_s__superclass, a); /* set _superclass property to "bytestream" */
}

int fts_bytestream_check(fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, fts_s__superclass, a);

  if(fts_is_symbol(a) && fts_get_symbol(a) == fts_s_bytestream)
    return 1;
  else
    return 0;
}

void fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  fts_bytestream_listener_t *l = stream->listeners;

  while(l)
    {
      fts_bytestream_listener_t *next = l->next;

      l->callback(l->listener, n, c);
      l = next;
    }
}

void fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *listener, fts_bytestream_callback_t fun)
{
  fts_bytestream_listener_t *l = (fts_bytestream_listener_t *)fts_malloc(sizeof(fts_bytestream_listener_t));

  l->callback = fun;
  l->listener = listener;
  l->next = stream->listeners;
  
  stream->listeners = l;      
}

void fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *listener)
{
  fts_bytestream_listener_t *l = stream->listeners;

  if(l)
    {
      fts_bytestream_listener_t *freeme = 0;
      
      if(l && l->listener == listener)
	{
	  freeme = l;
	  stream->listeners = l->next;
	}
      else
	{
	  while(l->next)
	    {
	      if(l->next->listener == listener)
		{
		  freeme = l->next;
		  l->next = l->next->next;
		  
		  break;
		}
	      
	      l = l->next;
	    }
	}
      
      /* free removed listener */
      if(freeme)
	fts_free(freeme);
    }
}


/***********************************************************************
 *
 * Pipe bytestream
 *
 * The pipe stream object implements a byte stream over pipes. On
 * its creation, it takes 2 arguments (pipe input and pipe output) which
 * defaults to stdin and stdout of the FTS process. In case stdin and stdout
 * are used, FTS stdout is redirected to a file.
 *
 */

typedef struct _fts_pipestream_t {
  fts_bytestream_t bytestream;
#ifdef WIN32
  HANDLE in;
  HANDLE out;
#else
  int in;
  int out;
#endif
} fts_pipestream_t;


static void fts_pipestream_receive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_pipestream_t *this = (fts_pipestream_t *) o;
#define NN 1024
  unsigned char buffer[NN];

#if WIN32
  DWORD n = 0, count = 0;

  if (PeekNamedPipe(this->in, NULL, 0, NULL, &count, NULL) && (count > 0)) {
    count = (count > NN)? NN : count;
    if (ReadFile(this->in, (LPVOID) buffer, (DWORD) count, &n, NULL)) {
      fts_bytestream_input((fts_bytestream_t *) this, n, buffer);
    } else {
      /* let the listeners handle the error situation */
      fts_bytestream_input((fts_bytestream_t *) this, -1, buffer);      
    }
  } 

#else
  int n;

  n = read( this->in, buffer, NN);

  fts_bytestream_input((fts_bytestream_t *) this, n, buffer);
#endif
}

static void fts_pipestream_output(fts_bytestream_t *stream, int n, const unsigned char *buffer)
{
  fts_pipestream_t *this = (fts_pipestream_t *) stream;

#if WIN32
  DWORD count; 

  if (!WriteFile(this->out, (LPCVOID) buffer, (DWORD) n, &count, NULL)) {

    /* keep a trace of the error in the log file */
    LPVOID msg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
    fts_log("[pipe]: failed to write to pipe: (%s)\n", msg);
    LocalFree(msg);
  }
#else
  if ( write( this->out, buffer, n) < n)
    fts_log("[pipe]: failed to write to pipe: (%s)\n", strerror( errno));
#endif
}

static void fts_pipestream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  fts_pipestream_output(stream, 1, &c);
}

static void fts_pipestream_flush(fts_bytestream_t *stream)
{
#if WIN32
  fts_pipestream_t *this = (fts_pipestream_t *) stream;
  FlushFileBuffers(this->out);
#endif
}

#ifdef WIN32
static int duplicate_std_in_out( fts_pipestream_t *this)
{
  HANDLE _stdin, _stdout;

  /* obtain stdin and stdout */
  _stdin = GetStdHandle(STD_INPUT_HANDLE); 
  this->out = GetStdHandle(STD_OUTPUT_HANDLE); 

  if ((this->out == INVALID_HANDLE_VALUE) || (_stdin == INVALID_HANDLE_VALUE))
    {
      fts_log("Invalid pipes.\n");    
      fts_object_error( (fts_object_t *)this, "invalid pipes");
      return -1;
    }

  /* close the stdin */
  if (!DuplicateHandle(GetCurrentProcess(), _stdin, 
		       GetCurrentProcess(), &this->in, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
      fts_log("Failed to duplicate stdin.\n");    
      fts_object_error( (fts_object_t *)this, "failed to duplicate stdin");
      return -1;
    }

  CloseHandle(_stdin);

      /* redirect stdout to a file */
  _stdout = CreateFile("C:\\fts_stdout.txt", 
		       GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
  if ((_stdout == INVALID_HANDLE_VALUE) 
      || !SetStdHandle(STD_OUTPUT_HANDLE, _stdout))
    {
      fts_log("Failed to redirect the stdout. Stdout will not be available.\n");
    }

  return 0;
}
#else
static int duplicate_std_in_out( fts_pipestream_t *this)
{
  if ( (this->in = dup( 0)) < 0 || (this->out = dup( 1)) < 0)
    {
      fts_object_error( (fts_object_t *)this, "failed to duplicate standard input or output");
      return -1;
    }

  if ( dup2( open( "/dev/null", O_RDONLY), 0) < 0)
    fts_log( "[pipe] cannot redirect standard input. Stdin will not be available.\n");

  if ( dup2( open( "/dev/null", O_WRONLY), 1) < 0)
    fts_log( "[pipe] cannot redirect standard output. Stdout will not be available.\n");

  return 0;
}
#endif


static void fts_pipestream_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_pipestream_t *this = (fts_pipestream_t *) o;

  fts_bytestream_init((fts_bytestream_t *) this);
  
  fts_bytestream_set_input( (fts_bytestream_t *) this);

  fts_bytestream_set_output((fts_bytestream_t *) this, fts_pipestream_output, fts_pipestream_output_char, fts_pipestream_flush);

#ifdef WIN32
  if (ac == 2 && fts_is_pointer( at) && fts_is_pointer( at+1))
    {
      this->in = (HANDLE)fts_get_pointer( at);
      this->out = (HANDLE)fts_get_pointer( at+1);
    }
  else
    {
      if ( duplicate_std_in_out( this) < 0)
	return;
    }

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_ALWAYS);  
#else
  if (ac == 2 && fts_is_int( at) && fts_is_int( at+1))
    {
      this->in = fts_get_int( at);
      this->out = fts_get_int( at+1);
    }
  else
    {
      if ( duplicate_std_in_out( this) < 0)
	return;
    }

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->in);  
#endif
}

static void fts_pipestream_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_pipestream_t *this = (fts_pipestream_t *) o;

  fts_sched_remove( (fts_object_t *)this);

#ifdef WIN32
  if (this->in != INVALID_HANDLE_VALUE) {
    CloseHandle(this->in);
    this->in = INVALID_HANDLE_VALUE;
  }  
  if (this->out != INVALID_HANDLE_VALUE) {
    CloseHandle(this->out);
    this->out = INVALID_HANDLE_VALUE;
  }  
#else
  close( this->in);
  close( this->out);
  this->in = -1;
  this->out = -1;
#endif

  fts_bytestream_destroy((fts_bytestream_t *) this);
}

static void fts_pipestream_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_pipestream_t), fts_pipestream_init, fts_pipestream_delete);
  fts_bytestream_class_init(cl);

  fts_class_message_varargs(cl, fts_s_sched_ready, fts_pipestream_receive);
}

/***********************************************************************
 *
 * Memory bytestream
 * (the object that implements an output stream writing into an array of bytes)
 *
 */

struct _fts_memorystream_t {
  fts_bytestream_t bytestream;
  fts_stack_t output_buffer;
  int input_size;
};

static void fts_memorystream_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  fts_memorystream_t *this = (fts_memorystream_t *) stream;
  int i;

  for ( i = 0; i < n; i++)
    fts_stack_push( &this->output_buffer, unsigned char, c[i]);
}

static void fts_memorystream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  fts_memorystream_output(stream, 1, &c);
}

static void fts_memorystream_flush(fts_bytestream_t *stream)
{
}

static void fts_memorystream_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *this = (fts_memorystream_t *) o;

  fts_bytestream_init((fts_bytestream_t *) this);

  fts_bytestream_set_output((fts_bytestream_t *) this, 
			    fts_memorystream_output,
			    fts_memorystream_output_char,
			    fts_memorystream_flush);
  
  fts_stack_init( &this->output_buffer, unsigned char);
}

static void fts_memorystream_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *this = (fts_memorystream_t *) o;

  fts_bytestream_destroy((fts_bytestream_t *) this);

  fts_stack_destroy( &this->output_buffer);
}

static void fts_memorystream_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_memorystream_t), fts_memorystream_init, fts_memorystream_delete);
  fts_bytestream_class_init(cl);
}

unsigned char *fts_memorystream_get_bytes( fts_memorystream_t *stream)
{
  return (unsigned char *)fts_stack_base( &stream->output_buffer);
}

void fts_memorystream_reset( fts_memorystream_t *stream)
{
  fts_stack_clear( &stream->output_buffer);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_bytestream_init( void)
{
  fts_s_bytestream = fts_new_symbol("bytestream");
  fts_s__superclass = fts_new_symbol("_superclass");

  fts_pipestream_class = fts_class_install( fts_new_symbol("pipestream"), fts_pipestream_instantiate);
  fts_memorystream_class = fts_class_install( fts_new_symbol("memorystream"), fts_memorystream_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
