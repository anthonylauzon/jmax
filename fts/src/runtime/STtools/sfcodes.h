#ifndef _ST_SFCODES_H_
#define _ST_SFCODES_H_

#include "sfheader.h"

extern const SFCODE STendcode;


extern short STcode(SFCODE *sfc);
extern short STbsize(SFCODE *sfc);
extern void STsetCode(SFCODE *sfc, short code);
extern void STsetBsize(SFCODE *sfc, short bsize);
/*
 * Alignment-save access to SFCODE fields.
 */

extern SFCODE *STnextCode(SFHEADER *hd, SFCODE *sfc);
/*
 * Returns the next sfcode field.
 * No proof for SF_END!
 */

extern void STdeleteCodes(SFHEADER *hd);
/*
 * Removes all codes in header.
 */

extern SFCODE *STgetsfcode(SFHEADER *hd, short code);
/*
 * STgetsfcode() searches a header for a specified
 * code.  If the code is found, getsfcode returns
 * a pointer to the beginning of the SFCODE structure not
 * to the information structure itself. 
 * The srate, number of channels, magicnumber, 
 * number of bytes per channel are NOT coded via these routines. 
 * Returns NULL on failure.
 */

extern int STputsfcode(SFHEADER *hd, char *data, SFCODE *new);
/*
 * STputsfcode() inserts new data into a soundfile header.
 * If such data (data having the same sfcode) already exists,
 * putsfcode replaces the old data with the new. 
 * (size of the old and of the new data can be different). 
 * If the data is new to the header, it is added
 * at the end of the previous sfcodes.
 * Returns 0 on success, -1 on failure.
 */


#endif /* _ST_SFCODES_H_ */
