#include <stdio.h>
#include <sys/file.h>
#include <strings.h>
#ifdef sgi
#include <bstring.h>
#endif
#include <stdlib.h>

#include "sfcodes.h"

/* access functions for custom IRCAM header code section */

/* setting code to nil */
const SFCODE STendcode = {SF_END, sizeof(SFCODE)};

/* read code key */
short STcode(SFCODE *sfc)
{
	short ret;

	bcopy(&sfc->code, &ret, sizeof(short));

	return ret;
}

/* read code value size */
short STbsize(SFCODE *sfc)
{
	short ret;

	bcopy(&sfc->bsize, &ret, sizeof(short));

	return ret;
}

/* write code key */ 
void STsetCode(SFCODE *sfc, short code)
{
	bcopy(&code, &sfc->code, sizeof(short));
}

/* write code value size */
void STsetBsize(SFCODE *sfc, short bsize)
{
	bcopy(&bsize, &sfc->bsize, sizeof(short));
}

/* get next code entry */
SFCODE *STnextCode(SFHEADER *hd, SFCODE *sfc)
{
	SFCODE  *next = (SFCODE *)((char *)sfc + STbsize(sfc));


	if (STcode(sfc) == SF_END)
		return NULL;

	if (STbsize(sfc) < sizeof(SFCODE)) {
		fprintf(stderr, "Error: Bad sfcode byte count!\n");
		return NULL;
	}

	if ((char *)next >= (char *)hd + sizeof(SFHEADER)) {
		fprintf(stderr, "Error: Sfcodes exceeds header size!\n");
		return NULL;
	}

	return next;
}

/* look for and return specific code information */ 
SFCODE *STgetsfcode(SFHEADER *hd, short code)
{
	SFCODE     *sfc = &sfcodes(hd);


	while(sfc) {
		if (STcode(sfc) == code)
			return sfc;

		sfc = STnextCode(hd, sfc);
	}

	return NULL;
}

/* write/update code entry */
int STputsfcode(SFHEADER *hd, char *data, SFCODE *new)
{
	SFCODE      
		*next,
		*sfc = &sfcodes(hd), 
		*old = NULL, *end = NULL;

	const char 
		*hdend = (char *)hd + sizeof(SFHEADER);


	while (sfc) {
		if (STcode(sfc) == STcode(new)) {  
		/* previous data to be replaced */
			if (old) 
				fprintf(stderr, "Warning: Multiple sfcodes of type %d (using last one).\n", STcode(sfc));
			old = sfc;
			next = STnextCode(hd, sfc);
		} 

		if (STcode(sfc) == SF_END) {  
		/* new data...put at end */
			end = sfc++;
			break;
		}

		sfc = STnextCode(hd, sfc);
	}

	if (old) {    
		long  tail = (char *)sfc - (char *)next;
		char  *save;

	/* replace old data */
		if (STbsize(old) == STbsize(new)) {
		/* if sizes match perfectly */
			bcopy((char *)new, (char *)old, sizeof(SFCODE));
			bcopy(data, (char *)old + sizeof(SFCODE), STbsize(new) - sizeof(SFCODE));
			return 0;
		}
		/* 
		 * else store the rest of the header data (sfc - next), 
		 * copy in new data, then copy back the old data
		 */
		if (((char *)old + STbsize(new) + tail) > hdend)
			return -1;

		save = (char *)malloc(tail);   
		bcopy(next, save, tail);
		bcopy((char *)new, (char *)old, sizeof(SFCODE));
		bcopy(data, (char *)old + sizeof(SFCODE), STbsize(new) - sizeof(SFCODE));
		bcopy(save, (char *)old + STbsize(new), tail);

		return 0;
	}

	if (end) {    
	/* data is new, insert at previous end location */
		if((char *)sfc + STbsize(new) > hdend)
			return -1;

		bcopy((char *)new, (char *)end, sizeof(SFCODE));
		bcopy(data, (char *)end + sizeof(SFCODE), STbsize(new) - sizeof(SFCODE));
		bcopy(&STendcode, (char *)end + STbsize(new), sizeof(SFCODE));
		return 0;
	}

	return -1;
}

/* set code entries to nil */
void STdeleteCodes(SFHEADER *hd)
{
	bcopy(&STendcode, &sfcodes(hd), sizeof(SFCODE));
}


