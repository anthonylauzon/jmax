/* $Id$
 *
 * jMax
 * Copyright (C) 1994-2002 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 */

/*
 * This file's authors:  Diemo Schwarz, 20.8.2002
 */

/* 
 * $Log$
 */


#ifndef _JMAXSDIF_H_
#define _JMAXSDIF_H_


#include <fts/fts.h>
#include <sdif.h>

/* heuristically determine jmax version */
#ifdef FTS_VERSION_STRING
#   define JMAXSDIF_JMAX3 0
#   define jmaxsdif_get_time()	fts_get_time()
#else
#   define JMAXSDIF_JMAX3 1
#   define jmaxsdif_get_time()	fts_timebase_get_time(timebase)
#endif

#define JMAXSDIFVERSION "0_2"


typedef enum { state_stopped, state_running } jmaxsdif_state_t;

typedef struct
{
    fts_object_t	o;		/* mandatory fts base class */
    char *		objectname;	/* who are we? */
    jmaxsdif_state_t	state;
    char *		filename;
    SdifFileT *		file;		/* NULL if not open */
} jmaxsdif_t;


#if JMAXSDIF_JMAX3
extern fts_timebase_t *timebase;
#endif
extern fts_symbol_t readsdif_symbol;
extern fts_symbol_t writesdif_symbol;
extern fts_symbol_t s_define;
extern fts_symbol_t s_open;
extern fts_symbol_t s_record;
extern fts_symbol_t s_close;


/* prototypes */
void sdif_config(void);
void jmaxsdif_exit(void);
void jmaxsdif_errorhandler(int		    error_tag, 
			   SdifErrorLevelET error_level, 
			   char*	    error_message, 
			   SdifFileT*	    error_file, 
			   SdifErrorT*	    error_ptr, 
			   char*	    source_file, 
			   int		    source_line);
int  jmaxsdif_closefile(jmaxsdif_t *this);

fts_status_t readsdif_instantiate(fts_class_t *, int, const fts_atom_t *);
fts_status_t writesdif_instantiate(fts_class_t *, int, const fts_atom_t *);


#endif /* ifndef _JMAXSDIF_H_ */
