/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _KERNEL_H_
#define _KERNEL_H_
extern void fts_kernel_init(int argc, char **argv);
extern void fts_run(const char **bootdata);
extern void fts_halt(void);
#endif
