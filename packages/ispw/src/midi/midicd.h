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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

/*======================================================================*/
/*									*/
/*	Lib midicrypt et mididecrypt - Objets externes MAX -		*/
/*	Concu pour des echanges de donnees entre Mac et Next via Midi.	*/
/*									*/
/*	IRCAM - LEFEVRE Adrien - Decembre 1994 - VERSION NEXT		*/
/*									*/
/*======================================================================*/

#ifndef _MIDICD_H_
#define _MIDICD_H_

#define M_FORMAT    80		/*  Nombre max. d'arg. de la liste format */
#define M_DATA  4+5*M_FORMAT	/*  Nombre max. d'octets du buffer Midi */

#define SYSEX_S	    0xF0	/*  Octet de debut de message exclusif Midi */
#define SYSEX_E	    0xF7	/*  Octet de fin de message exclusif Midi */
#define SYSEX_CC    0x7D	/*  Code Constructeur du message exclusif Midi */

#define SYSEX_F	    0x40	/*  Rang du bit dans le 2nd Septet indiquant */
                                /* un message de Format */
#define FORMAT_MIN  1		/*  Valeur minimum des arguments dans format */
#define FORMAT_MAX  30		/*  Valeur maximum des arguments dans format */

#endif
