/*------------audp_format.h-------------------------------

Description:
 Declarations for the masks used to describe the audio samples

---------------------------------------------------------*/


/*************************/
/* Data Formats Masks    */
/*************************/

/* endianess */

#define audp_LE 0x00      /* Little Endian */   
#define audp_BE 0x80      /* Big Endian */      

#define SWAP_2(S,C) \
( C = ((unsigned char *)&S)[0], \
  ((unsigned char *)&S)[0] = ((unsigned char *)&S)[1], \
  ((unsigned char *)&S)[1] = C )

#define SWAP_4(S,C) \
( C = ((unsigned char *)&S)[0], \
  ((unsigned char *)&S)[0] = ((unsigned char *)&S)[3], \
  ((unsigned char *)&S)[3] = C, \
  C = ((unsigned char *)&S)[1], \
  ((unsigned char *)&S)[1] = ((unsigned char *)&S)[2], \
  ((unsigned char *)&S)[2] = C )

/* sample_format */

#define audp_undef 0x00   /* undefined format */
#define	audp_int16 0x01   /* 16 bits signed integer */ 
#define	audp_uint16 0x02  /* 16 bits unsigned integer */
#define	audp_int32 0x03   /* 32 bits signed integer */
#define	audp_uint32 0x04  /* 32 bits unsigned integer */
#define	audp_float32 0x05 /* 32 bits float */

/* unsigned char format = endianess | sample_format */


/*************************/
/* Sample Rate Masks     */
/*************************/


#define	audp_sr22 0x00 /* Sampling rate = 22050Hz */
#define	audp_sr32 0x40 /* Sampling rate = 32000Hz */
#define	audp_sr44 0xa0 /* Sampling rate = 44100Hz */
#define	audp_sr48 0xc0 /* Sampling rate = 48000Hz */


