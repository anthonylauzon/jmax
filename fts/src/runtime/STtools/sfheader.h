#ifndef _ST_SFHEADER_H_
#define _ST_SFHEADER_H_

#define STversion {fprintf(stderr, "STtools version "_ST_VERSION_" "__DATE__" "__TIME__" by Rolf Woehrmann.\n");exit(0);}

#include "aiff.h"
#include "wav.h"

/* 
 * extended to include Microsoft's proprietary RIFF soundfile format (*.WAV)
 * with thanks to Olivier Trystram who hacked this format better than I did
 * Peter Hoffmann, IRCAM, 04/98
 */

/*
 * DEFINITION OF CONSTANTS
 */

#define SIZEOF_HEADER 1024	
#define SF_MAGIC1 0144
#define SF_MAGIC2 0243

#define SF_UNKNOWN 0
#define SF_VAX 1
#define SF_SUN 2
#define SF_MIPS 3
#define SF_NEXT 4

/*
 * Not part of the IRCAM soundfile standard,
 * just used for internal indication that the
 * current opened soundfile is an AIFF/AIFC/WAV type.
 */
#define SF_AIFF 99
#define SF_AIFC 98
#define SF_SONIC 97
#define SF_WAV 96

#if NeXT
#define SF_MACHINE SF_NEXT
#elif sgi
#define SF_MACHINE SF_NEXT
#elif mips
#define SF_MACHINE SF_MIPS
#elif vax
#define SF_MACHINE SF_VAX
#elif __sun
#define SF_MACHINE SF_SUN
#endif

#define STlittleEndianCPU (SF_MACHINE == SF_MIPS) /* || SF_MACHINE == SF_VAX) */
#define STbigEndianCPU (SF_MACHINE == SF_NEXT || SF_MACHINE == SF_SUN)

#define SF_SHORT sizeof(short)
#define SF_FLOAT sizeof(float)
#define SF_8bit sizeof(ST8bit)
#define SF_16bit sizeof(ST16bit)
#define SF_24bit sizeof(ST24bit)
#define SF_32bit sizeof(ST32bit)

#define SF_BUFSIZE (16 * 1024)
#define SF_MAXCHAN 16
#define SF_CBUFSIZE (SF_MAXCHAN * SF_BUFSIZE)

#define PRI '\001' /* Fichier contenant de PRI */
#define PFT '\002' /* Fichier contenant des PFT */
#define PFT_AMPFRE '\003' /* Les PFT sont code en amplitude-frequence */
#define PFT_AMPDB '\004' /* amplitude-frequence mais en decibel normalisees */


/*
 * DEFINITION OF SFCODE AND RELATED DATA STRUCTS
 *
 * Tow routines in sfcodes.c are used to insert additionnal information into a header
 * or to retrieve such information.
 */

#define SF_END 0 /* Meaning no more information */
#define SF_MAXAMP 1 /* Meaning maxamp follows */
#define SF_COMMENT 2 /* code for "comment line" */

typedef struct sfcode {
  short	code;
  short	bsize;
} SFCODE;

typedef struct sfmaxamp {
  float	value[SF_MAXCHAN];
  long	samploc[SF_MAXCHAN];
  long	timetag;
} SFMAXAMP;

extern const SFCODE STendcode;

/*
 * DEFINITION OF SFHEADER FORMAT
 */

typedef union sfheader {
  struct {
    unsigned char	 sf_magic1;
    unsigned char	 sf_magic2;
    unsigned char	 sf_machine;
    unsigned char	 sf_param;
    float	 sf_srate;
    int	 sf_chans;
    int	 sf_packmode;
    SFCODE sf_codes;
  } sfinfo;
  char	filler[SIZEOF_HEADER];
  struct {
    char skip[20];
    STaifcCommonChunk common;
    STsoundDataChunk soundData;
    unsigned long soundDataPosition;
    unsigned long soundDataRead;
  } aiff;
  /* WAV header is defined in WAV.H, but mutual inclusion of .h files is not possible in C */
  WAVHDR wav;
} SFHEADER;


/*
 * DEFINITION OF MACROS TO GET HEADER INFO
 * x is a pointer to SFHEADER
 */

#define sfchans(x) (x)->sfinfo.sf_chans
#define sfmagic1(x) (x)->sfinfo.sf_magic1
#define sfmagic2(x) (x)->sfinfo.sf_magic2
#define sfmachine(x) (x)->sfinfo.sf_machine
#define sfparam(x) (x)->sfinfo.sf_param
#define sfsrate(x) (x)->sfinfo.sf_srate
#define sfclass(x) (x)->sfinfo.sf_packmode
#define sfcodes(x) (x)->sfinfo.sf_codes


/*
 * MACROS FOR TESTING SOUNDFILE FORMATS
 */

#define STisIRCAM(x) (((x)->sfinfo.sf_magic1 == SF_MAGIC1) && ((x)->sfinfo.sf_magic2 == SF_MAGIC2) \
		      && (x)->sfinfo.sf_machine >= SF_VAX && (x)->sfinfo.sf_machine <= SF_NEXT) /* i.e. no PC/MAC */

#define STisLocalIRCAM(x) (((x)->sfinfo.sf_magic1 == SF_MAGIC1) && ((x)->sfinfo.sf_magic2 == SF_MAGIC2) \
			   && (x)->sfinfo.sf_machine == SF_MACHINE) /* i.e. same machine */

#define STendian(x) ((x) == SF_NEXT || (x) == SF_SUN) /* i.e. big endian (see above) */

#define STisForeignIRCAM(x) (((x)->sfinfo.sf_magic1 == SF_MAGIC1) && ((x)->sfinfo.sf_magic2 == SF_MAGIC2) \
			     && (x)->sfinfo.sf_machine >= SF_VAX && (x)->sfinfo.sf_machine <= SF_NEXT && \
			     STendian((x)->sfinfo.sf_machine) != STendian(SF_MACHINE)) /* i.e. a change in byte order btween UNIX's*/

#define STisAIFF(x) ((x)->sfinfo.sf_machine == SF_AIFF)
#define STisAIFC(x) ((x)->sfinfo.sf_machine == SF_AIFC)
#define STisSONIC(x) ((x)->sfinfo.sf_machine == SF_SONIC)
#define STisAIFX(x) (STisAIFF(x) || STisAIFC(x) || STisSONIC(x))
#define STisWAV(x) ((x)->sfinfo.sf_machine == SF_WAV)

#define STis8bit(x) (sfclass(x) == SF_8bit)
#define STis16bit(x) (sfclass(x) == SF_16bit)
#define STis24bit(x) (sfclass(x) == SF_24bit)
#define STis32bit(x) (sfclass(x) == SF_32bit && STisAIFF(x))
#define STisShort(x) (sfclass(x) == SF_SHORT)
#define STisFloat(x) (sfclass(x) == SF_FLOAT && STisIRCAM(x))

		 
/*
 * DEFINITION OF MACRO TO GET MAXAMP INFO
 *
 * sfm is ptr to SFMAXAMP
 * sfst is the address of a stat struct
 */

#define sfmaxamp(mptr,chan) (mptr)->value[chan]
#define sfmaxamploc(mptr,chan) (mptr)->samploc[chan]
#define sfmaxamptime(x) (x)->timetag


#endif /* _ST_SFHEADER_H_ */
