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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

/* #define LOAD_DEBUG  */

/* Private structure */
typedef struct fts_binary_file_desc_t {
  int fd;
  unsigned char *code;
  fts_symbol_t *symbols;
} fts_binary_file_desc_t;

union swap_union_t {
  long l;
  char c[4];
};

static int has_to_swap(void)
{
  union swap_union_t u;

  u.l = 0x11223344;
  return u.c[0] != 0x11;
}

static void swap_long( unsigned long *p)
{
  union swap_union_t *pu;
  char tmp;

  pu = (union swap_union_t *)p;
  tmp = pu->c[0];
  pu->c[0] = pu->c[3];
  pu->c[3] = tmp;
  tmp = pu->c[1];
  pu->c[1] = pu->c[2];
  pu->c[2] = tmp;
}

static int fts_binary_file_map( const char *name, fts_binary_file_desc_t *desc)
{
  int fd;
  fts_binary_file_header_t header;
  off_t file_size;
  unsigned int symbols_size;
  char *symbuf;
  int i;

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading binary file %s\n", name);
#endif

  /* open the file */
  fd = open( name, O_RDONLY);
  if ( fd < 0)
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_map");
#endif
      return fd;
    }

  /* get file size */
  {
    struct stat buf;

    if ( fstat(fd, &buf) < 0)
      {
#ifdef LOAD_DEBUG
	perror( "fts_binary_file_map");
#endif
	close(fd);
	return -1;
      }
    file_size = buf.st_size;
  }

  /* read the header */
  if (read( fd, &header, sizeof( header)) < sizeof( header))
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_map");
#endif
      close(fd);
      return -1;
    }

  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if (header.magic_number != FTS_BINARY_FILE_MAGIC)
    {
      close(fd);
      return -1;
    }

  /* allocate code */

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading %d bytecodes\n", header.code_size);
#endif

  desc->code = (unsigned char *)fts_malloc( header.code_size);
  if (!desc->code)
    {
      close(fd);
      return -1;
    }

  /* read the code */

  if (read( fd, desc->code, header.code_size) < header.code_size)
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_map");
#endif
      close(fd);
      return -1;
    }

  /* allocate code and read symbols */

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading %d symbols\n", header.n_symbols);
#endif

  if (header.n_symbols > 0)
    {
      desc->symbols = (fts_symbol_t *)fts_malloc( header.n_symbols * sizeof( fts_symbol_t));

      /* In case of corrupted file, we initialize the
	 table with the error symbol, so to have some hope
	 of opening the result */

      for (i = 0; i < header.n_symbols; i++)
	desc->symbols[i] = fts_s_error;

      if (!desc->symbols)
	{
	  close(fd);
	  return -1;
	}

      /* allocate temporary memory for the symbol table */
      symbols_size = file_size - lseek( fd, 0, SEEK_CUR);

      symbuf = (char *) fts_malloc( symbols_size);
      if ( !symbuf)
	{
	  close(fd);
	  return -1;
	}

      /* read the symbol table */
      if (read( fd, symbuf, symbols_size) < symbols_size)
	{
#ifdef LOAD_DEBUG
	  perror( "fts_binary_file_map");
#endif
	  close(fd);
	  return -1;
	}

      /* enter the symbols in the global symbol table */
      {
	char *p;
	int i;

	i = 0;
	p = symbuf;
	while (*p)
	  {
	    fts_symbol_t symbol;

	    symbol = fts_new_symbol_copy( p);

	    if ( !symbol)
	      {
#ifdef LOAD_DEBUG
		fprintf(stderr, "Reading NULL symbol\n");
#endif
		desc->symbols[i] = 0;
	      }
	    else
	      {
#ifdef LOAD_DEBUG
		fprintf(stderr, "Reading symbol %s\n", fts_symbol_name(symbol));
#endif
		desc->symbols[i] = symbol;
	      }
	    i++;
	    
	    /* advance to next symbol */
	    while (*p++)
	      ;
	  }
      }
      
      fts_free( symbuf);
    }

  close(fd);
  return 1;
}

static void fts_binary_file_dispose( fts_binary_file_desc_t *desc)
{
  fts_free( desc->code);
  fts_free( desc->symbols);
}

/* Return the top of the object stack, usually the last object created
   at top level (again, usually the top level patcher, but can be different
   for clipboards).
   */

fts_object_t *fts_binary_file_load( const char *name,
				    fts_object_t *parent,
				    int ac, const fts_atom_t *at,
				    fts_expression_state_t *e)
{
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

  if (fts_binary_file_map( name, &desc) < 0)
    {
      post("fts_binary_file_load: Cannot map bmax file %s\n", name);
      return 0;
    }

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols, ac, at, e);

  fts_binary_file_dispose( &desc);

  return obj;
}


