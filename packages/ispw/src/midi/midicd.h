/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
