#ifndef _KERNEL_H_
#define _KERNEL_H_
extern void fts_kernel_init(int argc, char **argv);
extern void fts_run(const char **bootdata);
extern void fts_halt(void);
#endif
