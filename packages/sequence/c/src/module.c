/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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

#include "internal_sequence.h"

FTS_MODULE_INIT(sequence)
{
  FTS_MODULE_INIT_CALL(seqsym);

  FTS_MODULE_INIT_CALL(sequence_class);
  FTS_MODULE_INIT_CALL(track);
	FTS_MODULE_INIT_CALL(track_editor);
  FTS_MODULE_INIT_CALL(event);

  FTS_MODULE_INIT_CALL(scoob);
  FTS_MODULE_INIT_CALL(scomark);
  FTS_MODULE_INIT_CALL(seqmess);

  FTS_MODULE_INIT_CALL(seqfind);
  FTS_MODULE_INIT_CALL(seqstep);
  FTS_MODULE_INIT_CALL(seqplay);
  FTS_MODULE_INIT_CALL(seqrec);
  FTS_MODULE_INIT_CALL(locate);
}
