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

#include <sequence/c/include/sequence.h>

extern void seqsym_config(void);

extern void sequence_class_config(void);
extern void track_config(void);
extern void event_config(void);

extern void note_config(void);
extern void seqmess_config(void);

extern void seqfind_config(void);
extern void seqstep_config(void);
extern void seqplay_config(void);
extern void seqrec_config(void);
extern void locate_config(void);

extern void getdur_config(void);

void
sequence_config(void)
{
  seqsym_config();

  sequence_class_config();
  track_config();
  event_config();

  note_config();
  seqmess_config();

  seqfind_config();
  seqstep_config();
  seqplay_config();
  seqrec_config();
  locate_config();

  getdur_config();
}
