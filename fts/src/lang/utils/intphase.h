#ifndef _INTPHASE_H_
#define _INTPHASE_H_

#define FTS_INTPHASE_BITS (24)
#define FTS_INTPHASE_RANGE (1 << FTS_INTPHASE_BITS)

/* this type should have at least (!) FTS_INTPHASE_BITS bits and be efficient to be calculated */
typedef unsigned long fts_intphase_t;

#endif
