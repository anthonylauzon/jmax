#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/loader.h"

/* Private structure */
typedef struct fts_binary_file_desc_t {
  int fd;
  unsigned char *code;
  fts_symbol_t *symbols;
} fts_binary_file_desc_t;

static int fts_binary_file_map( const char *name, fts_binary_file_desc_t *desc)
{
  int fd;
  fts_binary_file_header_t header;
  off_t file_size;
  int symbols_size;
  char *symbuf;

  /* open the file */
  fd = open( name, O_RDONLY);
  if ( fd < 0)
    {
      perror( "fts_binary_file_map");
      return fd;
    }
  desc->fd = fd;

  /* get file size */
  {
    struct stat buf;

    if ( fstat(fd, &buf) < 0)
      {
	perror( "fts_binary_file_map");
	return -1;
      }
    file_size = buf.st_size;
  }

  /* read the header */
  if (read( fd, &header, sizeof( header)) < sizeof( header))
    {
      perror( "fts_binary_file_map");
      return -1;
    }

  if (header.magic_number != FTS_BINARY_FILE_MAGIC)
    {
      return -1;
    }

  /* allocate code and symbols */
  desc->code = (unsigned char *)fts_malloc( header.code_size);
  if (!desc->code)
    {
      return -1;
    }

  desc->symbols = (fts_symbol_t *)fts_malloc( header.n_symbols * sizeof( fts_symbol_t));
  if (!desc->symbols)
    {
      return -1;
    }

  /* read the code */
  if (read( fd, desc->code, header.code_size) < header.code_size)
    {
      perror( "fts_binary_file_map");
      return -1;
    }

  /* allocate temporary memory for the symbol table */
  symbols_size = file_size - lseek( fd, 0, SEEK_CUR);

  symbuf = (char *) fts_malloc( symbols_size);
  if ( !symbuf)
    {
      return -1;
    }

  /* read the symbol table */
  if (read( fd, symbuf, symbols_size) < symbols_size)
    {
      perror( "fts_binary_file_map");
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
	    return -1;
	  }
	desc->symbols[i] = symbol;
	i++;

	/* advance to next symbol */
	while (*p++)
	  ;
      }
  }
  fts_free( symbuf);

  return 1;
}

static void fts_binary_file_dispose( fts_binary_file_desc_t *desc)
{
  close( desc->fd);
  fts_free( desc->code);
  fts_free( desc->symbols);
}

/* Return the top of the object stack, usually the last object created
   at top level (again, usually the top level patcher, but can be different
   for clipboards).
   */

fts_object_t *fts_binary_file_load( const char *name, fts_object_t *parent)
{
  fts_object_t *obj;
  fts_binary_file_desc_t desc;

  if (fts_binary_file_map( name, &desc) < 0)
    {
      post("fts_binary_file_load: Cannot map bmax file %s\n", name);
      return 0;
    }

  obj = fts_run_mess_vm(parent, desc.code, desc.symbols);

  if (obj == 0)
    post("fts_binary_file_load: VM return null for %s\n", name);

  fts_binary_file_dispose( &desc);

  return obj;
}

