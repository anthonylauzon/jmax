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

/*======================================================================*/
/*									*/
/*	mididecrypt - Objet externe MAX -				*/
/*	Concu pour des echanges de donnees entre Mac et Next via Midi.	*/
/*									*/
/*	IRCAM - LEFEVRE Adrien - Decembre 1994 - VERSION NEXT		*/
/*									*/
/*======================================================================*/


#include <fts/fts.h>

#define M_FORMAT    80		/*  Nombre max. d'arg. de la liste format */
#define M_DATA  4+5*M_FORMAT	/*  Nombre max. d'octets du buffer Midi */

#define SYSEX_S	    0xF0	/*  Octet de debut de message exclusif Midi */
#define SYSEX_E	    0xF7	/*  Octet de fin de message exclusif Midi */
#define SYSEX_CC    0x7D	/*  Code Constructeur du message exclusif Midi */

#define SYSEX_F	    0x40	/*  Rang du bit dans le 2nd Septet indiquant */
                                /* un message de Format */
#define FORMAT_MIN  1		/*  Valeur minimum des arguments dans format */
#define FORMAT_MAX  30		/*  Valeur maximum des arguments dans format */

#define NOM  "mididecrypt"

typedef struct decrypt {
  fts_object_t o;
  fts_atom_t lstout[M_FORMAT];
  int chn;
  int enb;
  char Buf[M_DATA];
  char Format[M_FORMAT];
  int nb_elm, pt_buf;
  long Elm[M_FORMAT];
} decrypt_t;

/*--------------------------------------------------------------------------*/
/* Helper functions                                                         */
/*--------------------------------------------------------------------------*/

static char App2( long v, long min, long max)
{
  if(v < min)
    v = min;
  if(v > max)
    v = max;

  return(v);
}

static void Chanel2( decrypt_t *x, long n)
{
  n = App2( n, 0, 16);

  if(n == 0)
    x->chn = 16;
  else
    x->chn = n - 1;
}

static void out_list( decrypt_t *x)
{
  int i;

  for( i = 0; i < x->nb_elm; i++)
    fts_set_int( &(x->lstout[i]), x->Elm[i]);

  fts_outlet_varargs( (fts_object_t *)x, 0, x->nb_elm, x->lstout);
}

static void Calcul2( decrypt_t *x)
{
  char f,p,k,n;
  int i,j;
  long g,m,s;

  if( x->Buf[1] & SYSEX_F)
    {
      n = App2( x->Buf[2], 0, 127);
      x->nb_elm = x->pt_buf-3;

      for( i = 3; i < x->pt_buf; i++)
	x->Format[i-3] = App2( x->Buf[i], FORMAT_MIN, FORMAT_MAX);

      fts_outlet_int( (fts_object_t *)x, 1, n);
    }
  else
    {
      j = 2;
      k = 7;  /* pointeur sur bit dans x->Buf[j] */
      for( i = 0; i < x->nb_elm; i++)
	{
	  f = x->Format[i];
	  s = 0;
	  p = f+1; /* pointeur sur bit dans s */

	  while( p > k)
	    {
	      m = x->Buf[j++] & ((1 << k) - 1);
	      s += m << (p-k);
	      p -= k;
	      k = 7;
	    }

	  s += (x->Buf[j] >> (k-p)) & ((1 << p) - 1);
	  k -= p;

	  if(k == 0)
	    {
	      j++;
	      k = 7;
	    }

	  g = s >> f;
	  s -= g << f;

	  if (g)
	    s *= -1;

	  x->Elm[i] = s;
	}

      out_list( x );
    }
}

/*--------------------------------------------------------------------------*/
/* Method bang                              				    */
/*--------------------------------------------------------------------------*/

static void decrypt_bang( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  decrypt_t *this = (decrypt_t *)o;

  if( this->enb)
    out_list( this);
}

/*--------------------------------------------------------------------------*/
/* Method int                               				    */
/*--------------------------------------------------------------------------*/

static void decrypt_int( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  decrypt_t *this = (decrypt_t *)o;
  int n = fts_get_int( &at[0]);

  if ( this->enb)
    switch(n) {
    case SYSEX_S:
      this->pt_buf = 0;
      break;

    case SYSEX_E:
      if ( (this->Buf[0] == SYSEX_CC)
	   &&( (this->chn == (this->Buf[1] & 0x0F))
	       || (this->chn  & 0x10)
	       || (this->Buf[1] & 0x10)
	       ))
	{
	  Calcul2(this);
	}
      break;

    default:
      if(this->pt_buf < M_DATA)
	this->Buf[ this->pt_buf++ ] = App2( n, 0, 127);
    }
}

/*--------------------------------------------------------------------------*/
/* Method format                            				    */
/*--------------------------------------------------------------------------*/

static void decrypt_format( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  decrypt_t *this = (decrypt_t *)o;
  char n;
  int i;

  if (( ac != 0) && (this->enb))
    {
      if ( ac > M_FORMAT+1 )
	ac = M_FORMAT+1;

      for( i = 0; i < ac; i++)
	if ( !fts_is_int( &at[i]))
	  {
	    post( "%s: arguments du message <format> non valides", NOM);
	    return;
	  }

      n = App2( fts_get_int( &at[0]), 0, 127);

      if ( ac > 1)
	{
	  this->nb_elm = ac-1;

	  for( i = 1; i< ac; i++)
	    this->Format[i-1] = App2( fts_get_int( &at[i]), FORMAT_MIN, FORMAT_MAX);
	  
	  fts_outlet_int( o, 1, n);
	}
    }
}

/*--------------------------------------------------------------------------*/
/* Method enable                            				    */
/*--------------------------------------------------------------------------*/

static void decrypt_enable( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  decrypt_t *this = (decrypt_t *)o;

  this->enb = ( fts_get_int( &at[0]) > 0);
}


/*--------------------------------------------------------------------------*/
/* Method chn                                  				    */
/*--------------------------------------------------------------------------*/

static void decrypt_chn( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Chanel2( (decrypt_t *)o, fts_get_int( &at[0]));
}


/*--------------------------------------------------------------------------*/
/* Method init                                                              */
/*--------------------------------------------------------------------------*/

static void decrypt_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  decrypt_t *this = (decrypt_t *)o;
  int i;

  this->chn = 0;
  this->enb = 1;
  this->pt_buf = 0;
  this->nb_elm = 1;

  for( i = 0; i < M_DATA;  i++)
    this->Buf[i]  = 0;

  for( i = 0; i < M_FORMAT; i++)
    this->Format[i] = 8;

  for( i = 0; i < M_FORMAT; i++)
    this->Elm[i]  = 0;

  if( ac > 0)
    {
      if( ac > M_FORMAT+1)
	ac = M_FORMAT+1;

      for(i = 1; i < ac; i++)
	{
	  if (!fts_is_int(at + i))
	    {
	      post("%s: argument %d non valide (%s)\n", NOM, i, fts_get_class_name( &at[i]));
	      return;
	    }
	}

      Chanel2( this, fts_get_int(at));

      if (ac > 1)
	{
	  this->nb_elm = ac - 1;
	  for(i=1; i<ac; i++)
	    this->Format[i - 1] = App2(fts_get_int(at + i), FORMAT_MIN, FORMAT_MAX);
	}
    }
}

/*--------------------------------------------------------------------------*/
/* Class instantiation                                                      */
/*--------------------------------------------------------------------------*/

static void decrypt_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(decrypt_t), decrypt_init, 0);

  fts_class_message_varargs(cl, fts_new_symbol( "enable"), decrypt_enable);
  fts_class_message_varargs(cl, fts_new_symbol( "format"), decrypt_format);

  fts_class_inlet_bang(cl, 0, decrypt_bang);
  fts_class_inlet_int(cl, 0, decrypt_int);

  fts_class_inlet_int(cl, 1, decrypt_chn);

  fts_class_outlet_varargs(cl, 0);
  fts_class_outlet_int(cl, 1);
}


void mididecrypt_config( void)
{
  fts_class_install( fts_new_symbol( "mididecrypt"), decrypt_instantiate);
}
