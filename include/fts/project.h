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

#ifndef _FTS_PROJECT_H_
#define _FTS_PROJECT_H_

/** Allocates a new project. This function is a wrapper around
 * fts_object_create.
 *
 * @param name the name of the project
 * @return a new project or NULL in case of error
 * @ingroup project */
FTS_API fts_project_t* fts_project_new(fts_symbol_t name);

/** Destroys the ressources allocated by this project. This function
 * is a wrapper around fts_object_destroy.
 *
 * @param pkg the project
 * @ingroup project */
FTS_API void fts_project_destroy(fts_project_t* pkg);


/** Opens the project associated with a file.
 *
 * @param filename the name of the file to be opened
 * @ingroup project 
 */
FTS_API fts_project_t *fts_project_open(const char* filename);

/** Closes the current project. The current project will also be
 * destroyed.
 *
 * @return 0 if no error occured, -1 if an error occured
 * @ingroup project */
FTS_API int fts_project_close(void);

/** Returns a pointer to the current project
 *
 * @return the current project
 * @ingroup project 
 */
FTS_API fts_project_t *fts_project_get(void);

/** Sets the current project. The old project will automatically be
 *closed and destroyed.
 *
 * @param project pointer to the new project
 * @ingroup project 
 */
FTS_API void fts_project_set(fts_project_t *project);

/** Returns the directory of the current project.
 *
 * @ingroup project 
 */
FTS_API fts_symbol_t fts_project_get_dir(void);

/** Returns the full pathname of a data file of the current
 * project. The data file is searched on the projects data paths and on
 * the data paths of all its required packages.
 *
 * @param filename the name of the file to search
 * @ingroup project */
FTS_API fts_symbol_t fts_project_get_data_file(fts_symbol_t filename);


/** Returns the arguments for the default midiport.
 *
 * @ingroup project */
FTS_API void fts_project_get_default_midi(int* pac, fts_atom_t** pat);


/** Returns the arguments for the default midiport in.
 *
 * @ingroup project */
FTS_API void fts_project_get_default_midi_in(int* pac, fts_atom_t** pat);


/** Returns the arguments for the default midiport out.
 *
 * @ingroup project */
FTS_API void fts_project_get_default_midi_out(int* pac, fts_atom_t** pat);

#endif /* _FTS_PROJECT_H_ */
