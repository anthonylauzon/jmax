#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/loader.h"

/* Private structure */
typedef struct fts_binary_file_desc_t {
  int fd;
  size_t length;
  void *address;
  fts_word_t *code;
  fts_symbol_t *symbols;
} fts_binary_file_desc_t;

static int fts_binary_file_map( const char *name, fts_binary_file_desc_t *desc)
{
  int fd;
  void *address;
  fts_binary_file_header_t *header;
  int symbols_size;

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
    desc->length = buf.st_size;
  }

  /* map the file */
  address = mmap( NULL, desc->length, PROT_READ, MAP_PRIVATE, fd, 0);
  if (address == MAP_FAILED)
    {
      perror( "fts_binary_file_map");
      return -1;
    }
  desc->address = address;
    
  /* get the header */
  header = (fts_binary_file_header_t *)address;

  if (header->magic_number != FTS_BINARY_FILE_MAGIC)
    return -1;

  /* get the code */
  desc->code = (fts_word_t *)((char *)address + sizeof( fts_binary_file_header_t));

  /* allocate temporary memory for the symbol table */
  desc->symbols = (fts_symbol_t *)fts_malloc( header->n_symbols * sizeof( fts_symbol_t));
  if (!desc->symbols)
    {
      return -1;
    }

  /* enter the symbols in the global symbol table */
  {
    char *p;
    int i;

    i = 0;
    p = (char *)address + sizeof( fts_binary_file_header_t) + header->code_size*sizeof(long);
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

  return 1;
}

static void fts_binary_file_dispose( fts_binary_file_desc_t *desc)
{
  munmap( desc->address, desc->length);
  close( desc->fd);
  fts_free( desc->symbols);
}

int fts_binary_file_load( const char *name)
{
  fts_binary_file_desc_t desc;

  if (fts_binary_file_map( name, &desc) < 0)
    return -1;

  fts_run_mess_vm( desc.code, desc.symbols);

  fts_binary_file_dispose( &desc);

  return 1;
}

