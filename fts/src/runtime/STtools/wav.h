/* author Olivier Trystram, adapted to STtools library by Peter Hoffmann, IRCAM, 04/98 */

/*---------------------------------------------------------------

    Header commun aux progs WAV.

---------------------------------------------------------------*/

#ifndef __WAV_H
#define __WAV_H

#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------
	to provide prototypes for "fabs" <math.h> and "SFHEADER" <ST/sfheader.h> 
--------------------------------------------------------------*/

#include <math.h>

/*--------------------------------------------------------------
	some of Microsoft's types for WAV header data 
--------------------------------------------------------------*/

typedef unsigned char  BYTE;
typedef unsigned int   UINT;
typedef signed long    LONG;
typedef unsigned short WORD;
typedef unsigned long  DWORD;


/*---------------------------------------------------------------
    quelques definitions pour la manipulation des big et
    littles endians.
---------------------------------------------------------------

#define FALSE          (0 == 1)
#define TRUE           (1 == 1)
typedef int            BOOL;

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

*/

/*---------------------------------------------------------------
    Quelques definitions pour le decodage des fichiers WAV
---------------------------------------------------------------*/

#define MONO        1
#define STEREO      2
#define WAVHDRLEN  44L


/*
    WAV structure
    taille = 44 octets
    rem : 44 = 36 + 8, et longueur de RIFF + lenriff = 8, cryptique, ca ?
*/
typedef struct waveheader {
  char RIFF [4];         /* contient "RIFF" */
  DWORD lenriff;         /* = taille fichier - 8 = lendata + 36 */

  char WAVfmt [8];       /* contient "WAVEfmt " */
  DWORD lenhead;         /* 0x10L, longueur de ce chunk  */
  WORD wFormatTag;       /* 1 = type de donnees PCM */
  WORD nChannels;        /* 1 = mono, 2 = stereo */
  DWORD nSamplesPerSec;  /* taux d'echantillonnage 11025, 22050, 44100 */
  DWORD nAvgBytesPerSec; /* = nSamplesPerSec * nBlockAlign */
  WORD nBlockAlign;      /* = nBitsPerSamples/8 * nChannels */
  WORD nBitsPerSamples;  /* 4, 8, ou 16 bits */

  char data[4];          /* contient "data" */
  DWORD lendata ;        /* nombre d'echantillons */
} WAVHDR;




/*---------------------------------------------------------------
    les declarations de fonctions
---------------------------------------------------------------*/

int wavhdr_read( int fd,  WAVHDR * wav );

/*
void wavhdr_write( FILE *outf, struct WAVHDR * wav );
*/

void wavhdr_print(  WAVHDR wav );

int wavhdr_verif(  WAVHDR wav );

void wavhdr(  WAVHDR * wav,
             WORD nChan,
             DWORD nSamples,
             DWORD Taux_Echant,
             WORD BitsPerSample  );

int wavget_mem(  WAVHDR wav,
                float ** chan1,
                float ** chan2 );

int wavdata_read( FILE * inf,
                   WAVHDR wav,
                  float * chan1,
                  float * chan2 );

int wavdata_write( FILE * outf,
                    WAVHDR wav,
                   float * chan1,
                   float * chan2 );


/* utilitaires */

float wav_max(  WAVHDR wav,
               float * chan1,
               float * chan2 );

void wav_ampli(  WAVHDR wav,
                float * chan1,
                float * chan2 );

/*---------------------------------------------------------------
    STtools library wrappers for some of the above functions
---------------------------------------------------------------*/

/* reads a WAV header with transparent byte (re)ordering */
extern int STreadWAVheader(int fd, WAVHDR *hd);

/*
	NOTE: the prototypes for all wrappers using SFHEADER have to
	 be declared in a separate file "wavP.h" in libST, for sfheader.h
	 cannot be included here because this would entail a mutual
	 reference of header files "sfheader.h" and "wav.h". Unfortunately,
	 the C compiler used is still a ONE PASS COMPILER. Poor UNIX world...
	 The same holds for the additional header file "aiffP.h" in libST.
*/

#endif /* #define __WAV_H */



























