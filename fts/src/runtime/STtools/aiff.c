#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#ifdef sgi
#include <bstring.h>
#endif

#include "aiff.h"
#include "sfheader.h"
#include "convert.h"
#include "sfiles.h"

static int STseekToChunkType(int fd, STform *form, const STiffID *type)
/*
 * Seeks to first occurence of a specific chunk type.
 * Returns 0 on success, -1 on failure while printing error messages to stderr.
 */
{
	STchunk  *chunk = form->chunks;
	long     pos = sizeof(form->h.id) + sizeof(form->h.size) + sizeof(form->h.type);


	while (chunk->h.id[0] != '\0') {
		if (strncmp(chunk->h.id, (const char *)type, 4) == 0) {
			if (lseek(fd, pos, SEEK_SET) == -1) {
				fprintf(stderr, "Can't seek file!\n");
				return -1;
			}
			return 0;
		}

		pos += chunk->h.size + sizeof(chunk->h.id) + sizeof(chunk->h.size);
		pos += pos % 2;
		chunk++;
	}

	fprintf(stderr, "Can't find chunk type %s!\n", *type);
	lseek(fd, 0, SEEK_SET);

	return -1;
}


int STreadForm(int fd, STform *form)
{
	/* read header */

	lseek(fd, 0, SEEK_SET); 

	if (STread(fd, (char *)form->h.id, sizeof(form->h.id)) != sizeof(form->h.id)) {
		fprintf(stderr, "Can't read form header!\n");
		return -1;
	}

	if (STreadBigEndian32bit(fd, &form->h.size) != sizeof(form->h.size)) {
		fprintf(stderr, "Can't read form header!\n");
		return -1;
	}

	if (STread(fd, (char *)form->h.type, sizeof(form->h.type)) != sizeof(form->h.type)) {
		fprintf(stderr, "Can't read form header!\n");
		return -1;
	}

	/* test ids */

	if (strncmp(form->h.id, (const char *)STformID, 4) != 0) {
		fprintf(stderr, "Bad form ID!\n");
		return -1;
	}

	/* scan chunks */
	{
		const int   
			chunkStep = 10;

		int         
			chunkCount = 0, 
			chunkAlloc = chunkStep, 
			bytesToRead = form->h.size - sizeof(STiffID);

		STchunk
			*chunk = (STchunk *)calloc(chunkAlloc, sizeof(STchunk));


		if (chunk == NULL) {
			fprintf(stderr, "Can't allocate chunk space!\n");
			return -1;
		}

		form->chunks = chunk;

		/* read loop */

		while(1) {
			if (STread(fd, (char *)chunk->h.id, sizeof(chunk->h.id)) != sizeof(chunk->h.id)) {
				fprintf(stderr, "Can't read chunk header!\n");
				return -1;
			}
			
			if (STreadBigEndian32bit(fd, &chunk->h.size) != sizeof(chunk->h.size)) {
				fprintf(stderr, "Can't read chunk header!\n");
				return -1;
			}
			
			if ((bytesToRead -= sizeof(STchunkHeader) + chunk->h.size + chunk->h.size % 2) <= 0)
				break;
			
			lseek(fd, chunk->h.size + chunk->h.size % 2, SEEK_CUR);
			chunk++;

			if (++chunkCount >= chunkAlloc - 1) {
				chunkAlloc += chunkStep;
				if ((form->chunks = realloc(form->chunks, chunkAlloc * sizeof(STchunk))) == NULL) {
					fprintf(stderr, "Can't reallocate chunk space!\n");
					return -1;
				}
				chunk = form->chunks + chunkCount;
			}
		}
		chunk = NULL;
	}

	return 0;
}


void STmakeAIFCheader(const SFHEADER *hd, int sampleCount, STaifcCommonChunk *common, STsoundDataChunk *soundData)
{
	/* make common chunk */

	strncpy(common->h.id, (const char *)STcommonID, 4);
	common->h.size = sizeof(common->channels) + sizeof(common->samples) + sizeof(common->sampWidth) + sizeof(common->sampRate) 
		+ sizeof(common->compressionType) + sizeof(common->compressionName.count);

	common->channels = sfchans(hd);
	common->samples = sampleCount;
	common->sampWidth = sfclass(hd) * 8;
	STconvDoubleToExtended(sfsrate(hd), (STextended *)common->sampRate);
	strncpy(common->compressionType, (const char *)STaifcNoneID, 4);
	common->compressionName.count = 0;

	/* make sound data chunk */

	strncpy(soundData->h.id, (const char *)STsoundDataID, 4);
	soundData->h.size = sizeof(soundData->offset) + sizeof(soundData->blockSize) + sampleCount * common->sampWidth / 8 * sfchans(hd);

	soundData->offset = 0;
	soundData->blockSize = 0;
	soundData->data = NULL;
}


void STmakeAIFFheader(const SFHEADER *hd, int sampleCount, STaiffCommonChunk *common, STsoundDataChunk *soundData)
{
	/* make common chunk */

	strncpy(common->h.id, (const char *)STcommonID, 4);
	common->h.size = sizeof(common->channels) + sizeof(common->samples) + sizeof(common->sampWidth) + sizeof(common->sampRate);

	common->channels = sfchans(hd);
	common->samples = sampleCount;
	common->sampWidth = sfclass(hd) * 8;
	STconvDoubleToExtended(sfsrate(hd), (STextended *)common->sampRate);

	/* make sound data chunk */

	strncpy(soundData->h.id, (const char *)STsoundDataID, 4);
	soundData->h.size = sizeof(soundData->offset) + sizeof(soundData->blockSize) + sampleCount * common->sampWidth / 8 * sfchans(hd);

	soundData->offset = 0;
	soundData->blockSize = 0;
	soundData->data = NULL;
}


int STupdateAIFXsampleCount(int fd, SFHEADER *hd)
{
	STform              form;
	STaifcCommonChunk   common;
	STsoundDataChunk    soundData;
	int                 pos = lseek(fd, 0, SEEK_CUR);


	/* correct byte padding */

	if (pos % 2) {
		char c = 0;

		STwrite(fd, &c, 1);
	}

	/* scan form + chunks */

	if (STreadForm(fd, &form) != 0)
		return -1;

	/* make chunks */

	STmakeAIFCheader(hd, 0, &common, &soundData);

	/* correct form size */

	lseek(fd, sizeof(form.h.id), SEEK_SET);
	form.h.size = pos - sizeof(form.h.id) - sizeof(form.h.size);
	STwriteBigEndian32bit(fd, form.h.size);

	/* correct sound data chunk size */

	if (STseekToChunkType(fd, &form, STsoundDataID) != 0)
		return -1;

	lseek(fd, sizeof(soundData.h.id), SEEK_CUR);
	soundData.h.size = pos - lseek(fd, 0, SEEK_CUR) - sizeof(soundData.h.size);
	STwriteBigEndian32bit(fd, soundData.h.size);
	
	/* correct sample count */

	common.samples = (soundData.h.size - sizeof(soundData.offset) - sizeof(soundData.blockSize) - soundData.offset) / (common.sampWidth / 8) / common.channels;

	if (STseekToChunkType(fd, &form, STcommonID) != 0)
		return -1;

	lseek(fd, sizeof(common.h.id) + sizeof(common.h.size) + sizeof(common.channels), SEEK_CUR);

	STwriteBigEndian32bit(fd, common.samples);

	/* seek to end of file */

	lseek(fd, 0, SEEK_END);

	return 0;
}


int STwriteAIFCheader(int fd, const SFHEADER *hd, int sampleCount)
{
	STform              form;
	STaifcCommonChunk   common;
	STsoundDataChunk    soundData;
	int                 ret, written = 0;


	/* make chunks */
	
	STmakeAIFCheader(hd, sampleCount, &common, &soundData);

	/* make form */

	strncpy(form.h.id, (const char *)STformID, 4);
	strncpy(form.h.type, (const char *)STaifcID, 4);
	form.h.size = sizeof(form.h.type) + common.h.size + common.h.size % 2 + soundData.h.size + soundData.h.size % 2 + 2 * (sizeof(common.h.id) + sizeof(common.h.size));

	/* write form */

	lseek(fd, 0, SEEK_SET); 

	if ((ret = STwrite(fd, (char *)form.h.id, sizeof(form.h.id))) != sizeof(form.h.id)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, form.h.size)) != sizeof(form.h.size)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)form.h.type, sizeof(form.h.type))) != sizeof(form.h.type)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;

	/* write common */

	if ((ret = STwrite(fd, (char *)common.h.id, sizeof(common.h.id))) != sizeof(common.h.id)) {
		fprintf(stderr, "Can't write common header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, common.h.size)) != sizeof(common.h.size)) {
		fprintf(stderr, "Can't write common header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndianShort(fd, common.channels)) != sizeof(common.channels)) {
		fprintf(stderr, "Can't write common chunk channels!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndian32bit(fd, common.samples)) != sizeof(common.samples)) {
		fprintf(stderr, "Can't write common chunk samples!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndianShort(fd, common.sampWidth)) != sizeof(common.sampWidth)) {
		fprintf(stderr, "Can't write common chunk sampWidth!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)common.sampRate, sizeof(common.sampRate))) != sizeof(common.sampRate)) {
		fprintf(stderr, "Can't write common chunk sampRate!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)common.compressionType, sizeof(common.compressionType))) != sizeof(common.compressionType)) {
		fprintf(stderr, "Can't write common chunk compressionType!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)&common.compressionName.count, sizeof(common.compressionName.count))) != sizeof(common.compressionName.count)) {
		fprintf(stderr, "Can't write common chunk compressionName!\n");
		return -1;
	} 
	written += ret;

	/* pad bytes */

	if (written % 2) {
		char c = 0;

		if (STwrite(fd, &c, 1) != 1) {
			fprintf(stderr, "Can't write common chunk!\n");
			return -1;
		} 
		written++;
	}
		
	/* write soundData chunk */

	if ((ret = STwrite(fd, (char *)soundData.h.id, sizeof(soundData.h.id))) != sizeof(soundData.h.id)) {
		fprintf(stderr, "Can't write soundData header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, soundData.h.size)) != sizeof(soundData.h.size)) {
		fprintf(stderr, "Can't write soundData header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndian32bit(fd, soundData.offset)) != sizeof(soundData.offset)) {
		fprintf(stderr, "Can't write soundData chunk!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, soundData.blockSize)) != sizeof(soundData.blockSize)) {
		fprintf(stderr, "Can't write soundData chunk!\n");
		return -1;
	} 
	written += ret;
		
	return 0;
}


int STwriteAIFFheader(int fd, const SFHEADER *hd, int sampleCount)
{
	STform              form;
	STaiffCommonChunk   common;
	STsoundDataChunk    soundData;
	int                 ret, written = 0;


	/* make chunks */
	
	STmakeAIFFheader(hd, sampleCount, &common, &soundData);

	/* make form */

	strncpy(form.h.id, (const char *)STformID, 4);
	strncpy(form.h.type, (const char *)STaiffID, 4);
	form.h.size = sizeof(form.h.type) + common.h.size + common.h.size % 2 + soundData.h.size + soundData.h.size % 2 + 2 * (sizeof(common.h.id) + sizeof(common.h.size));

	/* write form */

	lseek(fd, 0, SEEK_SET); 

	if ((ret = STwrite(fd, (char *)form.h.id, sizeof(form.h.id))) != sizeof(form.h.id)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, form.h.size)) != sizeof(form.h.size)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)form.h.type, sizeof(form.h.type))) != sizeof(form.h.type)) {
		fprintf(stderr, "Can't write form header!\n");
		return -1;
	} 
	written += ret;

	/* write common */

	if ((ret = STwrite(fd, (char *)common.h.id, sizeof(common.h.id))) != sizeof(common.h.id)) {
		fprintf(stderr, "Can't write common header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, common.h.size)) != sizeof(common.h.size)) {
		fprintf(stderr, "Can't write common header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndianShort(fd, common.channels)) != sizeof(common.channels)) {
		fprintf(stderr, "Can't write common chunk channels!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndian32bit(fd, common.samples)) != sizeof(common.samples)) {
		fprintf(stderr, "Can't write common chunk samples!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndianShort(fd, common.sampWidth)) != sizeof(common.sampWidth)) {
		fprintf(stderr, "Can't write common chunk sampWidth!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwrite(fd, (char *)common.sampRate, sizeof(common.sampRate))) != sizeof(common.sampRate)) {
		fprintf(stderr, "Can't write common chunk sampRate!\n");
		return -1;
	} 
	written += ret;

	/* pad bytes */

	if (written % 2) {
		char c = 0;

		if (STwrite(fd, &c, 1) != 1) {
			fprintf(stderr, "Can't write common chunk!\n");
			return -1;
		} 
		written++;
	}
		
	/* write soundData chunk */

	if ((ret = STwrite(fd, (char *)soundData.h.id, sizeof(soundData.h.id))) != sizeof(soundData.h.id)) {
		fprintf(stderr, "Can't write soundData header!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, soundData.h.size)) != sizeof(soundData.h.size)) {
		fprintf(stderr, "Can't write soundData header!\n");
		return -1;
	} 
	written += ret;
		
	if ((ret = STwriteBigEndian32bit(fd, soundData.offset)) != sizeof(soundData.offset)) {
		fprintf(stderr, "Can't write soundData chunk!\n");
		return -1;
	} 
	written += ret;

	if ((ret = STwriteBigEndian32bit(fd, soundData.blockSize)) != sizeof(soundData.blockSize)) {
		fprintf(stderr, "Can't write soundData chunk!\n");
		return -1;
	} 
	written += ret;
		
	return 0;
}


static int STreadPstring(int fd, STpstring *p) 
{
  if (STread(fd, &p->count, sizeof(STbyte)) != sizeof(STbyte)) {
		fprintf(stderr, "Can't read pstring!\n");
		return -1;
	}

	if (p->count > 0 && (p->data = (char *)malloc(p->count)) == NULL) {
		fprintf(stderr, "Can't allocate space for pstring!\n");
		return -1;
	}

  if (STread(fd, p->data, p->count) != p->count) {
		fprintf(stderr, "Can't read pstring text!\n");
		return -1;
	}

	return 0;
}


int STreadCommonChunk(int fd, STform *form, STaifcCommonChunk *common)
{
	if (STseekToChunkType(fd, form, STcommonID) < 0) {
		fprintf(stderr, "Can't find common chunk!\n");
		return -1;
	}

	
	if (STread(fd, (char *)common->h.id, sizeof(common->h.id)) != sizeof(common->h.id) ||
			STreadBigEndian32bit(fd, &common->h.size) != sizeof(common->h.size) ||
			STreadBigEndianShort(fd, &common->channels) != sizeof(common->channels) ||
			STreadBigEndian32bit(fd, &common->samples) != sizeof(common->samples) ||
			STreadBigEndianShort(fd, &common->sampWidth) != sizeof(common->sampWidth) ||
			STread(fd, (char *)common->sampRate, sizeof(common->sampRate)) != sizeof(common->sampRate)) {
		fprintf(stderr, "Can't read aiff common chunk!\n");
		return -1;
	} 

	if (strncmp(form->h.type, (const char *)STaiffID, 4) == 0) {
		bcopy(STaifcNoneID, common->compressionType, 4);
		
	} else if (strncmp(form->h.type, (const char *)STaifcID, 4) == 0) {
		if (STread(fd, (char *)common->compressionType, sizeof(common->compressionType)) != sizeof(common->compressionType)) {
			fprintf(stderr, "Can't read aifc common chunk!\n");
			return -1;
		} 
		if (STreadPstring(fd, &common->compressionName) < 0)
			return -1;

	} else {
		fprintf(stderr, "Bad form type!\n");
		return -1;
	}

	return 0;
}


int STreadSoundDataChunk(int fd, STform *form, STsoundDataChunk *data)
{
	if (STseekToChunkType(fd, form, STsoundDataID) < 0) {
		fprintf(stderr, "Can't find sound data chunk!\n");
		return -1;
	}

	
	if (STread(fd, (char *)data->h.id, sizeof(data->h.id)) != sizeof(data->h.id) ||
			STreadBigEndian32bit(fd, &data->h.size) != sizeof(data->h.size) ||
			STreadBigEndian32bit(fd, &data->offset) != sizeof(data->offset) ||
			STreadBigEndian32bit(fd, &data->blockSize) != sizeof(data->blockSize)) {
		fprintf(stderr, "Can't read aiff sound data chunk!\n");
		return -1;
	} 

	data->data = NULL;

	/* seek to offset */

	if (data->offset > 0)
		lseek(fd, data->offset, SEEK_CUR);

	return 0;
}

