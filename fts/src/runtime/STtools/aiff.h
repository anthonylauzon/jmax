#ifndef _ST_AIFF_H_
#define _ST_AIFF_H_

/*
 * basic iff types
 */

typedef char STiffID[4];

typedef struct {
	char c0, c1, c2;
} ST24bit;

typedef char   ST8bit;
typedef char   STbyte;
typedef short  ST16bit;
typedef int    ST32bit;


typedef char STextended[10];


typedef struct {
	STbyte   count;
	char     *data;
} STpstring;

typedef struct {
	STiffID  id;
	ST32bit  size;
} STchunkHeader;

typedef struct {
	STchunkHeader  h;
	char           *data;
} STchunk;

typedef struct {
	STiffID  id;
	ST32bit  size;
	STiffID  type;
} STformHeader;

typedef struct {
	STformHeader h;
	STchunk      *chunks;
} STform;

#define STformID  ((const STiffID *)"FORM")


/*
 * aiff & aifc specific types 
 */

#define STaiffID  ((const STiffID *)"AIFF")
#define STaifcID  ((const STiffID *)"AIFC")

typedef struct
{
	STchunkHeader    h;
	short            channels;
	ST32bit          samples;
	short            sampWidth;
	STextended       sampRate;
} STaiffCommonChunk;

#define STcommonID   ((const STiffID *)"COMM")

typedef struct
{
	STchunkHeader    h;
	short            channels;
	ST32bit          samples;
	short            sampWidth;
	STextended       sampRate;
	STiffID          compressionType;
	STpstring        compressionName;
} STaifcCommonChunk;

#define STaifcNoneID   ((const STiffID *)"NONE")

typedef struct
{
	STchunkHeader     h;
	ST32bit           offset;
	ST32bit           blockSize;
	char              *data;
} STsoundDataChunk;

#define STsoundDataID  ((const STiffID *)"SSND")


#endif /* _ST_AIFF_H_ */
