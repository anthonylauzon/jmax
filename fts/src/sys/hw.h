#ifndef _HW_H_
#define _HW_H_

/* generic include with the prototypes of HW dependent functions. */

extern void fts_platform_init(void);
extern void fts_pause(void);
extern unsigned int fts_check_fpe(void);

/* bit masks to be used with fts_check_fpe */

#define FTS_INVALID_FPE  0x01
#define FTS_DIVIDE0_FPE  0x02
#define FTS_OVERFLOW_FPE 0x04

#endif
