#ifndef _ST_AIFF_PH_
#define _ST_AIFF_PH_


#include "aiff.h"


extern int STreadForm(int fd, STform *form);
/*
 * Reads overall form and scans chunk headers.
 * Performs possible byteorder conversion.
 * Returns 0 on success, -1 on failure while printing error messages to stderr.
 */

extern int STreadCommonChunk(int fd, STform *form, STaifcCommonChunk *common);
/*
 * Reads common chunk from an already scanned form.
 * Performs possible byteorder conversion.
 * You should allways supply a STaifcCommonChunk struct, regardless wheter
 * the file is a 'aiff' or 'aifc'. In the 'aiff' case, the compression type
 * is set to 'NONE'.
 * Returns 0 on success, -1 on failure while printing error messages to stderr.
 */

extern int STreadSoundDataChunk(int fd, STform *form, STsoundDataChunk *data);
/*
 * Reads sound data chunk from an already scanned form.
 * Performs possible byteorder conversion.
 * Seeks to beginning of sound data.
 * Returns 0 on success, -1 on failure while printing error messages to stderr.
 */

extern int STupdateAIFXsampleCount(int fd, SFHEADER *hd);
/*
 * Updates the samleCount and size SSND chunk field in a currently opened file.
 * The file must be opened for reading & writing and the file position after the
 * the last sample. A pad byte will be added if needed. 
 * Returns 0 on success, -1 on failure. Seeks to the end of file before returning.
 * The soundData chunk is assumed to be at the end.
 */

extern int STwriteAIFCheader(int fd, const SFHEADER *hd, int sampleCount);
/*
 * This routine writes an AIFC header with common and soundData chunk
 * ready to put the samples afterward.
 * Returns 0 on success, -1 on failure.
 */

extern int STwriteAIFFheader(int fd, const SFHEADER *hd, int sampleCount);
/*
 * This routine writes an AIFF header with common and soundData chunk
 * ready to put the samples afterward.
 * Returns 0 on success, -1 on failure.
 */


#endif /* _ST_AIFF_PH_ */

