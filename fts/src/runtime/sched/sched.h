#ifndef _SCHED_H_
#define _SCHED_H_

/*Scheduler  Pubblic functions */

extern fts_module_t fts_sched_module;

/* run time */

extern void fts_sched_run(void);
extern void fts_restart(void);
extern void fts_sched_halt(void);

/* declarations Scheduler */

enum declaration_verb {provide, require, freerun};
extern void fts_sched_declare(void (* fun)(), enum declaration_verb decl, fts_symbol_t state, const char *name);

extern void fts_sched_compile(void);

/* Debug functions */

extern void fts_sched_describe_order(void); 
extern void fts_sched_describe_declarations(void);

/* Tick length handling */

extern float fts_sched_get_tick_length(void);
extern void  fts_sched_set_tick_length(float t);

#endif
