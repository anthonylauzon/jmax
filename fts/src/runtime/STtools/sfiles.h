#ifndef _ST_SFILES_H_
#define _ST_SFILES_H_

#include "sfheader.h"
#include "wav.h" /* for WORD and DWORD defs */

extern int STread(int fd, char *buff, int count);
/*
 * Pipe safe read.
 * Return -1 on failure, or bytes read.
 */

extern int STreadBigEndianShort(int fd, short *s);
extern int STreadBigEndian16bit(int fd, ST16bit *i);
extern int STreadBigEndian24bit(int fd, ST24bit *i);
extern int STreadBigEndian32bit(int fd, ST32bit *i);
/*
 * Pipe safe big endian read.
 * Return -1 on failure, or bytes read.
 */
extern int STreadLittleEndianWORD(int fd, WORD *w);
extern int STreadLittleEndianDWORD(int fd, DWORD *dw);
/*
 * Pipe safe little endian read for WAV file header.
 * Return -1 on failure, or bytes read.
 */

extern int STwrite(int fd, char *buff, int count);
/*
 * Pipe safe write.
 * Return -1 on failure, or bytes written.
 */

extern int STwriteBigEndianShort(int fd, short s);
extern int STwriteBigEndian16bit(int fd, ST16bit i);
extern int STwriteBigEndian24bit(int fd, ST24bit i);
extern int STwriteBigEndian32bit(int fd, ST32bit i);
/*
 * Pipe safe big endian write.
 * Return -1 on failure, or bytes written.
 */

extern int STwriteLittleEndianWORD(int fd, WORD w);
extern int STwriteLittleEndianDWORD(int fd, DWORD dw);
/*
 * Pipe safe little endian write for WAV file header.
 * Return -1 on failure, or bytes written.
 */

extern int STwheader(int fd, const SFHEADER *hd);
/* 
 * Writes header hd in file fd returning 0 on success, -1 on failure. 
 * Writes error messages to stderr.
 * Automatic conversion without changing hd.
 */

extern int STrheader(int fd, SFHEADER *hd);
/* 
 * Reads header hd from file fd with automatic conversion returning 0 on success, -1 on failure. 
 * Automatic conversion.
 * Writes error messages to stderr.
 * Seeks to begin of sample data.
 */

extern int STreadopensf(const char *path, SFHEADER *hd);
/*
 * Reads soundfile header hd from path returning filedescriptor on success, -1 on failure.
 * Writes error messages to stderr.
 * Automatic conversion of the header.
 * Seeks to begin of sample data.
 * Header space should be already allocated.
 * Path is sf expanded (see STgetsfname).
 */

extern int STwriteopensf(const char *path, SFHEADER *hd);
/*
 * Writes soundfile header hd in path returning filedescriptor on success, -1 on failure.
 * Writes error messages to stderr.
 * Automatic conversion without changing hd.
 * Seeks to begin of sample data.
 * Path is sf expanded (see STgetsfname).
 */

extern int STrwopensf(const char *path, SFHEADER *hd);
/*
 * Reads soundfile header hd from path returning filedescriptor on success, -1 on failure.
 * Opens file for reading and writing.
 * Writes error messages to stderr.
 * Automatic conversion of the header.
 * Seeks to begin of sample data.
 * Header space should be already allocated.
 * Path is sf expanded (see STgetsfname).
 */

extern int STclose(int fd, SFHEADER *hd);
/*
 * Closes the file descriptor. For writable AIFF/AIFC files the header fields
 * will be updated.
 * Returns 0 on success, -1 on failure.
 */

#define	DEFAULT_HOME_SFDIR	"/snd/"
#define	DEFAULT_SFDIR	      "~"

extern const char *STgetsfname(const char *shortname);
/*
 * Expands shortname to soundfile paths:
 * Paths beginning with '/', './', '../', '~' have their standard meaning
 * while relative paths are located in $SFDIR.
 */

extern int STsampleCount(int fd, SFHEADER *hd);
/*
 * Returns the sample count in file with descriptor fd, on error -1.
 */

extern float STsfseconds(int fd, SFHEADER *hd);
/*
 * Returns the sample count duration in seconds of file with descriptor fd, on error -1.
 */

extern int STreadSampBuffer(int fd, SFHEADER *hd, void *buff, int n);
/*
 * Reads channel independent samples in buffer from file id with automatic conversion.
 * Returns channel independent samples read or -1 on failure.
 * Writes error messages to stderr.
 */

extern int STwriteSampBuffer(int fd, SFHEADER *hd, void *buff, int n);
/*
 * Writes channel independent samples in buffer to file id with automatic conversion.
 * Returns channel independent samples read or -1 on failure.
 * Writes error messages to stderr.
 */

extern int STseekToSample(int fd, SFHEADER *hd, int sample);
/*
 * Seeks to the specified sample in the soundfile.
 * Returns -1 on failure, 0 on success. 
 */


#endif /* _ST_SFILES_H_ */
