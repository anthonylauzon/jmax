#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

#ifdef sgi
#include <bstring.h>
#endif

#ifdef sun
#define	MIN(a, b)	((a) < (b) ? (a) : (b))
#define	MAX(a, b)	((a) < (b) ? (b) : (a))
#else
#include <sys/param.h>
#endif

#include "sfiles.h"
#include "convert.h"
#include "sfcodes.h"
#include "sfheader.h"

/* 
	 NOTE: these definitions using SFHEADER are in separate local header files to avoid mutual references
	 of aiff/wav header files and sfheader.h, which uses aiff and wav structures for the declaration
	 of SFHEADER. Unfortunately, the C compiler used is still a ONE PASS COMPILER. Poor UNIX world...
*/
#include "aiffP.h"
#include "wavP.h"

#define STsonicBlock    262144
#define STsonicUsed     245760
#define STsonicSkipped  16384


/*
 * This library has been modified in order to include Microsoft's proprietary
 * RIFF sound file format (WAV).
 *
 * Treatment of WAV has been added to:
 * STsampleToFilePosition, STseekToSample, STrheader, STwheader, STsampleCount, 
 * STreadSampBuffer, STwriteSampBuffer, STclose. 
 *
 * New functions are: STread/writeLittleEndianWORD, STread/writeLittleEndianDWORD
 *
 * Peter Hoffmann, IRCAM, 04/98
 */

/* 
 * ------------------------------------------------
 * return byte offset of first sample in sound file
 * ------------------------------------------------
 */
static int STsampleToFilePosition(SFHEADER *hd, int sample)
{
	if (STisSONIC(hd)) {
		const int 
			firstBlockBytes = STsonicUsed - hd->aiff.soundDataPosition % STsonicBlock;

		int
			bytes = sample * sfchans(hd) * hd->aiff.common.sampWidth / 8,
			pos = hd->aiff.soundDataPosition,
			n;

		while (bytes > 0) 
			if (pos == hd->aiff.soundDataPosition) {
				n = MIN(bytes, firstBlockBytes);
				pos += n;
				bytes -= n;
			} else {
				n = MIN(bytes, STsonicUsed);
				pos += STsonicSkipped + n;
				bytes -= n;
			}

		return pos;
	}

	if (STisAIFX(hd))
		return sample * sfchans(hd) * hd->aiff.common.sampWidth / 8 + hd->aiff.soundDataPosition;

	if (STisWAV(hd)) /* WAV headers are always 44 bytes long.*/
		return sample * sfchans(hd) * hd->wav.nBitsPerSamples / 8 + WAVHDRLEN; 

	return sizeof(SFHEADER) + sample * sfchans(hd) * sfclass(hd);
}

/* 
 * ------------------------------------------------
 * on raw byte level: fault tolerant read
 * ------------------------------------------------
 */
int STread(int fd, char *buff, int count)
{
	int   ret, got = 0;
	char  *p = buff;


	do {
		ret = read(fd, p, count - got);
		if (ret == 0)
			break;
		else if (ret < 0) {
			if (errno != EAGAIN)
				return -1;
		} else {
			p += ret;
			got += ret;
		}
	} while (got < count);

	return got;
}

/*
 * ------------------------------------------------
 * on raw byte level: fault tolerant write
 * ------------------------------------------------
 */

int STwrite(int fd, char *buff, int count)
{
	int   ret, got = 0;
	char  *p = buff;


	do {
		ret = write(fd, p, count - got);
		if (ret == 0)
			break;
		else if (ret < 0) {
			if (errno != EAGAIN)
				return -1;
		} else {
			p += ret;
			got += ret;
		}
	} while (got < count);

	return got;
}

/*
 * ------------------------------------------------
 * on sample level, byte order tolerant write 
 * ------------------------------------------------
 */

int STwriteBigEndianShort(int fd, short s)
{
	if (STlittleEndianCPU)
		STswap16((char *)&s, 1);

	return STwrite(fd, (char *)&s, sizeof(short));
}


int STwriteBigEndian16bit(int fd, ST16bit i)
{
	if (STlittleEndianCPU)
		STswap16((char *)&i, 1);

	return STwrite(fd, (char *)&i, sizeof(ST16bit));
}


int STwriteBigEndian24bit(int fd, ST24bit i)
{
	if (STlittleEndianCPU)
		STswap24((char *)&i, 1);

	return STwrite(fd, (char *)&i, sizeof(ST24bit));
}


int STwriteBigEndian32bit(int fd, ST32bit i)
{
	if (STlittleEndianCPU)
		STswap32((char *)&i, 1);

	return STwrite(fd, (char *)&i, sizeof(ST32bit));
}

/* the same for little endian WAV files: */

int STwriteLittleEndianWORD(int fd, WORD w)
{
	if (STbigEndianCPU)
		STswap16((char *)&w, 1);

	return STwrite(fd, (char *)&w, sizeof(WORD));
}

int STwriteLittleEndianDWORD(int fd, DWORD dw)
{
	if (STbigEndianCPU)
		STswap32((char *)&dw, 1);

	return STwrite(fd, (char *)&dw, sizeof(DWORD));
}

/*
 * ------------------------------------------------
 * on sample level, byte order tolerant read 
 * ------------------------------------------------
 */

int STreadBigEndianShort(int fd, short *s)
{
	int ret = STread(fd, (char *)s, sizeof(short));

	if (STlittleEndianCPU)
		STswap16((char *)s, 1);

	return ret;
}


int STreadBigEndian16bit(int fd, ST16bit *i)
{
	int ret = STread(fd, (char *)i, sizeof(ST16bit));

	if (STlittleEndianCPU)
		STswap16((char *)i, 1);

	return ret;
}


int STreadBigEndian24bit(int fd, ST24bit *i)
{
	int ret = STread(fd, (char *)i, sizeof(ST24bit));

	if (STlittleEndianCPU)
		STswap24((char *)i, 1);

	return ret;
}


int STreadBigEndian32bit(int fd, ST32bit *i)
{
	int ret = STread(fd, (char *)i, sizeof(ST32bit));

	if (STlittleEndianCPU)
		STswap32((char *)i, 1);
	
	return ret;
}

/* the same for little endian WAV files: */

int STreadLittleEndianWORD(int fd, WORD *w)
{
	int ret = STread(fd, (char *)w, sizeof(WORD));

	if (STbigEndianCPU)
		STswap16((char *)w, 1);

	return ret;
}

int STreadLittleEndianDWORD(int fd, DWORD *dw)
{
	int ret = STread(fd, (char *)dw, sizeof(DWORD));

	if (STbigEndianCPU)
		STswap32((char *)dw, 1);

	return ret;
}

/*
 * ------------------------------------------------
 * read/write sound file header, byte order transparent 
 * ------------------------------------------------
 */

int STwheader(int fd, const SFHEADER *hd)
{
	SFHEADER lh = *hd;


	if (STisForeignIRCAM(hd)) /* swap byte order in header! */
		STcnvCustomHeader(&lh);

	if (fd != fileno(stdout) && !isatty(fd)) 
		lseek(fd, 0, SEEK_SET); 

	if ((STisIRCAM(hd) && STwrite(fd, (char *)&lh, sizeof(SFHEADER)) != sizeof(SFHEADER)) ||
			(STisAIFF(hd) && STwriteAIFFheader(fd, hd, 0) < 0) || 
			(STisAIFC(hd) && STwriteAIFCheader(fd, hd, 0) < 0) ||
			(STisWAV(hd) && STwriteWAVheader(fd, hd, 0) < 0)) {
		fprintf(stderr, "sfiles (STwheader): Can't write soundfile header!\n");
		return -1;
	}

	return 0;
}


int STrheader(int fd, SFHEADER *hd)
{
	int ret;

	if (fd != fileno(stdin) && !isatty(fd)) 
		lseek(fd, 0, SEEK_SET); 

	if (STread(fd, (char *)hd, sizeof(SFHEADER)) != sizeof(SFHEADER)) {
		fprintf(stderr, "sfiles (STrheader): Error while reading soundfile header!\n");
		return -1;
	}

	/* check for other types of soundfile formats */

	if (!STisIRCAM(hd)) {
		if (fd == fileno(stdin)) {
			fprintf(stderr, "sfiles (STrheader):  Sorry, can't read other than IRCAM soundfiles from stdin!\n");
			return -1;
		}
		
		/* check for WAV type */
		if ((ret = STreadWAVheader(fd, &hd->wav)) > 0) {
			/* was able to recognize WAV header */
			if (ret != sizeof(WAVHDR)) {
				fprintf(stderr, "sfiles (STrheader): read other than expected size of WAV header: %d!\n", ret);
				return -1;
			}
			/* check for PCM format */
			if (hd->wav.wFormatTag != 1) {
				fprintf(stderr, "sfiles (STrheader): can only read PCM WAV files!\n");
				return -1;
			}
			
			/* check for compression ??? */
			
			/* check for lousy 4 bit resolution */
			
			if ((hd->wav.nBitsPerSamples < 8) || (hd->wav.nBitsPerSamples > 16)) {
				fprintf(stderr, "sfiles (STrheader): cannot treat other than 8 and 16 bit WAV files!\n");
				return -1;
			}
			
			/* set IRCAM header */
			
			sfmagic1(hd) = SF_MAGIC1;
			sfmagic2(hd) = SF_MAGIC2;
			sfparam(hd) = '\0';            /* not really used! */
			sfsrate(hd) = hd->wav.nSamplesPerSec;
			sfchans(hd) = hd->wav.nChannels;
			sfclass(hd) = hd->wav.nBitsPerSamples / 8;
			sfmachine(hd) = SF_WAV;
			/* no IRCAM custom codes to be derived from WAV headers: */
			bcopy(&STendcode, &sfcodes(hd), sizeof(SFCODE)); /* set IRCAM header code section to nil */
		}
		else {
			/* check for AIFF/AIFC type */
			STform   form;

			if (STreadForm(fd, &form) < 0) {
				fprintf(stderr, "sfiles (STrheader): Error while reading or unknown soundfile format!\n");
				return -1;
			}

			if (STreadCommonChunk(fd, &form, &hd->aiff.common) < 0)
				return -1;

			/* check for compression */

			if (*hd->aiff.common.compressionType != '\0' && strncmp(hd->aiff.common.compressionType, (const char *)STaifcNoneID, 4) != 0){
				fprintf(stderr, "sfiles (STrheader): Sorry, can't read compressed aifc soundfiles at the moment!\n");
				return -1;
			}

			/* check for sample widths */

			if (hd->aiff.common.sampWidth != 8 && hd->aiff.common.sampWidth != 16 && hd->aiff.common.sampWidth != 24 && 
					hd->aiff.common.sampWidth != 32) {
				fprintf(stderr, "sfiles (STrheader): Sorry, files has %dbit samples, can only read aiff/aifc/sonic files with 8/16/24/32bit samples!\n", hd->aiff.common.sampWidth);
				return -1;
			}

			/* set IRCAM header */

			sfmagic1(hd) = SF_MAGIC1;
			sfmagic2(hd) = SF_MAGIC2;
			sfparam(hd) = '\0';            /* not really used! */
			sfsrate(hd) = STconvExtended(hd->aiff.common.sampRate);
			sfchans(hd) = hd->aiff.common.channels;
			sfclass(hd) = hd->aiff.common.sampWidth / 8;
			/* no IRCAM custom codes to be derived from AIFX headers: */
			bcopy(&STendcode, &sfcodes(hd), sizeof(SFCODE)); /* set IRCAM header code section to nil */

			/* read sound data chunk */

			if (STreadSoundDataChunk(fd, &form, &hd->aiff.soundData) < 0)
				return -1;

			if (hd->aiff.soundData.blockSize == 262144) {
				sfmachine(hd) = SF_SONIC;
			} else if (strncmp(form.h.type, (const char *)STaiffID, 4) == 0) {
				sfmachine(hd) = SF_AIFF;
			} else
				sfmachine(hd) = SF_AIFC;

			hd->aiff.soundDataPosition = lseek(fd, 0, SEEK_CUR);

			/* SONIC offset correction */

			if (STisSONIC(hd)) {
				int
					bigblock_bytes    =  512 * 512,
					aiff_header_bytes =  hd->aiff.soundDataPosition - hd->aiff.soundData.offset,
					offset_bb_bytes   =  hd->aiff.soundData.offset - ( bigblock_bytes - aiff_header_bytes ),
					bigblock_number   =  offset_bb_bytes / 245760,
					skipped_bytes     =  ( 32 * 512 ) * bigblock_number;
					
				hd->aiff.soundDataPosition += skipped_bytes;
				lseek(fd, skipped_bytes, SEEK_CUR);

				skipped_bytes = (245760 - hd->aiff.soundDataPosition % 262144) % 3;
				if (skipped_bytes != 0) {
					hd->aiff.soundDataPosition += skipped_bytes;
					lseek(fd, skipped_bytes, SEEK_CUR);
					hd->aiff.common.samples--;
				}
			}

			hd->aiff.soundDataRead = 0;
		}

	} else 
		/* test IRCAM machine type */

		if (sfmachine(hd) != SF_NEXT && sfmachine(hd) != SF_MIPS && sfmachine(hd) != SF_SUN) {
			fprintf(stderr, "sfiles (STrheader): Sorry, can only read SGI/Next, SUN or DEC type IRCAM soundfiles!\n");
			return -1;
		}

	/* convert foreign headers */

	if (STisForeignIRCAM(hd)) /* swap byte order in header! */
		STcnvForeignHeader(hd);
	
	return 0;
}

/*
 * ------------------------------------------------
 * open sound files with intelligent path extension
 * ------------------------------------------------
 */

static int STopensf(const char *path, SFHEADER *hd, int mode)
{
	int fd;


	path = STgetsfname(path);

	if ((fd = open(path, mode)) < 0) {                                      
		fprintf(stderr, "sfiles (STopensf): Cannot access soundfile %s\n", path);      
		return -1;
	}

	if (STrheader(fd, hd) < 0)                                         
		return -1;

	return fd;
}

int STreadopensf(const char *path, SFHEADER *hd)
{
	return STopensf(path, hd, O_RDONLY);
}

int STrwopensf(const char *path, SFHEADER *hd)
{
	return STopensf(path, hd, O_RDWR);
}

int STwriteopensf(const char *path, SFHEADER *hd)
{
	int fd;


	if (strcmp(path, "-") == 0) 
		fd = fileno(stdout);
	else {
		path = STgetsfname(path);

		if ((fd = open(path, ((STisAIFX(hd) || STisWAV(hd)) ? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC, 0644)) < 0 ) {
			fprintf(stderr, "sfiles (STwriteopensf): Cannot open %s\n", path);      
			return -1;
		}
	}

	if(STwheader(fd, hd) < 0)                                         
			return -1;

	return fd;
}

/* extension of the tilde shorthand for home directory location */
static char *STtilde(char *new, const char *old)
{
	char          *p, *envptr = NULL;
	const char    *o;
	struct passwd *pw;
	static char   person[40] = {0};


	if (old[0] != '@')
		return (NULL);

	for (p = person, o = &old[1]; *o && *o != '/'; *p++ = *o++);
	*p = '\0';

	if (person[0] == '\0') {			/* then use current uid */
		envptr = getenv("HOME_SFDIR");
		if(!envptr)
	    return(NULL);
		strcpy(new, envptr);
	} else {
		pw = getpwnam(person);
		if (pw == NULL)
	    return (NULL);
		strcpy(new, DEFAULT_HOME_SFDIR);
		strcat(new, person);
	}

	strcat(new, o);

	return new;
}

/* intelligent path construction with SFDIR extension */
const char *STgetsfname(const char *shortname)
{
	char         *c, *envptr = NULL;
	static char  workspace[1025];


	if(!shortname || *shortname == '/') 
		return(shortname);

	if(*shortname == '@') 
		return(STtilde(workspace, shortname));

	if (strncmp(shortname, "./", 2) == 0) {
	  if (!(envptr = getenv("PWD")))
			return(shortname+2);
	  sprintf(workspace, "%s/%s", envptr, shortname+2);

	} else if(strncmp(shortname, "~/", 2) == 0) {
	  if (!(envptr = getenv("HOME")))
			return(shortname);
	  sprintf(workspace, "%s/%s", envptr, shortname+2);

	} else if (strncmp(shortname, "../", 3) == 0) {
	  if (!(envptr = getenv("PWD")))
			return(shortname);
		strcpy(workspace, envptr);
		if (!(c = strrchr(workspace, '/')))
			return(shortname);
		*++c = '\0';
		strcat(workspace, shortname+3);

	} else {
	  if (!(envptr = getenv("SFDIR")))
			return shortname;
	  sprintf(workspace, "%s/%s", envptr, shortname);
	}
	return(workspace);
}

/*
 * ------------------------------------------------------------
 * report length of sound (mono/stereo... ) in samples and time 
 * ------------------------------------------------------------
 */

int STsampleCount(int fd, SFHEADER *hd)
{
	if (STisIRCAM(hd)) {
		struct stat  sbuf;
		

		if (fstat(fd, &sbuf) < 0)
			return -1;
	
		return ((sbuf.st_size - sizeof(SFHEADER)) / sfclass(hd)) / sfchans(hd);

	} else if (STisWAV(hd)) {
		/* lendata does NOT give the number of (mono/stereo) samples, but their byte count */
		return (((int) hd->wav.lendata) / sfclass(hd)) / sfchans(hd);

	} else if (STisAIFX(hd)) {
		/* AIFF/AIFC/Sonic stuff */

		return hd->aiff.common.samples;
	} else {
		fprintf(stderr, "sfiles (STsampleCount): unknown sound file format!\n");
		return 0;
	}

}


float STsfseconds(int fd, SFHEADER *hd)
{
	int count = STsampleCount(fd, hd);

	return count > 0 ? (float)count / sfsrate(hd) : (float)count;
}


int STreadSampBuffer(int fd, SFHEADER *hd, void *buff, int n)
{
	int 
		count;

	/* NOTE: a priori, sfclass(hd) should be valid for all supported formats (see STrheader above)! */
	const int
		width = ((STisIRCAM(hd)) || (STisWAV(hd))) ? sfclass(hd) : hd->aiff.common.sampWidth / 8;
		
	/* if aiff/sonic truncate n to max aviable samples */

	if (STisAIFX(hd) && n > hd->aiff.common.samples * sfchans(hd) - hd->aiff.soundDataRead)
		n = hd->aiff.common.samples * sfchans(hd) - hd->aiff.soundDataRead;

	/* read samples */
	{
		int bytesRead = 0, bytes = n * width;


		if (sfmachine(hd) != SF_SONIC)
			bytesRead = STread(fd, buff, bytes);
		else {
			int 
				ret, toRead, left = STsonicBlock - lseek(fd, 0, SEEK_CUR) % STsonicBlock - STsonicSkipped;

			char
				*p = (char *)buff;


			while (bytes > 0) {
				if (left <= 0) {
					lseek(fd, STsonicSkipped + left, SEEK_CUR);
					left = STsonicUsed;
				}
				
				toRead = MIN(bytes, left);

				if ((ret = STread(fd, p, toRead)) <= 0)
					break;
				bytes -= ret;
				left -= ret;
				bytesRead += ret;
				p += ret;
			}
		}

		count = bytesRead / width;
	}

	/* swap if needed */
	/* swap bytes in case of different UNIX archs or when reading MAC formats (which is big Endian) */
	if (STisForeignIRCAM(hd)) {
		switch(sfclass(hd)) {
		case SF_SHORT:
			STswap16(buff, count);
			break;
			
		case SF_FLOAT:
			STswap32(buff, count);
			break;
		}
		
	} else if (STisAIFF(hd) || STisAIFC(hd)) {
		switch (hd->aiff.common.sampWidth) {
		case 16:
			if (STlittleEndianCPU) 
				STswap16(buff, count);
			break;

		case 24:
			if (STlittleEndianCPU) 
				STswap24(buff, count);
			break;

		case 32:
			if (STlittleEndianCPU) 
				STswap32(buff, count);
			break;
		}

		hd->aiff.soundDataRead += count;

	} else if (STisSONIC(hd)) {
		switch (hd->aiff.common.sampWidth) {
		case 16:
			if (STbigEndianCPU) 
				STswap16(buff, count);
			break;

		case 24:
			if (STbigEndianCPU) 
				STswap24((char *)buff, count);
			break;

		case 32:
			if (STbigEndianCPU) 
				STswap32(buff, count);
			break;
		}

		hd->aiff.soundDataRead += count;

	} else if (STisWAV(hd)) { /* WAV on PC is little endian! */
		switch (sfclass(hd)) {
		case SF_16bit:
			if (STbigEndianCPU) 
				STswap16(buff, count);
			break;
		}
	}


	return count;
}
	

int STwriteSampBuffer(int fd, SFHEADER *hd, void *buff, int n)
{
	int ret;

	/* swap bytes in case of different UNIX archs or when writing MAC format (which is big Endian) */
	if (STisForeignIRCAM(hd) || (STisAIFX(hd) && STlittleEndianCPU)) {
		switch(sfclass(hd)) {
		case SF_16bit:
			STswap16(buff, n);
			break;
		
		case SF_24bit:
			STswap24(buff, n);
			break;
		
		case SF_32bit:
			STswap32(buff, n);
			break;
		}
	} else if (STisWAV(hd)) { /* WAV on PC is little endian! */
		switch (sfclass(hd)) {
		case SF_16bit:
			if (STbigEndianCPU) 
				STswap16(buff, n);
			break;
		}
	}

	if ((ret = STwrite(fd, buff, n * sfclass(hd))) < 0) {
		fprintf(stderr, "sfiles (STwriteSampBuffer): Error %d while writing samples!\n", errno);
		return -1;
	}

	return ret / sfclass(hd);
}	


int STseekToSample(int fd, SFHEADER *hd, int sample)
{
	/* not lseek'able */

	if (fd == fileno(stdin) || fd == fileno(stdout) || isatty(fd)) 
		if (!STisIRCAM(hd)) {
			fprintf(stderr, "sfiles (STseekToSample): Sorry, can't seek other than IRCAM soundfiles from pipes or ttys!\n");
			return -1;
		} else {
			int  ret, toRead, bytes = sample * sfchans(hd) * sfclass(hd);
			char buff[SF_BUFSIZE];

			while (bytes > 0) {
				toRead = bytes > SF_BUFSIZE ? SF_BUFSIZE : bytes;
				if ((ret = STread(fd, buff, toRead)) <= 0) {
					fprintf(stderr, "sfiles (STseekToSample): Read error while skipping samples for seeking!\n");
					return -1;
				}
				bytes -= ret;
			}
			return 0;
		}

	/* can lseek */

	if (STisAIFX(hd)) /* only AIFX has this header field: */
		hd->aiff.soundDataRead = sample * sfchans(hd);

	return lseek(fd, STsampleToFilePosition(hd, sample), SEEK_SET) == -1 ? -1 : 0;  
}

/* intercept closing of AIFX and WAV files to update sample count information in header */
int STclose(int fd, SFHEADER *hd)
{
	int errorcode = 0;
	
	if (STisAIFF(hd) || STisAIFC(hd) || STisWAV(hd)) {
		int  val;

		if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
			fprintf(stderr, "sfiles (STclose): Fcntl error!\n");
			return -1;
		}

		if ((val & O_ACCMODE) == O_RDWR) {
			lseek(fd, 0, SEEK_END);
			if (STisAIFF(hd) || STisAIFC(hd)) {
				STupdateAIFXsampleCount(fd, hd);
			}
			else if (STisWAV(hd)) {
				STupdateWAVsampleCount(fd, hd);
			}	
		}
		/* else do nothing: there are functions that call STclose in read only mode (e.g. querysf) */  
	}

	close(fd);

	return errorcode;
}
