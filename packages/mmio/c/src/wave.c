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
 */
#include "wave.h"

#define BUF_SIZE_INT8 8192
#define BUF_SIZE_INT16 8192
#define ERRBUF_SIZE 1024
#define MMIO_BUF_SIZE 65536

char mmio_loader_errbuf[ERRBUF_SIZE];

void 
mmio_loader_error(fts_audiofile_t *aufile, char* fmt, ...)
{
  va_list args; 

  va_start(args, fmt); 
  _vsnprintf(mmio_loader_errbuf, ERRBUF_SIZE, fmt, args); 
  va_end(args); 

  if(aufile)
    fts_audiofile_set_error(aufile, mmio_loader_errbuf);

  fts_log("[mmio] %s\n", mmio_loader_errbuf);
}

int 
mmio_loader_open_read(fts_audiofile_t* aufile)
{
  wave_t* wave = NULL;
  int error, num, pos;
  MMCKINFO chunk;
  char* file;
  MMRESULT res;

  wave = (wave_t*) fts_calloc(sizeof(wave_t));
  file = (char*) fts_symbol_name(fts_audiofile_get_filename(aufile));

  /* open the file */
  wave->hmmio = mmioOpen(file, NULL, MMIO_READ | MMIO_DENYWRITE);
  if (wave->hmmio == NULL) {
    mmio_loader_error(aufile, "Failed to open the file (%s)", file);
    goto error_recovery;
  }

  res = mmioSetBuffer(wave->hmmio, NULL, MMIO_BUF_SIZE, 0);
  if (res != 0) {
    mmio_loader_error(aufile, "Failed to set the internal buffer size (%s)", file);
    goto error_recovery;
  }

  /* create the internal buffer */
  if (wave_realloc(wave, 8192) != 0) {
    mmio_loader_error(aufile, "Failed to create the internal buffer (%s)", file);
    goto error_recovery;
  }

  /* check it's a WAVE file */
  error = (int) mmioDescend(wave->hmmio, &wave->riff_chunk, NULL, 0);
  if (error != 0) {
    mmio_loader_error(aufile, "Not a RIFF file (%s)", file);
    goto error_recovery;
  }
  if ((wave->riff_chunk.ckid != FOURCC_RIFF) || (wave->riff_chunk.fccType != mmioFOURCC('W', 'A', 'V', 'E'))) {
    mmio_loader_error(aufile, "Not a WAVE file (%s)", file);
    goto error_recovery;
  }

  /* find the format chunk */
  chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');

  error = (int) mmioDescend(wave->hmmio, &chunk, &wave->riff_chunk, MMIO_FINDCHUNK);
  if (error != 0) {
    if (error == MMIOERR_CHUNKNOTFOUND) {
      mmio_loader_error(aufile, "Couldn't find the format chunk (%s)", file);
      goto error_recovery;
    } else {
      mmio_loader_error(aufile, "Read failed (%s)", file);
      goto error_recovery;
    }
  }

  if (chunk.cksize < (long) sizeof(PCMWAVEFORMAT)) {
    mmio_loader_error(aufile, "Invalid format chunk size (%s)", file);
    goto error_recovery;
  }

  /* read in the format data */
  num = mmioRead(wave->hmmio, (HPSTR) &wave->format, (long) sizeof(wave->format));
  if (num != (long) sizeof(wave->format)) {
    mmio_loader_error(aufile, "Read failed (%s)", file);
    goto error_recovery;
  }
  if (wave->format.wFormatTag != WAVE_FORMAT_PCM) {
    mmio_loader_error(aufile, "Unsupported sample format (%s)", file);
    goto error_recovery;
  }
  wave->format.cbSize = 0;

  if (mmioAscend(wave->hmmio, &chunk, 0) != 0) {
    mmio_loader_error(aufile, "Read failed (%s)", file);
    goto error_recovery;
  }

  if (wave->format.wFormatTag != WAVE_FORMAT_PCM) {
    mmio_loader_error(aufile, "Wave file with unsupported sample format (%s)", file);
    goto error_recovery;    
  }

  /* copy the format data into the audio file object */
  fts_audiofile_set_sample_rate(aufile, wave->format.nSamplesPerSec);
  fts_audiofile_set_num_channels(aufile, wave->format.nChannels);

  /* set the file position to the beginning of the data chunk */
  pos = mmioSeek(wave->hmmio, wave->riff_chunk.dwDataOffset + sizeof(FOURCC), SEEK_SET);
  if (pos < 0) {
    mmio_loader_error(aufile, "Failed to set the read position (%s)", file);
    goto error_recovery;
  }
  wave->data_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioDescend(wave->hmmio, &wave->data_chunk, &wave->riff_chunk, MMIO_FINDCHUNK) != 0) {
    mmio_loader_error(aufile, "Failed to set the read position (%s)", file);
    goto error_recovery;
  }
  fts_audiofile_set_num_frames(aufile, 8 * wave->data_chunk.cksize / wave->format.nChannels / wave->format.wBitsPerSample);

  fts_audiofile_set_handle(aufile, (void*) wave);

  return 0;

 error_recovery:

  if (wave != NULL) {
    wave_delete(wave);
  }
  return -1;
}

int 
mmio_loader_open_write(fts_audiofile_t* aufile)
{
  wave_t* wave = NULL;
  MMCKINFO chunk;
  char* file;
  int bytes_per_sample;
  fts_symbol_t sample_format;
  MMRESULT res;

  sample_format = fts_audiofile_get_sample_format(aufile);
  if (sample_format == fts_s_int8) {
    bytes_per_sample = 1;
  } else if (sample_format == fts_s_int16) {
    bytes_per_sample = 2;
  } else {
    mmio_loader_error(aufile, "Only the int8 and int16 formats are supported");
    goto error_recovery;
  }

  wave = (wave_t*) fts_calloc(sizeof(wave_t));
  file = (char*) fts_symbol_name(fts_audiofile_get_filename(aufile));

  /* open the file */
  wave->hmmio = mmioOpen(file, NULL, MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE | MMIO_EXCLUSIVE);
  if (wave->hmmio == NULL) {
    mmio_loader_error(aufile, "Failed to open the file (%s)", file);
    goto error_recovery;
  }

  res = mmioSetBuffer(wave->hmmio, NULL, MMIO_BUF_SIZE, 0);
  if (res != 0) {
    mmio_loader_error(aufile, "Failed to set the internal buffer size (%s)", file);
    goto error_recovery;
  }

  /* create the internal buffer */
  if (wave_realloc(wave, 8192) != 0) {
    mmio_loader_error(aufile, "Failed to create the internal buffer (%s)", file);
    goto error_recovery;
  }

  /* create the RIFF header */
  wave->riff_chunk.ckid = FOURCC_RIFF;
  wave->riff_chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  wave->riff_chunk.cksize = 0;

  if (mmioCreateChunk(wave->hmmio, &wave->riff_chunk, MMIO_CREATERIFF) != MMSYSERR_NOERROR) {
    mmio_loader_error(aufile, "Failed to create the wave chunk (%s)", file);
    goto error_recovery;
  }

  /* initialize the format */
  wave->format.wFormatTag = WAVE_FORMAT_PCM;
  wave->format.nChannels = fts_audiofile_get_num_channels(aufile);
  wave->format.nSamplesPerSec = fts_audiofile_get_sample_rate(aufile);
  wave->format.wBitsPerSample = bytes_per_sample * 8;
  wave->format.nBlockAlign = wave->format.nChannels * bytes_per_sample;
  wave->format.nAvgBytesPerSec = wave->format.nSamplesPerSec * wave->format.nBlockAlign;
  wave->format.cbSize = 0;

  /* create the format chunk */
  chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
  chunk.cksize = (long) sizeof(wave->format);
  if (mmioCreateChunk(wave->hmmio, &chunk, 0) != MMSYSERR_NOERROR) {
    mmio_loader_error(aufile, "Failed to create the format chunk (%s)", file);
    goto error_recovery;
  }  
  if (mmioWrite(wave->hmmio, (HPSTR) &wave->format, sizeof(wave->format)) < sizeof(wave->format)) {
    mmio_loader_error(aufile, "Failed to write the format chunk (%s)", file);
    goto error_recovery;
  }
  if (mmioAscend(wave->hmmio, &chunk, 0) != 0) {
    mmio_loader_error(aufile, "Failed to ascend from the format chunk (%s)", file);
    goto error_recovery;
  }

  /* create the data chunk */
  wave->data_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  wave->data_chunk.cksize = 0;

  if (mmioCreateChunk(wave->hmmio, &wave->data_chunk, 0) != MMSYSERR_NOERROR) {
    mmio_loader_error(aufile, "Failed to create the data chunk (%s)", file);
    goto error_recovery;
  }  

  fts_audiofile_set_handle(aufile, (void*) wave);

  return 0;

 error_recovery:

  if (wave != NULL) {
    wave_delete(wave);
  }
  return -1;
}

int 
mmio_loader_read(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen)
{
  wave_t* wave = (wave_t*) fts_audiofile_get_handle(aufile);

  if (wave == NULL) {
    return -1;
  } else if (wave->format.wBitsPerSample == 8) {
    return wave_read_int8(wave, buf, nbuf, buflen);
  } else if (wave->format.wBitsPerSample == 16) {
    return wave_read_int16(wave, buf, nbuf, buflen);
  } else {
    return -1;
  }
}


int 
mmio_loader_write(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen)
{
  wave_t* wave = (wave_t*) fts_audiofile_get_handle(aufile);
  int err;

  if (wave == NULL) {
    return -1;
  } else if (wave->format.wBitsPerSample == 8) {
    err = wave_write_int8(wave, buf, nbuf, buflen);
  } else if (wave->format.wBitsPerSample == 16) {
    err = wave_write_int16(wave, buf, nbuf, buflen);
  } else {
    return -1;
  }

  if (err == -1) {
    mmio_loader_close(aufile);
  }
  
  return err;
}

int 
mmio_loader_seek(fts_audiofile_t* aufile, unsigned int offset)
{
  wave_t* wave = (wave_t*) fts_audiofile_get_handle(aufile);
  unsigned int pos;

  if ((wave != NULL) && (wave->hmmio != NULL)) {

    offset = wave->riff_chunk.dwDataOffset + sizeof(FOURCC) + offset * wave->format.nBlockAlign;
    pos = mmioSeek(wave->hmmio, offset, SEEK_SET);

    if (pos < 0) {
      mmio_loader_error(aufile, "Failed to set the seek the position");
      return -1;
    }
    
    return 0;
  }
  return -1;
}

int 
mmio_loader_close(fts_audiofile_t* aufile)
{
  wave_t* wave = (wave_t*) fts_audiofile_get_handle(aufile);
  if (wave != NULL) {
    wave_delete(wave);
    fts_audiofile_set_handle(aufile, NULL);
  }
  return 0;
}

void 
wave_delete(wave_t* wave)
{
  MMIOINFO info;
  MMRESULT res;

  if (wave == NULL) {
    return;
  }

  if (wave->hmmio != NULL) {

    /* check if this file is open for writing. if so, update the
       chunks as required */
    res = mmioGetInfo(wave->hmmio, &info, 0);
    if ((res == 0) 
	&& (info.dwFlags & MMIO_WRITE)
	&& (mmioAscend(wave->hmmio, &wave->data_chunk, 0) == 0)) {
      mmioAscend(wave->hmmio, &wave->riff_chunk, 0);
    }

    /* close the file */
    mmioClose(wave->hmmio, 0);
  }
  if (wave->buffer != NULL) {
    fts_free(wave->buffer);
  }

  fts_free(wave);
}

int 
wave_read_int8(wave_t* wave, float** buf, int nbuf, unsigned int buflen)
{
  float* fbuf;
  int n, nchan;
  unsigned int size, frames, i, k;
  unsigned char* cbuf;

  nchan = wave->format.nChannels;
  size = wave->format.nBlockAlign * buflen;

  if ((size > wave->buf_size) && (wave_realloc(wave, size) != 0)) {
    mmio_loader_error(0, "Out of memory");
    goto error_recovery;
  }

  /* read the samples */
  size = mmioRead(wave->hmmio, (HPSTR) wave->buffer, size);
  if (size < 0) {
    goto error_recovery;
  }

  frames = size / nchan;
  cbuf = (unsigned char*) wave->buffer;

  /* copy the samples in to the buffer. if there are more channels
     requested than available in the file, fill the remaining
     buffers with zeros. */
  for (n = 0; n < nbuf; n++) {

    fbuf = buf[n];

    if (n < nchan) {
      for (i = 0, k = n; i < frames; i++, k += nchan) {
	fbuf[i] = ((float) cbuf[k] / 128.0f) - 1.0f;
      }
      for (i = frames; i < buflen; i++) {
	fbuf[i] = 0.0f;
      }
    } else {
      for (i = 0; i < buflen; i++) {
	fbuf[i] = 0.0f;
      }
    }
  }

  return frames;

 error_recovery:

  for (n = 0; n < nbuf; n++) {
    fbuf = buf[n];    
    for (i = 0; i < buflen; i++) {
      fbuf[i] = 0.0f;
    }
  }
  return -1;
}

int 
wave_read_int16(wave_t* wave, float** buf, int nbuf, unsigned int buflen)
{
  float* fbuf;
  short* sbuf;
  int n, nchan;
  unsigned int size, frames, i, k;

  nchan = wave->format.nChannels;
  size = wave->format.nBlockAlign * buflen;

  if ((size > wave->buf_size) && (wave_realloc(wave, size) != 0)) {
    mmio_loader_error(0, "Out of memory");
    goto error_recovery;
  }

  /* read the samples */
  size = mmioRead(wave->hmmio, (HPSTR) wave->buffer, size);
  if (size < 0) {
    goto error_recovery;
  }

  frames = size / nchan / 2;
  sbuf = (short*) wave->buffer;

  /* copy the samples in to the buffer. if there are more channels
     requested than available in the file, fill the remaining
     buffers with zeros. */
  for (n = 0; n < nbuf; n++) {

    fbuf = buf[n];

    if (n < nchan) {
      for (i = 0, k = n; i < frames; i++, k += nchan) {
	fbuf[i] = (float) sbuf[k] / 32768.0f;
      }
      for (i = frames; i < buflen; i++) {
	fbuf[i] = 0.0f;
      }
    } else {
      for (i = 0; i < buflen; i++) {
	fbuf[i] = 0.0f;
      }
    }
  }

  return frames;

 error_recovery:

  for (n = 0; n < nbuf; n++) {
    fbuf = buf[n];    
    for (i = 0; i < buflen; i++) {
      fbuf[i] = 0.0f;
    }
  }
  return -1;
}

int 
wave_write_int8(wave_t* wave, float** buf, int nbuf, unsigned int buflen)
{
  float* fbuf;
  int ch, nchan;
  unsigned int size, i, k;
  unsigned char* cbuf;

  nchan = wave->format.nChannels;
  size = wave->format.nBlockAlign * buflen;

  if ((size > wave->buf_size) && (wave_realloc(wave, size) != 0)) {
    mmio_loader_error(0, "Out of memory");
    return -1;
  }

  cbuf = wave->buffer;

  /* copy the samples in to the buffer. if there are more channels
     requested than available in the file, fill the remaining
     buffers with zeros. */
  for (ch = 0; ch < nchan; ch++) {
    if (ch < nbuf) {
      fbuf = buf[ch];
      for (i = 0, k = ch; i < buflen; i++, k += nchan) {
	cbuf[k] = (unsigned char) ((fbuf[i] + 1.0f) * 127.0f);
      }
    } else {
      for (i = 0, k = ch; i < buflen; i++, k += nchan) {
	cbuf[k] = 128;
      }
    }
  }

  /* write the samples */
  size = mmioWrite(wave->hmmio, (HPSTR) wave->buffer, size);
  
  if (size < 0) {
    return -1;
  }
  if (size == 0) {
    return 0;
  }

  return size / nchan;
}

int 
wave_write_int16(wave_t* wave, float** buf, int nbuf, unsigned int buflen)
{
  float* fbuf;
  short* sbuf;
  int ch, nchan;
  unsigned int size, i, k;

  nchan = wave->format.nChannels;
  size = wave->format.nBlockAlign * buflen;

  if ((size > wave->buf_size) && (wave_realloc(wave, size) != 0)) {
    mmio_loader_error(0, "Out of memory");
    return -1;
  }

  sbuf = (short*) wave->buffer;

  /* copy the samples in to the buffer. if there are more channels
     requested than available in the file, fill the remaining
     buffers with zeros. */
  for (ch = 0; ch < nchan; ch++) {
    if (ch < nbuf) {
      fbuf = buf[ch];
      for (i = 0, k = ch; i < buflen; i++, k += nchan) {
	sbuf[k] = (short) (fbuf[i] * 32767.0f);
      }
    } else {
      for (i = 0, k = ch; i < buflen; i++, k += nchan) {
	sbuf[k] = 0;
      }
    }
  }

  /* write the samples */
  size = mmioWrite(wave->hmmio, (HPSTR) wave->buffer, size);
  
  if (size < 0) {
    return -1;
  }
  if (size == 0) {
    return 0;
  }

  return size / nchan / 2;
}

int 
wave_realloc(wave_t* wave, unsigned int size)
{
  MMRESULT res;

  if (wave->buffer == NULL) {
    wave->buffer = (char*) fts_malloc(size);
    wave->buf_size = size;
  } else {
    wave->buffer = (char*) fts_realloc(wave->buffer, size);
    wave->buf_size = size;
  }

  if (wave->buffer == NULL) {
    return -1;
  }

  res = mmioSetBuffer(wave->hmmio, NULL, size, 0);
  return (res == 0)? 0 : -1;
}
