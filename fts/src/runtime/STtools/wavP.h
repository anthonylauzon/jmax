#ifndef _ST_WAV_PH_
#define _ST_WAV_PH_

#include "wav.h"

/* writes a WAV header with transparent byte (re)ordering */
extern int STwriteWAVheader (int, const SFHEADER *hd, int sampleCount);

/* rewrites a WAV header to update sample count */
extern int STupdateWAVsampleCount(int fd, SFHEADER *hd);


#endif /* _ST_WAV_PH_ */

