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

#ifndef _ATOMLIST_H_
#define _ATOMLIST_H_

/*****************************************************************************/
/*                                                                           */
/* Atom lists                                                                */
/*                                                                           */
/*****************************************************************************/

/*
  Structure: fts_atom_list_t

  Description:
    A list of atoms.
*/

struct fts_atom_list_cell;

typedef struct fts_atom_list
{
  fts_data_t dataobj;
  struct fts_atom_list_cell *head;
  struct fts_atom_list_cell *tail;
} fts_atom_list_t;

/*
  Function: fts_atom_list_init
  Description:
    Initializes an atom list
  Arguments:
    [out] list   a pointer to the atom list
  Returns:
    none
*/
extern void fts_atom_list_init( fts_atom_list_t *list);

/*
  Function: fts_atom_list_destroy
  Description:
    De-initializes an atom list
    Mainly frees the memory internally allocated
  Arguments:
    [out] list   a pointer to the atom list
  Returns:
    none
*/
extern void fts_atom_list_destroy( fts_atom_list_t *list);

/*
  NOT DOCUMENTED
  fts_atom_list_new
*/
extern fts_atom_list_t *fts_atom_list_new( void);

/*
  NOT DOCUMENTED
  fts_atom_list_free
*/
extern void fts_atom_list_free( fts_atom_list_t *list);

/*
  Function: fts_atom_list_clear
  Description:
    Clears the content of an atom list
    After calling this function, the atom list will be in the same state
    as after calling fts_atom_list_init
  Arguments:
    [out] list   a pointer to the atom list
  Returns:
    none
*/
extern void fts_atom_list_clear( fts_atom_list_t *list);

/*
  Function: fts_atom_list_append
  Description:
    Appends an array of atoms to an atom list.
  Arguments:
    [out] list   a pointer to the atom list
    [in]  ac     the count of atoms
    [in]  at     the array of atoms
  Returns:
    none
*/
extern void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *at);

/*
  Function: fts_atom_list_set
  Description:
    Set the content of an atom list from an array of atoms.
    Equivalent to clear+append.
  Arguments:
    [out] list   a pointer to the atom list
    [in]  ac     the count of atoms
    [in]  at     the array of atoms
  Returns:
    none
*/
extern void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *at);

/*
  NOT DOCUMENTED
  is_subsequence
*/
extern int  fts_atom_list_is_subsequence( fts_atom_list_t *list, int ac, const fts_atom_t *at);

/*
  NOT DOCUMENTED
  Save to bmax 
*/
extern void fts_atom_list_save_bmax( fts_atom_list_t *list, fts_bmax_file_t *f, fts_object_t *target);


/*****************************************************************************/
/*                                                                           */
/* Atom list iterators                                                       */
/*                                                                           */
/*****************************************************************************/

/*
  Structure: fts_atom_list_iterator_t
  Description:
    An atom list iterator.

    This is a temporary structure used to iterate other all the 
    atoms contained in an atom list, a la Java "Enumeration"

    The following code is an example of use of a fts_atom_list_iterator_t:

      fts_atom_list_iterator_init( &iter, table);

      while( !fts_atom_list_iterator_end( &iter))
        {
          void *p = fts_get_ptr( fts_atom_list_iterator_current( &iter));

	  fts_free( p);

	  fts_atom_list_iterator_next( &iter);
	}
*/

typedef struct fts_atom_list_iterator
{
  struct fts_atom_list_cell *cell;
} fts_atom_list_iterator_t;


/*
  Function: fts_atom_list_iterator_init
  Description:
    Initializes an atom list iterator
  Arguments:
    [out] iter   a pointer to the iterator to initialize
    [in] list    a pointer to the atom list on which to iterate
  Returns:
    none
*/
extern void fts_atom_list_iterator_init( fts_atom_list_iterator_t *iter, const fts_atom_list_t *list);

/*
  Function: fts_atom_list_iterator_next
  Description:
    Advance to the next atom
  Arguments:
    [out] iter   a pointer to the iterator
  Returns:
    none
*/
extern void fts_atom_list_iterator_next( fts_atom_list_iterator_t *iter);

/*
  Function: fts_atom_list_iterator_end
  Description:
    Tests if the iterator has reached end of atom list
  Arguments:
    [in] iter   a pointer to the iterator
  Returns:
    1 if iterator is at end of atom list, 0 if not
*/
extern int fts_atom_list_iterator_end( const fts_atom_list_iterator_t *iter);

/*
  Function: fts_atom_list_iterator_current
  Description:
    Access to the current atom in the iterator
  Arguments:
    [in] iter   a pointer to the iterator
  Returns:
    the current atom (a pointer to a fts_atom_t)
*/
extern fts_atom_t *fts_atom_list_iterator_current( const fts_atom_list_iterator_t *iter);

#endif
