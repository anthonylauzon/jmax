#include <unistd.h>
#include <math.h>
#include <strings.h>
#include <stdlib.h>

#include "convert.h"
#include "sfcodes.h"

/*
 edited to support read and write of Microsoft's WAV sound file format:
 WAV 8 bit samples are stored as [0..128..255] unsigned 8 bit.
 => in case of 8 bit in- or output, data have to be shifted to
 [-128..0..127] signed 8 bit representation before being 
 converted to floating point numbers, and vice versa.

 Edits are in STconvertSampleBuffer; STUnsignedToSigned8bitBuffer and
 STSignedToUnsigned8bitBuffer have been added.
 Peter Hoffmann, IRCAM, 04/98
*/


double STmaxValue(SFHEADER *hd)
{
  if (STisFloat(hd))
    return 1.0;

  switch (sfclass(hd)) {
  case SF_8bit:
    return ST8bitMax;
  case SF_16bit:
    return ST16bitMax;
  case SF_24bit:
    return ST24bitMax;
  case SF_32bit:
    return ST32bitMaxInDouble;
  }

  return 0;
}


void STswap16(void *buff, int n)
{
  char t, *c = buff;

  while (n-- > 0) {
    t = c[0];
    c[0] = c[1];
    c[1] = t;

    c += 2;
  }
}


void STswap24(void *buff, int n)
{
  char t, *c = buff;

  while (n-- > 0) {
    t = c[0];
    c[0] = c[2];
    c[2] = t;

    c += 3;
  }
}


void STswap32(void *buff, int n)
{
  char t, *c = buff;

  while (n-- > 0) {
    t = c[0];
    c[0] = c[3];
    c[3] = t;

    t = c[1];
    c[1] = c[2];
    c[2] = t;

    c += 4;
  }
}


/*
short ST8bitToShort(ST8bit n)
{
 short s;
 char *c = (char *)&s;


 if (STbigEndianCPU) {
 c[0] = n;
 c[1] = n & 0x80 ? 0xFF : 0x00;
 } else {
 c[1] = n;
 c[0] = n & 0x80 ? 0xFF : 0x00;
 }
 
 return s;
}
*/


float ST8bitToFloat(ST8bit n)
{
  short s;
  char *c = (char *)&s;


  if (STbigEndianCPU) {
    c[0] = n;
    c[1] = n & 0x80 ? 0xFF : 0x00;
  } else {
    c[1] = n;
    c[0] = n & 0x80 ? 0xFF : 0x00;
  }
 
  return (float)s/(float)ST16bitMax;
}


void STshortTo24bit(short s, ST24bit *t)
{
  char 
    *sc = (char *)&s,
    *tc = (char *)t;


  if (STbigEndianCPU) {
    tc[0] = sc[0];
    tc[1] = sc[1];
    tc[2] = sc[0] & 0x80 ? 0xFF : 0x00;
  } else {
    tc[2] = sc[1];
    tc[1] = sc[0];
    tc[0] = sc[1] & 0x80 ? 0xFF : 0x00;
  }
}


ST32bit STfloatToInteger(float f, double imax)
{
  double tmp = rint(f * (imax - 1.0));

  if (tmp <= -imax)
    return (ST32bit)-imax;

  if (tmp >= imax - 1)
    return (ST32bit)(imax - 1);

  return (ST32bit)tmp;
}


void STfloatTo24bit(float f, ST24bit *t)
{
  ST32bit
    s = STfloatTo32bit(f);

  char 
    *sc = (char *)&s,
    *tc = (char *)t;


  if (STbigEndianCPU) {
    tc[0] = sc[0];
    tc[1] = sc[1];
    tc[2] = sc[2];
  } else {
    tc[2] = sc[3];
    tc[1] = sc[2];
    tc[0] = sc[1];
  }
}


short ST24bitToShort(ST24bit n)
{
  short s;
  char *c = (char *)&s;
 

  if (STbigEndianCPU) {
    c[0] = n.c0;
    c[1] = n.c1;
  } else {
    c[1] = n.c2;
    c[0] = n.c1;
  }

  return s;
}


float ST24bitToFloat(ST24bit n)
{
  ST32bit l;
  char *c = (char *)&l;
 

  if (STbigEndianCPU) {
    c[0] = n.c0 & 0x80 ? 0xFF : 0x00;
    c[1] = n.c0;
    c[2] = n.c1;
    c[3] = n.c2;
  } else {
    c[3] = n.c2 & 0x80 ? 0xFF : 0x00;
    c[2] = n.c2;
    c[1] = n.c1;
    c[0] = n.c0;
  }

  return (float)l / (float)ST24bitMax;
}

/* conversion to and from Microsoft's 8 bit unsigned sample encoding [0..128..255] */

ST8bit STSignedToUnsigned8bit(ST8bit signed8bit)
{ /* shift origin from 0 to 128 (=ST8bitMax) */
  return signed8bit + ST8bitMax; /* i.e. [-128..0] to [0..128], and [0..127] to [128..255] */
} 

ST8bit STUnsignedToSigned8bit(ST8bit unsigned8bit)
{ /* shift origin from 128 (=ST8bitMax) to 0 */
  return unsigned8bit - ST8bitMax; /* i.e. [0..128] to [-128..0], and [128..255] to [0..127] */
} 

/* 
 * ------------------------------------------------------------------------------------
 *  conversion of whole buffers, using above defined sample-wise conversion routines 
 * ------------------------------------------------------------------------------------
 */

void STshortTo8bitBuffer(short *sbuff, ST8bit *ibuff, int n)
{
  while (n-- > 0)
    *ibuff++ = STshortTo8bit(*sbuff++);
}


void STshortTo24bitBuffer(short *sbuff, ST24bit *ibuff, int n)
{
  ibuff += n - 1;
  sbuff += n - 1;

  while (n-- > 0)
    STshortTo24bit(*sbuff--, ibuff--);
}


void STshortTo32bitBuffer(short *sbuff, ST32bit *ibuff, int n)
{
  ibuff += n - 1;
  sbuff += n - 1;

  while (n-- > 0) 
    *ibuff-- = STshortTo32bit(*sbuff--);
}


void STfloatToShortBuffer(float *fbuff, short *sbuff, int n)
{
  while (n-- > 0)
    *sbuff++ = STfloatToShort(*fbuff++);
}


void STfloatTo8bitBuffer(float *fbuff, ST8bit *ibuff, int n)
{
  while (n-- > 0)
    *ibuff++ = STfloatTo8bit(*fbuff++);
}


void STfloatTo16bitBuffer(float *fbuff, ST16bit *ibuff, int n)
{
  while (n-- > 0)
    *ibuff++ = STfloatTo16bit(*fbuff++);
}


void STfloatTo24bitBuffer(float *fbuff, ST24bit *ibuff, int n)
{
  while (n-- > 0)
    STfloatTo24bit(*fbuff++, ibuff++);
}


void STfloatTo32bitBuffer(float *fbuff, ST32bit *ibuff, int n)
{
  while (n-- > 0)
    *ibuff++ = STfloatTo32bit(*fbuff++);
}


void ST32bitToShortBuffer(ST32bit *buff, short *sbuff, int n)
{
  while (n-- > 0)
    *sbuff++ = ST32bitToShort(*buff++);
} 


void ST32bitToFloatBuffer(ST32bit *buff, float *fbuff, int n)
{
  while (n-- > 0)
    *fbuff++ = ST32bitToFloat(*buff++);
}


void ST8bitToShortBuffer(ST8bit *buff, short *sbuff, int n)
{
  buff += n - 1;
  sbuff += n - 1;

  while (n-- > 0)
    *sbuff-- = ST8bitToShort(*buff--);
}


void ST8bitToFloatBuffer(ST8bit *buff, float *fbuff, int n)
{
  buff += n - 1;
  fbuff += n - 1;

  while (n-- > 0) 
    *fbuff-- = ST8bitToFloat(*buff--);
}


void STshortToFloatBuffer(short *sbuff, float *fbuff, int n)
{
  fbuff += n - 1;
  sbuff += n - 1;

  while (n-- > 0)
    *fbuff-- = STshortToFloat(*sbuff--);
}


void ST24bitToFloatBuffer(ST24bit *buff, float *fbuff, int n)
{
  fbuff += n - 1;
  buff += n - 1;

  while (n-- > 0)
    *fbuff-- = ST24bitToFloat(*buff--);
}

void ST24bitToShortBuffer(ST24bit *buff, short *sbuff, int n)
{
  while (n-- > 0)
    *sbuff++ = ST24bitToShort(*buff++);
}

/* conversion to and from Microsoft's 8 bit unsigned sample encoding [0..128..255] */

void STUnsignedToSigned8bitBuffer(ST8bit *buffUnsigned, ST8bit *buffSigned, int n)
{
  while (n-- > 0)
    *buffSigned++ = STUnsignedToSigned8bit(*buffUnsigned++);
}

void STSignedToUnsigned8bitBuffer(ST8bit *buffSigned, ST8bit *buffUnsigned, int n)
{
  while (n-- > 0)
    *buffUnsigned++ = STSignedToUnsigned8bit(*buffSigned++);
}

/* 
 * ---------------------------------------------------------------------------------------------
 *  "main" function called from fromsf/tosf etc. using above defined buffer conversion routines 
 * ---------------------------------------------------------------------------------------------
 */

void STconvertSampleBuffer(SFHEADER *from_hd, SFHEADER *to_hd, void *buff, int i)
{
  if (STisShort(from_hd) && STisFloat(to_hd)) {
    STshortToFloatBuffer((short *)buff, (float *)buff, i);

  } else if (STisShort(from_hd) && !STisIRCAM(to_hd)) {
    switch (sfclass(to_hd)) {
    case SF_8bit:
      STshortTo8bitBuffer((short *)buff, (ST8bit *)buff, i);
      /* NOTE: WAV format's 8 bit representation is unsigned! */
      if (STisWAV(to_hd)) {
	STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
      }
      break;
    case SF_24bit:
      STshortTo24bitBuffer((short *)buff, (ST24bit *)buff, i);
      break;
    case SF_32bit:
      STshortTo32bitBuffer((short *)buff, (ST32bit *)buff, i);
      break;
    }

  } else if (STisFloat(from_hd) && STisShort(to_hd)) {
    STfloatToShortBuffer((float *)buff, (short *)buff, i);

  } else if (STisFloat(from_hd) && !STisIRCAM(to_hd)) {
    switch (sfclass(to_hd)) {
    case SF_8bit:
      STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
      /* NOTE: WAV format's 8 bit representation is unsigned! */
      if (STisWAV(to_hd)) {
	STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
      }
      break;
    case SF_16bit:
      STfloatTo16bitBuffer((float *)buff, (ST16bit *)buff, i);
      break;
    case SF_24bit:
      STfloatTo24bitBuffer((float *)buff, (ST24bit *)buff, i);
      break;
    case SF_32bit:
      STfloatTo32bitBuffer((float *)buff, (ST32bit *)buff, i);
      break;
    }

  } else if (!STisIRCAM(from_hd) && STisFloat(to_hd)) {
    switch (sfclass(from_hd)) {
    case SF_8bit:
      /* NOTE: WAV format's 8 bit representation is unsigned! */
      if (STisWAV(from_hd)) {
	STUnsignedToSigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
      }
      ST8bitToFloatBuffer((ST8bit *)buff, (float *)buff, i);
      break;
    case SF_16bit:
      STshortToFloatBuffer((ST16bit *)buff, (float *)buff, i);
      break;
    case SF_24bit:
      ST24bitToFloatBuffer((ST24bit *)buff, (float *)buff, i);
      break;
    case SF_32bit:
      ST32bitToFloatBuffer((ST32bit *)buff, (float *)buff, i);
      break;
    }

  } else if (!STisIRCAM(from_hd) && STisShort(to_hd)) {
    switch (sfclass(from_hd)) {
    case SF_8bit:
      /* NOTE: WAV format's 8 bit representation is unsigned! */
      if (STisWAV(from_hd)) {
	STUnsignedToSigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
      }
      ST8bitToShortBuffer((ST8bit *)buff, (short *)buff, i);
      break;
    case SF_24bit:
      ST24bitToShortBuffer((ST24bit *)buff, (short *)buff, i);
      break;
    case SF_32bit:
      ST32bitToShortBuffer((ST32bit *)buff, (short *)buff, i);
      break;
    }

  }

  /*
    NOTE: the following cases have been commented! 
    The reason seems to be that tosf does not read formats other than
    IRCAM floats or shorts (with the aid of Procom headers).

    else if (sfclass(from_hd) == SF_8bit && !STisIRCAM(to_hd)) {

    if (STisWAV(from_hd)) {
    STUnsignedToSigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
    }

    switch (sfclass(to_hd)) {
    case SF_16bit:
    ST8bitToShortBuffer((ST8bit *)buff, (ST16bit *)buff, i);
    break;
    case SF_24bit:
    ST8bitTo24bitBuffer((ST8bit *)buff, (ST24bit *)buff, i);
    break;
    case SF_32bit:
    ST8bitTo32bitBuffer((ST8bit *)buff, (ST32bit *)buff, i);
    break;
    }

    } else if (sfclass(from_hd) == SF_24bit && !STisIRCAM(to_hd)) {
    switch (sfclass(to_hd)) {
    case SF_8bit:
    ST24bitTo8bitBuffer((float *)buff, (ST8bit *)buff, i);

    if (STisWAV(to_hd)) {
    STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
    }

    break;
    case SF_16bit:
    ST24bitToShortBuffer((float *)buff, (ST16bit *)buff, i);
    break;
    case SF_32bit:
    ST24bitTo32bitBuffer((float *)buff, (ST32bit *)buff, i);
    break;
    }

    } else if (sfclass(from_hd) == SF_32bit && !STisIRCAM(to_hd)) {
    switch (sfclass(to_hd)) {
    case SF_8bit:
    ST32bitTo8bitBuffer((float *)buff, (ST8bit *)buff, i);

    if (STisWAV(to_hd)) {
    STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
    }

    break;
    case SF_16bit:
    ST32bitToShortBuffer((float *)buff, (ST16bit *)buff, i);
    break;
    case SF_24bit:
    ST32bitTo24bitBuffer((float *)buff, (ST24bit *)buff, i);
    break;
    }
    }
    */
}


void STconvertToFloatBuffer(SFHEADER *hd, void *buff, int n)
{
  SFHEADER ihd;

  ihd = *hd;
  sfclass(&ihd) = SF_FLOAT;
  sfmachine(&ihd) = SF_MACHINE;

  STconvertSampleBuffer(hd, &ihd, buff, n);
}


void STconvertToShortBuffer(SFHEADER *hd, void *buff, int n)
{
  SFHEADER ihd;

  ihd = *hd;
  sfclass(&ihd) = SF_SHORT;
  sfmachine(&ihd) = SF_MACHINE;

  STconvertSampleBuffer(hd, &ihd, buff, n);
}


void STconvertFromFloatBuffer(SFHEADER *hd, void *buff, int n)
{
  SFHEADER ihd;

  ihd = *hd;
  sfclass(&ihd) = SF_FLOAT;
  sfmachine(&ihd) = SF_MACHINE;

  STconvertSampleBuffer(&ihd, hd, buff, n);
}


void STconvertFromShortBuffer(SFHEADER *hd, void *buff, int n)
{
  SFHEADER ihd;

  ihd = *hd;
  sfclass(&ihd) = SF_SHORT;
  sfmachine(&ihd) = SF_MACHINE;

  STconvertSampleBuffer(&ihd, hd, buff, n);
}


/* 
 * ------------------------------------------------------------------------------------
 *  conversion of encoded header information
 * ------------------------------------------------------------------------------------
 */


static void STcnvCustomSfcodes(SFHEADER *hd)
{
  SFCODE *next, *sfc = &sfcodes(hd);


  while(sfc) {
    next = STnextCode(hd, sfc);

    STswap16(&sfc->code, 1);
    STswap16(&sfc->bsize, 1);

    sfc = next;
  }
}


static void STcnvForeignSfcodes(SFHEADER *hd)
{
  SFCODE *sfc = &sfcodes(hd);


  while(sfc) {
    STswap16(&sfc->code, 1);
    STswap16(&sfc->bsize, 1);

    sfc = STnextCode(hd, sfc);
  }
}


void STcnvForeignHeader(SFHEADER *hd)
{
  /* convert fields */

  STswap32(&sfsrate(hd), 1);
  STswap32(&sfchans(hd), 1);
  STswap32(&sfclass(hd), 1);

 /* convert codes */
  {
    SFCODE *sfc;
    SFMAXAMP *sfm;


    STcnvForeignSfcodes(hd); 

    if ((sfc = STgetsfcode(hd, SF_MAXAMP)) == NULL)
      return;

    sfc++;
    sfm = (SFMAXAMP *)sfc;

    STswap32(sfm->value, sfchans(hd));
    STswap32(sfm->samploc, sfchans(hd));
  }
}


void STcnvCustomHeader(SFHEADER *hd)
{
  /* convert codes */
  {
    SFCODE *sfc;
    SFMAXAMP *sfm;


    if ((sfc = STgetsfcode(hd, SF_MAXAMP))) {
      sfc++;
      sfm = (SFMAXAMP *)sfc;

      STswap32(sfm->value, sfchans(hd));
      STswap32(sfm->samploc, sfchans(hd));
      STswap32(&sfm->timetag, 1);
    }

    STcnvCustomSfcodes(hd);
  }

  /* convert fields */

  STswap32(&sfsrate(hd), 1);
  STswap32(&sfchans(hd), 1);
  STswap32(&sfclass(hd), 1);
}


int STconvertTimeSpec(const char *str, SFHEADER *hd)
{
  if (strchr(str, 's') != NULL) 
    return (int)rint(sfsrate(hd) * atof(str));
  else if (strchr(str, 'S') != NULL) 
    return atoi(str);
  else if (strchr(str, '.') != NULL) 
    return (int)rint(sfsrate(hd) * atof(str));
  else
    return atoi(str);
}


#define FloatToUnsigned(f) ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)

void STconvDoubleToExtended(double num, STextended *ext)
{
  char *bytes = (char *)ext;
  int sign, expon;
  double fMant, fsMant;
  unsigned long hiMant, loMant;


  if (num < 0) {
    sign = 0x8000;
    num *= -1;
  } else {
    sign = 0;
  }

  if (num == 0) {
    expon = 0; hiMant = 0; loMant = 0;
  } else {
    fMant = frexp(num, &expon);
    if ((expon > 16384) || !(fMant < 1)) { 
      expon = sign|0x7FFF; hiMant = 0; loMant = 0;
    } else { 
      expon += 16382;
      if (expon < 0) { 
	fMant = ldexp(fMant, expon);
	expon = 0;
      }
      expon |= sign;
      fMant = ldexp(fMant, 32); 
      fsMant = floor(fMant); 
      hiMant = FloatToUnsigned(fsMant);
      fMant = ldexp(fMant - fsMant, 32); 
      fsMant = floor(fMant); 
      loMant = FloatToUnsigned(fsMant);
    }
  }
 
  bytes[0] = expon >> 8;
  bytes[1] = expon;
  bytes[2] = hiMant >> 24;
  bytes[3] = hiMant >> 16;
  bytes[4] = hiMant >> 8;
  bytes[5] = hiMant;
  bytes[6] = loMant >> 24;
  bytes[7] = loMant >> 16;
  bytes[8] = loMant >> 8;
  bytes[9] = loMant;
}


#define UnsignedToFloat(u) (((double)((long)(u - 2147483647 - 1))) + 2147483648.0)

double STconvExtended(STextended e)
{
  double d;
  int expon;
  unsigned long hiMant, loMant;
  char *bytes = (char *)e;


  expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
  hiMant = ((unsigned long)(bytes[2] & 0xFF) << 24)
    | ((unsigned long)(bytes[3] & 0xFF) << 16)
    | ((unsigned long)(bytes[4] & 0xFF) << 8)
    | ((unsigned long)(bytes[5] & 0xFF));
  loMant = ((unsigned long)(bytes[6] & 0xFF) << 24)
    | ((unsigned long)(bytes[7] & 0xFF) << 16)
    | ((unsigned long)(bytes[8] & 0xFF) << 8)
    | ((unsigned long)(bytes[9] & 0xFF));

  if (expon == 0 && hiMant == 0 && loMant == 0) {
    d = 0;
  } else {
    if (expon == 0x7FFF) { /* Infinity or NaN */
      d = HUGE_VAL;
    } else {
      expon -= 16383;
      d = ldexp(UnsignedToFloat(hiMant), expon-=31);
      d += ldexp(UnsignedToFloat(loMant), expon-=32);
    }
  }

  if (bytes[0] & 0x80)
    return -d;
  else
    return d;
}


#undef UnsignedToFloat
