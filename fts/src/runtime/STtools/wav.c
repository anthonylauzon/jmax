/* author Olivier Trystram, adapted to STtools library by Peter Hoffmann, IRCAM, 04/98 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#ifdef sgi
#include <bstring.h>
#endif

#include "wav.h"
#include "convert.h"
#include "sfiles.h"

/*---------------------------------------------------------------

 Fonctions communes aux prg WAV

 Avec les linux ou win95, on peut allouer toute la RAM et
 le swap d'un coup.
 Donc je suis sans etats d'ame pour charger tout le fichier
 en memoire.

 Philosophie generale de ces fonctions :

 Recuperer les donnees (8 ou 16 bits), les transformer en float
 et tout le travail en memoire se fait sur ce format.

 puis on les re-convertit en 8 ou 16 bits (a la mode wav ==
 -16385 et +16384) juste avant de les ecrire sur disque ( dans la
 fonction wav_datawrite() )

---------------------------------------------------------------*/


/*---------------------------------------------------------------
 lit le header d'un fichier wav
 Attention : retour au debut du fichier

 si un chunk inconnu suit le header proprement, on part a la
 recherche du vrai debut des data en oubliant les chunk inconnus.
 Du coup, dans la structure wavhdr, on modifie ce qui concerne
 les data, mais pas ce qui concerne lenriff qui est tres lie au
 fichier (et a sa taille).

 la fonction retourne le nombre d'octets lus.
---------------------------------------------------------------*/

int wavhdr_read( int fd, WAVHDR * wav )
{
  char str[10], ch, found;
  DWORD c;
  short i;

  /* ... */

  /* 
     the simple reading of the header has been replaced by the 
     (much more complicated) STreadWAVheader routines below
     */

  /* ... */

  /*
    pour virer certain chunks inconnus
    (recorder.exe en cree)
    */
  found = 0;
  while ( found == 0 ) {

    for (i=0; i<4; i++ ) {
      str[i] = wav->data[i];
    }
    str[4] = '\0';
    if ( strcmp( str, "data" ) != 0 ) {

      found = 0;
      /* no stdout messages in piped programs! */
      fprintf(stderr, "wav (wavhdr_read): Skipping unknown chunk (8+%ld bytes). \n", wav->lendata);
      for (c=0; c<wav->lendata; c++ ) {
	STread( fd, &ch, 1);
      }

      wav->lenriff -= (8+wav->lendata);

      /* lecture de l'entete du chunk suivant */

      if (STread(fd, (char *)wav->data, sizeof(wav->data)) != sizeof(wav->data) ) {
	fprintf(stderr, "wav (wavhdr_read): cannot read WAV data chunk id!\n");
	return -1;
      }
      if (STreadLittleEndianDWORD(fd, &wav->lendata) != sizeof(wav->lendata)) {
	fprintf(stderr, "wav (wavhdr_read): cannot read WAV data chunk length!\n");
	return -1;
      }

    } else {
      found = 1;
    }
  }
  return 0;
}

/* unfortunately, it is more complicated than this (see STreadWAVheader below): */

/*---------------------------------------------------------------
 ecrit le header d'un fichier wav
 Attention : retour au debut du fichier
---------------------------------------------------------------*/
/*
void wavhdr_write( FILE *outf, struct WAVHDR * wav )
{
 rewind( outf );
 fwrite( wav, WAVHDRLEN, 1, outf );
}
*/

/* the following function is already covered by the more general querysf tool */
/*---------------------------------------------------------------
 affiche le contenu d'un header de wav
---------------------------------------------------------------*/
void wavhdr_print( WAVHDR wav )
{
  int i;
  char str[10];

  printf("-------- Contenu du header WAV (%d octets) ----------- \n", sizeof( WAVHDR ) );
  for (i=0; i<4; i++ ) {
    str[i] = wav.RIFF[i];
  }
  str[4] = '\0';
  printf("RIFF = [%s]\nlenriff = %lu\n", str, wav.lenriff );

  for (i=0; i<8; i++ ) {
    str[i] = wav.WAVfmt[i];
  }
  str[8] = '\0';

  printf("[WAVfmt] = [%s]\nlenhead = %lu\nwFormatTag = %u\n", str, wav.lenhead, wav.wFormatTag );
  printf("nChannels = %u\nnSamplesPerSec = %lu\nnAvgBytesPerSec = %lu\n", wav.nChannels, wav.nSamplesPerSec, wav.nAvgBytesPerSec );
  printf("nBlockAlign = %u\nnBitsPerSamples = %u\n", wav.nBlockAlign, wav.nBitsPerSamples );

  for (i=0; i<4; i++ ) {
    str[i] = wav.data[i];
  }
  str[4] = '\0';
  printf("\ndata = [%s]\nlendata = %lu\n", str, wav.lendata );

}


/* this could be interesting to use somewhen in the future. */
/*---------------------------------------------------------------
  verifie le contenu d'un header de wav
  ---------------------------------------------------------------*/
int wavhdr_verif( WAVHDR wav )
{
  int i;
  char str[10];

  for (i=0; i<4; i++ ) {
    str[i] = wav.RIFF[i];
  }
  str[4] = '\0';
  if ( strcmp( str, "RIFF" ) != 0 ) {
    printf("err. : Not a RIFF File.\n");
    return(1);
  }

  for (i=0; i<8; i++ ) {
    str[i] = wav.WAVfmt[i];
  }
  str[8] = '\0';
  if ( strcmp( str, "WAVEfmt " ) != 0 ) {
    printf("err. : Header doesn't contain 'WAVEfmt '.\n");
    return(1);
  }

  if ( wav.lenhead != 0x10 ) {
    printf("err. : lenhead <> 0x10\n");
    return(1);
  }

  if ( wav.wFormatTag != 1 ) {
    printf("err. : wFormatTag <> 1 (PCM)\n");
    return(1);
  }

  if ( (wav.nChannels != MONO ) && (wav.nChannels != STEREO ) ) {
    printf("err. : nChannels <> 1 or 2.\n");
    return(1);
  }

  /* pas sur qu'il faille laisser ce test */
  if ( (wav.nSamplesPerSec != 11025) &&
       (wav.nSamplesPerSec != 22050) &&
       (wav.nSamplesPerSec != 44100) ) {
    printf("err. : nSamplesPerSec <> 11025 or 22050 or 44100.\n");
    return(1);
  }

  if ( wav.nBlockAlign != ((wav.nBitsPerSamples/8) * wav.nChannels) ) {
    printf("err. : Inconsistant nBlockAlign\n");
    return(1);
  }

  if ( wav.nAvgBytesPerSec != wav.nSamplesPerSec * wav.nBlockAlign ) {
    printf("err. : Inconsistant nAvgBytesPerSec\n");
    return(1);
  }

  if ( ( wav.nBitsPerSamples != 8 ) &&
       ( wav.nBitsPerSamples != 16 ) ) {
    printf("err. : nBitsPerSamples <> 8 or 16.\n");
    return(1);
  }

  for (i=0; i<4; i++ ) {
    str[i] = wav.data[i];
  }
  str[4] = '\0';
  if ( strcmp( str, "data" ) != 0 ) {
    printf("err. : Header doesn't contain 'data'.\n");
    return(1);
  }

  return 0;

}

/* used by the wrappers below */
/*---------------------------------------------------------------
  creation d'un header de wav
  ---------------------------------------------------------------*/
void wavhdr( WAVHDR * wav,
	     WORD nChan,
	     DWORD nBytes,
	     DWORD Taux_Echant,
	     WORD BitsPerSample )
{
  wav->RIFF[0] = 'R';
  wav->RIFF[1] = 'I';
  wav->RIFF[2] = wav->RIFF[3] = 'F';

  wav->lenriff = nBytes /* * (BitsPerSample/8) * nChan) */ + WAVHDRLEN - 8;

  wav->WAVfmt[0] = 'W';
  wav->WAVfmt[1] = 'A';
  wav->WAVfmt[2] = 'V';
  wav->WAVfmt[3] = 'E';
  wav->WAVfmt[4] = 'f';
  wav->WAVfmt[5] = 'm';
  wav->WAVfmt[6] = 't';
  wav->WAVfmt[7] = ' ';

  wav->lenhead = 0x10L;
  wav->wFormatTag = 1;

  wav->nChannels = nChan;
  wav->nSamplesPerSec = Taux_Echant;
  wav->nBitsPerSamples = BitsPerSample;

  wav->nBlockAlign = (BitsPerSample/8) * nChan ;
  wav->nAvgBytesPerSec = Taux_Echant * wav->nBlockAlign;

  wav->data[0] = 'd';
  wav->data[1] = 'a';
  wav->data[2] = 't';
  wav->data[3] = 'a';
  wav->lendata = nBytes;
}

/*
 * NOTE: we do not use all the following functions, for we dot it
 * with the buffer conversion and read/write routines of the libST library
 * (sfiles.c and convert.c)
 */

/*---------------------------------------------------------------
 allocation de la memoire :
 chan1 et 2 : les pointeurs sur les tableaux
 si le wav est mono, chan2 reste a NULL
 retourne 0 si OK, 1 si non
---------------------------------------------------------------*/
int wavget_mem( WAVHDR wav,
		float ** chan1,
		float ** chan2 )
{

  *chan1 = *chan2 = NULL;
  if ( wav.nChannels == STEREO ) {
    /* stereo : allocation des 2 canaux */
    *chan1 = malloc( wav.lendata/2*sizeof(float) + 2 );
    *chan2 = malloc( wav.lendata/2*sizeof(float) + 2 );
    if ( *chan1 == NULL ) {
      printf("wavget_mem : error alloc chan1.\n");
      return 1;
    }
    if ( *chan2 == NULL ) {
      printf("wavget_mem : error alloc chan2.\n");
      free(*chan1);
      return 1;
    }
  } else {
    /* mono : allocation d'un seul canal ( * 4 car ce sont des float ) */
    *chan1 = malloc( wav.lendata*sizeof(float) + 2 );
    if ( *chan1 == NULL ) {
      printf("wavget_mem : error alloc chan1.\n");
      return 1;
    }
  }
  return 0;
}



/*---------------------------------------------------------------
 Lit les donnees du fichier dont le descripteur est inf.
 wav est le header du fichier a lire, il est cense etre correct,
 i.e. passe par wavhdr_verif().

 Le donn‚es entieres sont converties en floats

 retourne 0 si OK, 1 si non
---------------------------------------------------------------*/
int wavdata_read( FILE * inf,
		  WAVHDR wav,
		  float * chan1,
		  float * chan2 )
{
  unsigned char buf8[1025]; /* pour lire un peu + vite les 8 bits */
  short buf16[1025]; /* pour lire un peu + vite les 16 bits */
  DWORD c1, c2; /* les curseurs sur les tableaux */
  short i; /* compteur */


  /* tests */
  if ( chan1 == NULL ) {
    printf("wavdata_read : chan1 is not allocated.\n");
    return 1;
  }
  if ( (wav.nChannels == STEREO) && (chan2 == NULL) ) {
    printf("wavdata_read : stereo and chan2 is not allocated.\n");
    return 1;
  }

  /* au cas ou, on revient au debut des echantillons du fichier */
  /*
    pb en cas de chunk inconnu, on ne sais plus ou est le
    debut des data
    */
  /* fseek( inf, wav.lenriff-wav.lendata, SEEK_SET ); */

  c1 = c2 = 0L;

  if ( wav.nChannels == MONO ) {
    if ( wav.nBitsPerSamples == 8 ) {
      do {
	fread( buf8, 1024, 1, inf );
	i = 0;
	do {
	  chan1[c1++] = (float)((short)buf8[i++] - 127) * 127;
	} while ( (i<1024) && (c1<wav.lendata) );
      } while ( c1 < wav.lendata );
    }
    if ( wav.nBitsPerSamples == 16 ) {
      do {
	fread( buf16, 1024, 1, inf );
	i = 0;
	do {
	  chan1[c1++] = (float)buf16[i++];
	} while ( (i<1024) && (c1<wav.lendata) );
      } while ( c1 < wav.lendata );
    }
  } /* if nchannels = mono */



  if ( wav.nChannels == STEREO ) {
    if ( wav.nBitsPerSamples == 8 ) {
      do {
	fread( buf8, 1024, 1, inf );
	i = 0;
	do {
	  chan1[c1++] = (float)((short)buf8[i++] - 127) * 127;
	  chan2[c2++] = (float)((short)buf8[i++] - 127) * 127;
	} while ( (i<1024) && ((c1+c2)<wav.lendata) );
      } while ( (c1+c2) < wav.lendata );
    }
    if ( wav.nBitsPerSamples == 16 ) {
      do {
	fread( buf16, 1024, 1, inf );
	i = 0;
	do {
	  chan1[c1++] = (float)buf16[i++];
	  chan2[c2++] = (float)buf16[i++];
	} while ( (i<1024) && ((c1+c2)<wav.lendata) );
      } while ( (c1+c2) < wav.lendata );
    }
  } /* if nchannels = stereo */

  return 0;
}

/*
 * NOTE: according to Microsoft's "Multimedia Programming Interface and Data
 * Specification v 1.0, pp. 83-95, 8 bit coding is [0..128..255] and
 * 16 bit coding is [-32768..0..32767], so we do not use the following function
 */

/*---------------------------------------------------------------
 re-normalise les donnees des canaux.

 les 8 bits entre 0 et 255 avec un milieu a 127
 Les 16 bits finissent entre -16385 et +16384

 Les fonctions precedentes doivent les avoir laissees entre
 -16385 et 16384.

 Cette fonction est appelee juste avant ecriture sur disque
---------------------------------------------------------------*/
void wav_norm( WAVHDR wav, float * chan1, float * chan2 )
{
  DWORD i;

  /* tests */
  if ( chan1 == NULL ) {
    printf("wav_norm : chan1 is not allocated.\n");
    return;
  }
  if ( (wav.nChannels == STEREO) && (chan2 == NULL) ) {
    printf("wav_norm : stereo and chan2 is not allocated.\n");
    return;
  }


  if ( wav.nBitsPerSamples == 8 ) {
    if ( wav.nChannels == MONO ) {
      for ( i=0; i<wav.lendata; i++ ) {
	chan1[i] = chan1[i]/127 + 127 ;
      }
    }
    if ( wav.nChannels == STEREO ) {
      for ( i=0; i<(wav.lendata/2); i++ ) {
	chan1[i] = chan1[i]/127 + 127;
	chan2[i] = chan2[i]/127 + 127;
      }
    }
  }

  if ( wav.nBitsPerSamples == 16 ) {
    /* rien de prevu si ce n'est un test pour verifier que les
       data vont bien de -16385 a +16384 ??? */
  }

}


/*---------------------------------------------------------------
 ecrit les donnees du fichier dont le descripteur est outf.
 wav est le header du fichier a ecrire, il est cense etre correct,
 i.e. passe par wavhdr_verif()... Et doit correspondre a la
 realite.
 les data sont des short, et sont normes dans cette fonction

 retourne 0 si OK, 1 si non
---------------------------------------------------------------*/
int wavdata_write( FILE * outf,
		   WAVHDR wav,
		   float * chan1,
		   float * chan2 )
{
  unsigned char buf8[1025]; /* pour ecrire un peu + vite les 8 bits */
  short buf16[1025]; /* pour ecrire un peu + vite les 16 bits */
  DWORD c1, c2; /* les compteurs sur les tableaux */
  short i; /* compteur */


  /* re-normalise les data 8 bits */
  wav_norm( wav, chan1, chan2 );

  /* au cas ou, on revient au debut des echantillons du fichier */
  /* rem : pas comme read, on est certain de la longueur du header */
  fseek( outf, WAVHDRLEN, SEEK_SET );

  c1 = c2 = 0L;

  if ( wav.nChannels == MONO ) {
    if ( wav.nBitsPerSamples == 8 ) {
      do {
	i = 0;
	do {
	  buf8[i++] = (BYTE)chan1[c1++];
	} while ( (i<1024) && (c1<wav.lendata) );
	fwrite( buf8, i, 1, outf );
      } while ( c1 < wav.lendata );
    }
    if ( wav.nBitsPerSamples == 16 ) {
      do {
	i = 0;
	do {
	  buf16[i++] = (short)chan1[c1++];
	} while ( (i<1024) && (c1<wav.lendata) );
	fwrite( buf16, i, 1, outf );
      } while ( c1 < wav.lendata );
    }
  } /* if nchannels = mono */



  if ( wav.nChannels == STEREO ) {
    if ( wav.nBitsPerSamples == 8 ) {
      do {
	i = 0;
	do {
	  buf8[i++] = (BYTE)chan1[c1++];
	  buf8[i++] = (BYTE)chan2[c2++];
	} while ( (i<1024) && ((c1+c2)<wav.lendata) );
	fwrite( buf8, i, 1, outf );
      } while ( (c1+c2) < wav.lendata );
    }
    if ( wav.nBitsPerSamples == 16 ) {
      do {
	i = 0;
	do {
	  buf16[i++] = (short)chan1[c1++];
	  buf16[i++] = (short)chan2[c2++];
	} while ( (i<1024) && ((c1+c2)<wav.lendata) );
	fwrite( buf16, i, 1, outf );
      } while ( (c1+c2) < wav.lendata );
    }
  } /* if nchannels = stereo */

  return 0;

}


/*---------------------------------------------------------------
 UTILITAIRES
---------------------------------------------------------------*/

/* this is done by the tool peaksf */
/*---------------------------------------------------------------
 Cherche le maximum des data en valeur absolue
---------------------------------------------------------------*/
float wav_max( WAVHDR wav, float * chan1, float * chan2 )
{
  float max;
  DWORD i;

  /* recherche du maximum en valeur absolue */
  max = (float)0.0;
  if ( wav.nChannels == MONO ) {
    for ( i=0; i<wav.lendata; i++ ) {
      if ( fabs((float)chan1[i]) > max ) max = (float)fabs((float)chan1[i]);
    }
  }
  if ( wav.nChannels == STEREO ) {
    for ( i=0; i<(wav.lendata/2); i++ ) {
      if ( fabs((float)chan1[i]) > max ) max = (float)fabs((float)chan1[i]);
      if ( fabs((float)chan2[i]) > max ) max = (float)fabs((float)chan2[i]);
    }
  }

  return max;

}





/*---------------------------------------------------------------

 Amplifie les donnees des deux canaux.
 travaille uniquement sur des short. (cf debut de ce fichier)

---------------------------------------------------------------*/
void wav_ampli( WAVHDR wav, float * chan1, float * chan2 )
{
  DWORD i;
  float coef, max;

  /* tests */
  if ( chan1 == NULL ) {
    printf("wav_ampli : chan1 is not allocated.\n");
    return;
  }
  if ( (wav.nChannels == STEREO) && (chan2 == NULL) ) {
    printf("wav_ampli : stereo and chan2 is not allocated.\n");
    return;
  }

  max = wav_max( wav, chan1, chan2 );

  /* modification des data */
  coef = (float)16383.0/max;
  coef -= coef*(float)0.04;

  if ( wav.nChannels == MONO ) {
    for ( i=0; i<wav.lendata; i++ ) {
      chan1[i] = chan1[i]*coef;
    }
  }
  if ( wav.nChannels == STEREO ) {
    for ( i=0; i<(wav.lendata/2); i++ ) {
      chan1[i] = chan1[i]*coef;
      chan2[i] = chan2[i]*coef;
    }
  }

}

/*---------------------------------------------------------------------
 add wrappers that integrate the above routines in STtools library 
-----------------------------------------------------------------------

  There are the following adaptions to STtools library:
  - reading is with file descriptor instead of file pointer
  - using custom STread (pipe fault tolerant) and STreadLittleEndian (transparent byte swapping)

  There are 3 returns:
  1. read error -> return is negative
  2. success -> return should be the size of the header struct read (44 byte)
  3. no WAV file -> return is zero
*/

int STreadWAVheader(int fd, WAVHDR *hd) {

  int ret;
  int count = 0; /* bytes read */
 
  /* seek to begin of file */
  lseek(fd, 0, SEEK_SET); 

  /* try to identify "RIFF" chunk */ 
  if ((ret = STread(fd, (char *)hd->RIFF, sizeof(hd->RIFF))) != sizeof(hd->RIFF) ) {
    fprintf(stderr, "hd (STreadHDheader): cannot read WAV RIFF chunk id!\n");
    return -1;
  }
  count += ret;
 
  if (strncmp(hd->RIFF, "RIFF", 4) != 0) {
    return 0; /* no error case, 0 means it was simply not a WAV header */
  }
 
  if ((ret = STreadLittleEndianDWORD(fd, &hd->lenriff)) != sizeof(hd->lenriff)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV RIFF chunk length!\n");
    return -1;
  }
  count += ret;
 
  /* try to identify "fmt " chunk (NOTE: read preceding "WAVE" keyword on the fly) */ 
  if ((ret = STread(fd, (char *)hd->WAVfmt, sizeof(hd->WAVfmt))) != sizeof(hd->WAVfmt) ) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV format chunk id!\n");
    return -1;
  }
  count += ret;

  if (strncmp(hd->WAVfmt, "WAVEfmt ", 8) != 0) {
    fprintf(stderr, "wav (STreadWAVheader): expected 'WAVEfmt ' chunk id, but found %s!\n", hd->WAVfmt);
    return -1;
  }

  if ((ret = STreadLittleEndianDWORD(fd, &hd->lenhead)) != sizeof(hd->lenhead)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV fmt chunk length!\n");
    return -1;
  }
  count += ret;

  /* now, finally, the data we are REALLY interested in: */

  if ((ret = STreadLittleEndianWORD(fd, &hd->wFormatTag)) != sizeof(hd->wFormatTag)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV format tag!\n");
    return -1;
  }
  count += ret;

  if ((ret = STreadLittleEndianWORD(fd, &hd->nChannels)) != sizeof(hd->nChannels)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV number of channels!\n");
    return -1;
  }
  count += ret;

  if ((ret = STreadLittleEndianDWORD(fd, &hd->nSamplesPerSec)) != sizeof(hd->nSamplesPerSec)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV sampling rate!\n");
    return -1;
  }
  count += ret;

  if ((ret = STreadLittleEndianDWORD(fd, &hd->nAvgBytesPerSec)) != sizeof(hd->nAvgBytesPerSec)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV average byte rate!\n");
    return -1;
  }
  count += ret;

  if ((ret = STreadLittleEndianWORD(fd, &hd->nBlockAlign)) != sizeof(hd->nBlockAlign)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV block alignment!\n");
    return -1;
  }
  count += ret;

  if ((ret = STreadLittleEndianWORD(fd, &hd->nBitsPerSamples)) != sizeof(hd->nBitsPerSamples)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV sample width!\n");
    return -1;
  }
  count += ret;

  /* try to read "data" chunk */ 
  if ((ret = STread(fd, (char *)hd->data, sizeof(hd->data))) != sizeof(hd->data) ) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV data chunk id!\n");
    return -1;
  }
  count += ret;
  /* check for "data" id will be made in wavhdr_read below */

  if ((ret = STreadLittleEndianDWORD(fd, &hd->lendata)) != sizeof(hd->lendata)) {
    fprintf(stderr, "wav (STreadWAVheader): cannot read WAV data chunk length!\n");
    return -1;
  }
  count += ret;

  /* ignore possible following chunks: */
  wavhdr_read(fd, hd);

  return count;
}

int STwriteWAVheader (int fd, const SFHEADER *hd, int samplesByteCount) {

  WAVHDR wav;
  int ret;
  int count = 0; /* bytes written */

  /* make WAV header */

  WORD nChan = (WORD) sfchans(hd);
  DWORD nBytes = (DWORD) samplesByteCount;
  DWORD nSamplesPerSec = (DWORD) sfsrate(hd);
  WORD nBitsPerSamples = (WORD) sfclass(hd) * 8;

  /* yes, these data are sufficient, the rest can be derived */
  wavhdr ( &wav,
	   nChan,
	   nBytes,
	   nSamplesPerSec,
	   nBitsPerSamples 
	   );
  /* rewind to begin of file, to be sure */
  lseek(fd, 0, SEEK_SET); 
 
  /* try to write "RIFF" chunk */ 
  if ((ret = STwrite(fd, (char *)wav.RIFF, sizeof(wav.RIFF))) != sizeof(wav.RIFF) ) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV RIFF chunk id!\n");
    return -1;
  }
  count += ret;
 
  if ((ret = STwriteLittleEndianDWORD(fd, wav.lenriff)) != sizeof(wav.lenriff)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV RIFF chunk length!\n");
    return -1;
  }
  count += ret;
 
  /* try to write "fmt " chunk (NOTE: write preceding "WAVE" keyword on the fly) */ 
  if ((ret = STwrite(fd, (char *)wav.WAVfmt, sizeof(wav.WAVfmt))) != sizeof(wav.WAVfmt) ) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV format chunk id!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianDWORD(fd, wav.lenhead)) != sizeof(wav.lenhead)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV fmt chunk length!\n");
    return -1;
  }
  count += ret;

  /* now, finally, the data we are REALLY interested in: */

  if ((ret = STwriteLittleEndianWORD(fd, wav.wFormatTag)) != sizeof(wav.wFormatTag)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV format tag!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianWORD(fd, wav.nChannels)) != sizeof(wav.nChannels)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV number of channels!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianDWORD(fd, wav.nSamplesPerSec)) != sizeof(wav.nSamplesPerSec)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV sampling rate!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianDWORD(fd, wav.nAvgBytesPerSec)) != sizeof(wav.nAvgBytesPerSec)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV average byte rate!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianWORD(fd, wav.nBlockAlign)) != sizeof(wav.nBlockAlign)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV block alignment!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianWORD(fd, wav.nBitsPerSamples)) != sizeof(wav.nBitsPerSamples)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV sample width!\n");
    return -1;
  }
  count += ret;

  /* try to write "data" chunk */ 
  if ((ret = STwrite(fd, (char *)wav.data, sizeof(wav.data))) != sizeof(wav.data) ) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV data chunk id!\n");
    return -1;
  }
  count += ret;

  if ((ret = STwriteLittleEndianDWORD(fd, wav.lendata)) != sizeof(wav.lendata)) {
    fprintf(stderr, "wav (STwriteWAVheader): cannot write WAV data chunk length!\n");
    return -1;
  }
  count += ret;

  return count;
}

/* this function is called after the last sample of the sound file is written */
int STupdateWAVsampleCount(int fd, SFHEADER *hd) {
  /* NOTE: pre: caller seeked to end of file! */
  int pos = lseek(fd, 0, SEEK_CUR); /* i.e. get EOF position */

  /* just compute the difference between sample offset and EOF */
  /* NOTE: do NOT divide by sample width and channel number, for lendata is size in BYTES,
     contrary to information you might find in different documentations on the WAV format */
  int nBytes = (pos - WAVHDRLEN);
  /* update field in header */
  return STwriteWAVheader(fd, hd, nBytes);
}






