#ifndef _UCS_H_
#define _UCS_H_

/*
   FTS Universal configuration System include file 

*/

extern fts_module_t fts_ucs_module;

/* In FTS 2.0, the use of these functions outside 
   the ucs.c file is formally discouraged; ucs will
   be phased out !!!

 */
extern void fts_ucs_define_command(fts_symbol_t opcode, fts_symbol_t sub_opcode,
				   fts_status_t (* fun)(int argc, const fts_atom_t *argv),
				   const char *usage_string,
				   const char *help_string);

extern fts_status_t fts_ucs_execute_command(int argc, const fts_atom_t *argv);

extern fts_status_t fts_ucs_execute_command_opcode(fts_symbol_t opcode, int argc, const fts_atom_t *argv);

#endif
