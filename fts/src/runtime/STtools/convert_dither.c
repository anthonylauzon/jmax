#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "convert.h"

/* 
	 These library functions are to add additive dithering to the conversion
	 routines in libST/convert.c
	 In order to achieve this, "STconvertSampleBufferWithDithering" converts
	 an input sample buffer to floating point numbers, adds 2 LSB dithering,
	 and converts to the integer format desired. Wherever dithering makes
	 no sense, the original version "STconvertSampleBuffer" is called.

	 Peter Hoffmann, IRCAM, 09/02/1998
*/

/* this function is only to test different random number generators */
float STuniform_random (void) {

	/* draw random number, scale from [0..2**31-1] to [-1.0..1.0] */
	/*
	return( (float) (random()/(ST32bitMaxInDouble/2.0))-1.0);
	*/
	return( (float) (drand48()*2.0 - 1.0));
}

double STdither(void) {
	/* 
		 This function draws random numbers in the interval [-1.0..~1.0] with
		 a triangular distribution.
		 Before calling this function for the first time, the random number
		 generator should be initialized, e.g. srand48 with current daily time.
		 The formula implemented is:

		 STdither = (1 - random_value**(1/2)) or
		 STdither = (random_value**(1/2) - 1)
		 depending on a coin tossing,

		 where random_value = [0.0..~1.0]		 
	*/

	/* allocate 48 bit state array for jrand48 */
	/* 
		 static unsigned short xsubi[3] = {0,0,0}; 
	*/

	/* draw uncorrelated random numbers */
	double rnd = drand48(); /* [0.0..~1.0] */
	/* long sign = jrand48(xsubi); */ /* [-2**31..~2**31] */
	long sign = random() - 1073741824L;
	if (sign >= 0) { /* head or tail? */
		return ((1.0 - sqrt(rnd))); /* [0.0..~1.0] */
	}
	else {
		return ((sqrt(rnd) - 1.0)); /* [-1.0..~0.0] */
	}
}

/* 
	 This function adds a random dither signal with 2 LSB amplitude to a
	 floating point sample buffer in order to decorrelate quantization
	 noise from the input signal.
	 The argument 1/imax is the value of 2 LSBs of the destination 
	 quantization format 2**(-(m-1), 
	 e.g. 2**(-(8-1)) = 1/ST8bitMax etc., as defined in "convert.h".
	 I.e. the same argument is passed as to the function "STfloatToInteger" 
	 in libST/convert.c
*/

void STaddDitherToBuffer(float *fbuff, int n, double imax)
{
	/*
	int i;
	for (i = 0; i < n; i++) {
		fbuff[i] += (float) (STdither() / imax);
	}
	*/
	while (n-- > 0)
		*fbuff++ += (float) (STdither() / imax);
}

/*
	This function treats all conversion cases where dither should be added:
	either when passing from floats to integers (8, 16, 24, or 32 bit)
	or when requantizing with less precision (e.g. 24 to 16 bit).
	In all other cases, it degrades to STconvertSampleBuffer defined in convert.c
 */
/* NOTE: in STconvertSampleBuffer in convert.c, in both the if-branches
	 which are to be found in the commented section of the function body:
        } else if (sfclass(from_hd) == SF_24bit && !STisIRCAM(to_hd)) {
	 and
        } else if (sfclass(from_hd) == SF_32bit && !STisIRCAM(to_hd)) {,
	 the from-buffer pointers are casted to float, whereas they should be casted
	 to either ST24bit or ST32bit!
*/ 
int STconvertSampleBufferWithDither(SFHEADER *from_hd, SFHEADER *to_hd, void *buff, int i)
{
	int error_code = 0; /* success */

	if (STisShort(from_hd) && !STisIRCAM(to_hd)) {
		switch (sfclass(to_hd)) {
		case SF_8bit:
			STshortToFloatBuffer((short *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST8bitMax);
			STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
			/* NOTE: WAV format's 8 bit representation is unsigned! */
			if (STisWAV(to_hd)) {
				STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
			}
			break;
		default:
			STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1; /* warning to report ignoring of -D option */
		}
	} else if (STisFloat(from_hd) && STisShort(to_hd)) {
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);
			STfloatToShortBuffer((float *)buff, (short *)buff, i);
	} else if (STisFloat(from_hd) && !STisIRCAM(to_hd)) {
		switch (sfclass(to_hd)) {
		case SF_8bit:
			STaddDitherToBuffer((float *)buff, i, ST8bitMax);			
			STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
			/* NOTE: WAV format's 8 bit representation is unsigned! */
			if (STisWAV(to_hd)) {
				STSignedToUnsigned8bitBuffer((ST8bit *)buff, (ST8bit *)buff, i);
			}
			break;
		case SF_16bit:
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);
			STfloatTo16bitBuffer((float *)buff, (ST16bit *)buff, i);
			break;
		case SF_24bit:
			STaddDitherToBuffer((float *)buff, i, ST24bitMax);
			STfloatTo24bitBuffer((float *)buff, (ST24bit *)buff, i);
			break;
		case SF_32bit:
			STaddDitherToBuffer((float *)buff, i, ST32bitMaxInDouble);
			STfloatTo32bitBuffer((float *)buff, (ST32bit *)buff, i);
			break;
		default:
			STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1; /* warning to report ignoring of -D option */
		}
	} else if (!STisIRCAM(from_hd) && STisShort(to_hd)) {
		switch (sfclass(from_hd)) {
		case SF_24bit:
			ST24bitToFloatBuffer((ST24bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);			
			STfloatToShortBuffer((float *)buff, (short *)buff, i);
			break;
		case SF_32bit:
			ST32bitToFloatBuffer((ST32bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);			
			STfloatToShortBuffer((float *)buff, (short *)buff, i);
			break;
		default:
			STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1; /* warning to report ignoring of -D option */
		}
	}
/* 
	 NOTE: the following cases are commented in STconvertSampleBuffer! 
	 the reason seems to be that tosf does not read formats other than
	 IRCAM floats or shorts (with the aid of Procom headers).

  else if (sfclass(from_hd) == SF_16bit && !STisIRCAM(to_hd)) {
	  switch (sfclass(to_hd)) {
	  case SF_8bit:
			ST24bitToFloatBuffer((ST24bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST8bitMax);
			STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
  	  break;
	  default:
	    STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1;
		}
	} else if (sfclass(from_hd) == SF_24bit && !STisIRCAM(to_hd)) {
	  switch (sfclass(to_hd)) {
	  case SF_8bit:
			ST24bitToFloatBuffer((ST24bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST8bitMax);
			STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
  	  break;
	  case SF_16bit:
			ST24bitToFloatBuffer((ST24bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);
			STfloatTo16bitBuffer((float *)buff, (ST16bit *)buff, i);
	    break;
	  default:
	    STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1;
		}
	} else if (sfclass(from_hd) == SF_32bit && !STisIRCAM(to_hd)) {
	  switch (sfclass(to_hd)) {
	  case SF_8bit:
			ST32bitToFloatBuffer((ST32bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST8bitMax);
			STfloatTo8bitBuffer((float *)buff, (ST8bit *)buff, i);
	    break;
	  case SF_16bit:
			ST32bitToFloatBuffer((ST32bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST16bitMax);
			STfloatTo16bitBuffer((float *)buff, (ST16bit *)buff, i);
	    break;
	  case SF_24bit:
			ST32bitToFloatBuffer((ST32bit *)buff, (float *)buff, i);
			STaddDitherToBuffer((float *)buff, i, ST24bitMax);
			STfloatTo24bitBuffer((float *)buff, (ST24bit *)buff, i);
	    break;
	  default:
	    STconvertSampleBuffer(from_hd, to_hd, buff, i);
			error_code = 1;
		}
	}
	*/
	else {
		STconvertSampleBuffer(from_hd, to_hd, buff, i);
		error_code = 1; /* warning to report ignoring of -D option */
	}
	return (error_code);
}




