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
#ifndef _SND_H_
#define _SND_H_

/* next soundfile header */

typedef struct{
  long magic;          /* must be equal to SND_MAGIC */
  long dataLocation;   /* Offset or pointer to the raw data */
  long dataSize;       /* Number of bytes of data in the raw data */
  long dataFormat;     /* The data format code */
  long samplingRate;   /* The sampling rate */
  long channelCount;   /* The number of channels */
  long info;           /* Textual information relating to the sound. */
} SNDSoundStruct;

#define SND_MAGIC ((long)0x2e736e64)
#define SND_FORMAT_LINEAR_16 (3)

#endfif

