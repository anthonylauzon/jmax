#ifndef _ST_CONVERT_H_
#define _ST_CONVERT_H_

#include "sfheader.h"
#include "aiff.h"

extern void STswap16(void *buff, int n);
/* 
 * Swaps bytes in buff containing n 16bit words. 
 */

extern void STswap24(void *buff, int n);
/* 
 * Swaps bytes in buff containing n 24bit words. 
 */

extern void STswap32(void *buff, int n);
/* 
 * Swaps bytes in buff containing n 32bit words. 
 */


#define STShortMax  32768
#define ST8bitMax   128
#define ST16bitMax  32768
#define ST24bitMax  8388608
#define ST32bitMaxInDouble  2147483648.0

extern double STmaxValue(SFHEADER *hd);
/*
 * Returns the maximum value of the dataformat.
 */


/* ========== float to ... ========== */

#define STfloatToShort(f)  (short)STfloatToInteger(f, STShortMax)
#define STfloatTo8bit(f)   (ST8bit)STfloatToInteger(f, ST8bitMax)
#define STfloatTo16bit(f)  (ST16bit)STfloatToInteger(f, ST16bitMax)
#define STfloatTo32bit(f)  (ST32bit)STfloatToInteger(f, ST32bitMaxInDouble)

extern ST32bit STfloatToInteger(float f, double imax);

extern void STfloatTo24bit(float f, ST24bit *t);


/* ========== short/16bit to ... ========== */

#define STshortToFloat(n) ((float)(n)/(float)ST16bitMax)
#define ST16bitToFloat(n) ((float)(n)/(float)ST16bitMax)

#define STshortTo8bit(n)  ((ST8bit)((n)/256))
#define ST16bitTo8bit(n)  ((ST8bit)((n)/256))

#define STshortTo32bit(n) (65536*(ST32bit)(n))
#define ST16bitTo32bit(n) (65536*(ST32bit)(n))

extern void STshortTo24bit(short s, ST24bit *t);
#define ST16bitTo24bit(n,t) STshortTo24bit(n,t)


/* ========== 8bit to ... ========== */

#define ST8bitToShort(n) ((short)(n)*256)
#define ST8bitTo16bit(n) ((short)(n)*256)
#define ST8bitTo32bit(n) ((ST32bit)(n)*16777216L)
/*
.................extern void ST8bitTo24bit(ST8bit n, ST24bit *t);
*/

extern float ST8bitToFloat(ST8bit n);


/* ========== 24bit to ... ========== */

extern float ST24bitToFloat(ST24bit n);
/*
....................extern ST8bit ST24bitTo8bit(ST24bit n);
*/
extern short ST24bitToShort(ST24bit n);
/*
....................extern ST32bit ST24bitTo32bit(ST24bit n);
*/

/* ========== 32bit to ... ========== */

#define ST32bitToFloat(n) ((float)(n)/ST32bitMaxInDouble)
#define ST32bitTo8bit(n)  ((ST8bit)((n)/16777216L))
#define ST32bitToShort(n) ((n)/65536)
/*
.................extern void ST32bitTo24bit(ST32bit n, ST24bit *t);
*/

/* ========== 8 bit signed to 8 bit unsigned and vice versa ===== */
extern ST8bit STUnsignedToSigned8bit(ST8bit unsigned8bit);
extern ST8bit STSignedToUnsigned8bit(ST8bit signed8bit);


/* 
 * ============================ 
 * ========== BUFFER ========== 
 * ============================ 
*/

/* ========== float buffer to ... ========== */

extern void STfloatToShortBuffer(float *fbuff, short *sbuff, int n);
extern void STfloatTo8bitBuffer(float *fbuff, ST8bit *buff, int n);
extern void STfloatTo16bitBuffer(float *fbuff, ST16bit *buff, int n);
extern void STfloatTo24bitBuffer(float *fbuff, ST24bit *buff, int n);
extern void STfloatTo32bitBuffer(float *fbuff, ST32bit *buff, int n);


/* ========== short buffer to ... ========== */

extern void STshortTo8bitBuffer(short *sbuff, ST8bit *buff, int n);
extern void STshortTo24bitBuffer(short *sbuff, ST24bit *buff, int n);
extern void STshortTo32bitBuffer(short *sbuff, ST32bit *buff, int n);
extern void STshortToFloatBuffer(short *sbuff, float *fbuff, int n);


/* ========== 8bit buffer to ... ========== */

extern void ST8bitToShortBuffer(ST8bit *buff, short *sbuff, int n);
extern void ST8bitToFloatBuffer(ST8bit *buff, float *fbuff, int n);


/* ========== 24bit buffer to ... ========== */

extern void ST24bitToFloatBuffer(ST24bit *buff, float *fbuff, int n);
extern void ST24bitToShortBuffer(ST24bit *buff, short *sbuff, int n);


/* ========== 32bit buffer to ... ========== */

extern void ST32bitToShortBuffer(ST32bit *buff, short *sbuff, int n);
extern void ST32bitToFloatBuffer(ST32bit *buff, float *fbuff, int n);

/* ========== 8 bit signed to 8 bit unsigned and vice versa ======= */
void STSignedToUnsigned8bitBuffer(ST8bit *buffSigned, ST8bit *buffUnsigned, int n);
void STUnsignedToSigned8bitBuffer(ST8bit *buffUnsigned, ST8bit *buffSigned, int n);

/* ================================================================ */

extern void STconvertSampleBuffer(SFHEADER *from_hd, SFHEADER *to_hd, void *buff, int n);
/* 
 * Converts n channel independent samples according to specified formats. 
 */

extern void STconvertToFloatBuffer(SFHEADER *hd, void *buff, int n);
/* 
 * Converts n channel independent samples to floats. 
 */

extern void STconvertToShortBuffer(SFHEADER *hd, void *buff, int n);
/* 
 * Converts n channel independent samples to shorts. 
 */

extern void STconvertFromFloatBuffer(SFHEADER *hd, void *buff, int n);
/* 
 * Converts n channel independent samples from floats. 
 */

extern void STconvertFromShortBuffer(SFHEADER *hd, void *buff, int n);
/* 
 * Converts n channel independent samples from shorts. 
 */


extern void STcnvForeignHeader(SFHEADER *h);
/* 
 * Converts foreign IRCAM soundfile header to custom. 
 */

extern void STcnvCustomHeader(SFHEADER *h);
/* 
 * Converts custom IRCAM soundfile header to foreign. 
 */

extern int STconvertTimeSpec(const char *spec, SFHEADER *h);
/*
 * Converts a time specification string to samples:
 * integer or postfix 'S' denotes samples,
 * floats or postfix 's' denotes seconds.
 */

extern double STconvExtended(STextended e);
/*
 * Converts IEEE extended floating point to local double.
 */

extern void STconvDoubleToExtended(double d, STextended *e);
/*
 * Converts local double to IEEE extended floating point.
 */

 
#endif /* _ST_CONVERT_H_ */
