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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

/* Converted by MDC to use FILE * instead of file,
   in order to integrate it better with the loader;
   in particular, the clipboard will is now an unlinked, invisible
   file, never closed until exit, with fseek/fread/fwrite
   operations on it (to avoid leaving .clipboard file arounds)
   */


/* Private structure */

typedef struct fts_binary_file_desc_t
{
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

static int fts_binary_file_map( FILE *f, fts_binary_file_desc_t *desc)
{
  fts_binary_file_header_t header;

  /* read the header */

  if (fread( &header, sizeof( header), 1, f) < 1)
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_map");
#endif
      return -1;
    }

  if (has_to_swap())
    {
      swap_long( &header.magic_number);
      swap_long( &header.code_size);
      swap_long( &header.n_symbols);
    }

  if (header.magic_number != FTS_BINARY_FILE_MAGIC)
    return -1;

  /* allocate code */

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading %d bytecodes\n", header.code_size);
#endif

  desc->code = (unsigned char *)fts_malloc( header.code_size);
  if (!desc->code)
    return -1;

  /* read the code */

  if (fread( desc->code, sizeof(char), header.code_size, f) < header.code_size)
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_map");
#endif
      return -1;
    }

  /* allocate code and read symbols */

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading %d symbols\n", header.n_symbols);
#endif

  if (header.n_symbols > 0)
    {
      int i = 0;
      char buf[1024]; /* max  symbol size */
      int symbolIndex = 0;

      desc->symbols = (fts_symbol_t *)fts_malloc( header.n_symbols * sizeof( fts_symbol_t));

      /* In case of corrupted file, we initialize the
	 table with the error symbol, so to have some hope
	 of opening the result */

      for (symbolIndex = 0; symbolIndex < header.n_symbols; symbolIndex++)
	desc->symbols[symbolIndex] = fts_s_error;

      symbolIndex = 0;
      while (! feof(f) && (symbolIndex < header.n_symbols))
	{
	  buf[i] = getc(f);

	  if (buf[i] == 0)
	    {
	      desc->symbols[symbolIndex]= fts_new_symbol_copy(buf);
#ifdef LOAD_DEBUG
	      fprintf(stderr, "Reading symbol %s\n", fts_symbol_name(desc->symbols[symbolIndex]));
#endif
	      symbolIndex++;
	      i = 0;
	    }
	  else
	    i++;
	}
    }

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
  FILE *f;
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

#ifdef LOAD_DEBUG
  fprintf(stderr, "Reading binary file %s\n", name);
#endif

    /* open the file */

  f = fopen( name, "r");

  if ( f == 0)
    {
#ifdef LOAD_DEBUG
      perror( "fts_binary_file_load");
#endif
      return 0;
    }

  if (fts_binary_file_map(f, &desc) < 0)
    {
      fclose(f);
      post("fts_binary_file_load: Cannot load jMax max file %s\n", name);
      return 0;
    }

  fclose(f);

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols, ac, at, e);

  fts_binary_file_dispose( &desc);

  return obj;
}


/* Return the top of the object stack, usually the last object created
   at top level (again, usually the top level patcher, but can be different
   for clipboards).
   */

fts_object_t *fts_binary_filedesc_load(FILE *f,
				       fts_object_t *parent,
				       int ac, const fts_atom_t *at,
				       fts_expression_state_t *e)
{
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

  /* Rewind the file */

  fseek(f, 0, SEEK_SET);

  /* Read it */

  if (fts_binary_file_map(f, &desc) < 0)
    return 0;

  /* Eval it */

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols, ac, at, e);

  fts_binary_file_dispose( &desc);

  return obj;
}



