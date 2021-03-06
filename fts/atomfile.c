/*
 * jMax
 * Copyright (C) 2004 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
#include <fts/fts.h>
#include <stdlib.h>
#include <ctype.h>

#include <ftsconfig.h>

#include <string.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_IO_H
#include <io.h>
#endif

#define ATOMFILE_BUF_SIZE 512

struct fts_atomfile
{
  fts_symbol_t filename;
  FILE* file;
  
  /* for reading  */
  char buf[ATOMFILE_BUF_SIZE]; /* buffer of character read */
  long count; /* the current size of the buffer */
  long read; /* the index of the last char read */
};

fts_atomfile_t *
fts_atomfile_open_read(fts_symbol_t name)
{
  char *filename = (char *)fts_symbol_name(name);
  char str[1024];
  char *fullpath = fts_file_find(filename, str, 1023);
  
  if(fullpath != NULL)
  {
    FILE *file = fopen(fullpath, "r");
    
    if(file != NULL)
    {
      fts_atomfile_t *atomfile = (fts_atomfile_t *)fts_malloc(sizeof(fts_atomfile_t));
      
      atomfile->filename = name;
      atomfile->file = file;
      atomfile->count = 0;
      atomfile->read = 0;
    
      return atomfile;
    }
  }
  
  return NULL;
}

fts_atomfile_t *
fts_atomfile_open_write(fts_symbol_t name)
{
  char *filename = (char *)fts_symbol_name(name);
  char str[1024];
  char *fullpath = fts_make_absolute_path(NULL, filename, str, 1023);
  FILE *file = fopen(fullpath, "w");
  
  if(file != NULL)
  {
    fts_atomfile_t *atomfile = (fts_atomfile_t *)fts_malloc(sizeof(fts_atomfile_t));
    atomfile->filename = name;
    atomfile->file = file;
    
    return atomfile;
  }
  
  return NULL;
}

void
fts_atomfile_close(fts_atomfile_t *f)
{
  fclose(f->file);
  fts_free(f);
}

#define IS_SEPARATOR(c) ((((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\0'))? (c): 0)
#define IS_DIGIT(c) (('0' <= (c)) && ((c) <= '9'))
#define IS_EXP(c)   ((c) == 'e')
#define IS_SIGN(c) (((c) == '+') || ((c) == '-'))
#define IS_POINT(c) ('.' == (c))
#define IS_BACKSLASH_QUOTE(c) ('\\' == (c))
#define IS_DOUBLE_QUOTE(c) ('\"' == (c))
#define IS_ATOM_CHAR(c) (((c) == ';') || ((c) == ',') || ((c) == '$') || ((c) == '\''))
#define IS_EOF(c) (0xff == (c))

static void
atomfile_read_more(fts_atomfile_t *f)
{
  f->count = fread(f->buf, 1, ATOMFILE_BUF_SIZE, f->file);
  f->read = 0;
}

int
fts_atomfile_read(fts_atomfile_t *f, fts_atom_t *at, char *separator)
{
  char buf[1024];
  enum
  {
    read_begin,
    read_in_int, 
    read_in_float,
    read_in_symbol, 
    read_in_quoted_symbol,
    read_separator,
    read_end
  } status = read_begin;
  
  enum {unkown, an_int, a_float, a_symbol} read_type = unkown;
  
  int fill_p = 0;
  int backslash_quoted = 0;
  unsigned char c = '\0';
  char sep = 0;
  
  while(status != read_end)
  {
    if(!IS_EOF(c))
    {
      if (f->read >= f->count)
        atomfile_read_more(f);
      
      if (f->count == 0)
        c = 0xff;
      else
        c = f->buf[f->read];
    }
    
    if (IS_BACKSLASH_QUOTE(c) && !backslash_quoted)
    {
      
      f->read++;
      backslash_quoted = 1;
    }
    else
    {
      switch (status)
      {
        case read_begin:
          if (IS_EOF(c))
          {
            /* file end before anything found */
            return 0;
          }
          else if (IS_SEPARATOR(c))
          {
            /* skip seperators at file beginning */
            f->read++;
          }
          else if (IS_DIGIT(c) || IS_SIGN(c))
          {
            /* might be an integer (might be just a "-") */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_int;
          }
          else if (IS_DOUBLE_QUOTE(c) && !backslash_quoted)
          {
            /* begin of quoted symbol */
            f->read++;
            status = read_in_quoted_symbol;
          }
          else if (IS_ATOM_CHAR(c))
          {
            buf[fill_p++] = c;
            f->read++;
            
            if(backslash_quoted)
              /* begin of symbol starting with quoted single atom char */
              status = read_in_symbol;
            else
            {
              /* end of single atom character */
              buf[fill_p++] = '\0';
              read_type = a_symbol;
              status = read_separator;
            }
          }
          else
          {
            buf[fill_p++] = c;
            f->read++;
            status = read_in_symbol;
          }
          
          break;
          
        case read_in_int:
          if (IS_EOF(c))
          {
            /* end of file */
            buf[fill_p++] = '\0';
            read_type = an_int;
            status = read_end;
          }
          else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
          {
            /* end of integer atom (do not avance the read pointer) */
            if(buf[fill_p - 1] == '-')
              read_type = a_symbol;
            else
              read_type = an_int;
            
            buf[fill_p++] = '\0';
            status = read_separator;
          }
          else if (IS_DIGIT(c))
          {
            /* continue with integer */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_int;
          }
          else if (IS_POINT(c))
          {
            /* change to float */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_float;
          }
          else
          {
            /* change to symbol */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_symbol;
          }
          break;
          
        case read_in_float:
          if (IS_EOF(c))
          {
            /* end of file */
            buf[fill_p++] = '\0';
            read_type = a_float;
            status = read_end;
          }
          else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
          {
            /* end of float atom (do not avance the read pointer) */
            buf[fill_p++] = '\0';
            read_type = a_float;
            status = read_separator;
          }
          else if (IS_DIGIT(c)  ||  IS_EXP(c)  ||  IS_SIGN(c))
          {
            /* go on in float */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_float;
          }
          else
          {
            /* change to symbol */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_symbol;
          }
          break;
          
        case read_in_symbol:
          if (IS_EOF(c))
          {
            /* end of file */
            buf[fill_p++] = '\0';
            read_type = a_symbol;
            status = read_end;
          }
          else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
          {
            /* end of symbol atom (do not avance the read pointer) */
            buf[fill_p++] = '\0';
            read_type = a_symbol;
            status = read_separator;
          }
          else
          {
            /* go on in symbol */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_symbol;
          }
          break;
          
        case read_in_quoted_symbol:
          if (IS_EOF(c))
          {
            /* end of file */
            buf[fill_p++] = '\0';
            read_type = a_symbol;
            status = read_end;
          }
          else if (IS_DOUBLE_QUOTE(c) && !backslash_quoted)
          {
            /* end of quoted symbol */
            buf[fill_p++] = '\0';
            f->read++;
            read_type = a_symbol;
            status = read_separator;
          }
          else
          {
            /* go on in quoted symbol */
            buf[fill_p++] = c;
            f->read++;
            status = read_in_quoted_symbol;
          }
          break;
          
        case read_separator:
          if(IS_SEPARATOR(c))
          {
            switch(sep)
            {  
              case 0:
                if(c == ' ')
                {
                  sep = ' ';          
                  break;
                }
              case ' ':
                if(c == '\r')
                {
                  sep = '\r';
                  break;
                }
              case '\r':
                if(c == '\t')
                {
                  sep = '\t';
                  break;
                }
              case '\t':
                if(c == '\n')
                {
                  sep = '\n';
                  break;
                }
            }
            
            f->read++;
          }
          else
            status = read_end;
          break;
          
        case read_end:
          break;
      }
      
      if(backslash_quoted)
        backslash_quoted = 0;
    }
  }
  
  switch (read_type)
  {
    case an_int:
    {
      int l;
      
      sscanf(buf, "%d", &l);
      fts_set_int(at, l);
    }
      break;
      
    case a_float:
    {
      float f;
      
      sscanf(buf, "%f", &f);
      fts_set_float(at, f);
    }
      break;
      
    case a_symbol:
      if(strcmp("()", buf) == 0)
        fts_set_void(at);
      else
        fts_set_symbol(at, fts_new_symbol(buf));
      break;
      
    default:
      break;
  }
  
  *separator = sep;
  
  return 1; /* 1 is for atom found */
}

/* return zero if the atom was not writable, like a pointer or so.
The separator char is added after the symbol; 
it must be a valid (IS_SEPARATOR(c)) chars, otherwise is substituted by a blank. */
int
fts_atomfile_write(fts_atomfile_t *f, const fts_atom_t *at, char separator)
{
  char buf[1024];
  long offset = 0;
  long n_write = 0;
  
  if (!IS_SEPARATOR(separator))
    separator = ' ';
  
  if (fts_is_int(at))
    sprintf(buf, "%ld%c", fts_get_int(at), separator);
  else if (fts_is_float(at))
    sprintf(buf, "%#g%c", fts_get_float(at), separator);
  else if (fts_is_symbol(at))
  {
    const char *sym_str = fts_symbol_name(fts_get_symbol(at));
    
    int needs_double_quotes = 0;
    char *buf_ptr = buf;
    
    *buf_ptr++ = '\"';
    offset = 1;
    
    while(*sym_str != '\0')
    {
      if (IS_SEPARATOR(*sym_str) || IS_DIGIT(*sym_str))
        needs_double_quotes = 1;
      else if (IS_BACKSLASH_QUOTE(*sym_str) || IS_DOUBLE_QUOTE(*sym_str))
      {
        needs_double_quotes = 1;
        *(buf_ptr++) = '\\';
      }
      
      *buf_ptr++ = *sym_str++;
    }
    
    if(needs_double_quotes)
    {
      /* set double quotes at beginning and end of string */
      offset = 0;
      *buf_ptr++ = '\"';
    }
    
    /* concatenate speparator */
    *buf_ptr++ = separator;
    
    /* set end of string */
    *buf_ptr++ = '\0';
  }
    else
      sprintf(buf, "()%c", separator);
    
    n_write = strlen(buf) - offset;
    fwrite(buf + offset, 1, n_write, f->file);
    
    return 1;
}

void
fts_atomfile_import_handler(fts_class_t *cl, fts_method_t meth)
{
  fts_class_import_handler(cl, fts_new_symbol("text"), meth);
  fts_class_import_handler(cl, fts_new_symbol("txt"), meth);
}

void
fts_atomfile_export_handler(fts_class_t *cl, fts_method_t meth)
{
  fts_class_export_handler(cl, fts_new_symbol("text"), meth);
  fts_class_export_handler(cl, fts_new_symbol("txt"), meth);
}

/*******************************************************************************
 *
 *  atom file dumper
 *
 */
typedef struct 
{
  fts_dumper_t head;
  fts_atomfile_t *file;
} fts_atomfile_dumper_t;

fts_class_t *fts_atomfile_dumper_class = NULL;

static fts_method_status_t
atomfile_dumper_send(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_dumper_t *self = (fts_atomfile_dumper_t *)o;
  fts_atom_t a;
  int i;
  
  fts_set_symbol(&a, s);
  
  if(ac > 0)
  {
    fts_atomfile_write(self->file, &a, ' ');
    
    for(i=0; i<ac-1; i++)
      fts_atomfile_write(self->file, at + i, ' ');
    
    fts_atomfile_write(self->file, at + i, '\n');
  }
  else
    fts_atomfile_write(self->file, &a, '\n');
  
  return fts_ok;
}

static fts_method_status_t
atomfile_dumper_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_dumper_t *self = (fts_atomfile_dumper_t *)o;
  
  fts_dumper_init((fts_dumper_t *)self, atomfile_dumper_send);
  
  self->file = NULL;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);
    fts_atomfile_t *file = fts_atomfile_open_write(name);
    
    if(file != NULL)
      self->file = file;
    else
      fts_object_error(o, "cannot open file '%s'", fts_symbol_name(name));
  }
  else
    fts_object_error(o, "file name argument required");
  
  return fts_ok;
}

static fts_method_status_t
atomfile_dumper_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_dumper_t *self = (fts_atomfile_dumper_t *)o;
  
  if(self->file != NULL)
    fts_atomfile_close(self->file);
  
  return fts_ok;
}

static void
atomfile_dumper_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_atomfile_dumper_t), atomfile_dumper_init, atomfile_dumper_delete);
}

/*******************************************************************************
 *
 *  atom file loader
 *
 */
typedef struct 
{
  fts_loader_t head;
  fts_atomfile_t *file;
} fts_atomfile_loader_t;

fts_class_t *fts_atomfile_loader_class = NULL;

static fts_method_status_t
atomfile_loader_load(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_loader_t *self = (fts_atomfile_loader_t *)o;  
  fts_atomfile_t *file = self->file;
  fts_message_t *mess = NULL;
  fts_atom_t a;
  char c;
  
  while(fts_atomfile_read(file, &a, &c))
  {
    if(mess == NULL)
    {
      if(fts_is_symbol(&a))
        mess = fts_loader_message_get((fts_loader_t *)self, fts_get_symbol(&a));
    }
    else
    {
      fts_message_append(mess, 1, &a);
      
      if(c == '\n' || c == '\r')
      {
        fts_loader_message_send((fts_loader_t *)self, mess);
        mess = NULL;
      }
    }
  }

  return fts_ok;
}

static fts_method_status_t
atomfile_loader_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_loader_t *self = (fts_atomfile_loader_t *)o;  
  
  fts_loader_init((fts_loader_t *)self, atomfile_loader_load);
  
  self->file = NULL;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);
    fts_atomfile_t *file = fts_atomfile_open_read(name);
    
    if(file != NULL)
      self->file = file;
    else
      fts_object_error(o, "cannot open file '%s'", fts_symbol_name(name));
  }
  else
    fts_object_error(o, "file name argument required");
  
  return fts_ok;
}

static fts_method_status_t
atomfile_loader_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atomfile_loader_t *self = (fts_atomfile_loader_t *)o;
  
  if(self->file != NULL)
    fts_atomfile_close(self->file);
  
  return fts_ok;
}

static void
atomfile_loader_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_atomfile_loader_t), atomfile_loader_init, atomfile_loader_delete);
}

/*******************************************************************************
 *
 *  kernel init
 *
 */
FTS_MODULE_INIT(atomfile)
{
  fts_atomfile_dumper_class = fts_class_install(NULL, atomfile_dumper_instantiate);
  fts_atomfile_loader_class = fts_class_install(NULL, atomfile_loader_instantiate);    
}
