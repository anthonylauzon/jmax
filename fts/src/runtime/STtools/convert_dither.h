#ifndef _ST_CONVERTDITHER_H_
#define _ST_CONVERTDITHER_H_


extern int STconvertSampleBufferWithDither(SFHEADER *from_hd, SFHEADER *to_hd, void *buff, int n);
/* 
 * Converts n channel independent samples according to specified formatsr.
 * Adds dither, where appropriate. 
 */

#endif /* _ST_CONVERTDITHER_H_ */
