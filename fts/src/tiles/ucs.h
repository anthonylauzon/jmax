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
#ifndef _UCS_H_
#define _UCS_H_

/*
   FTS Universal configuration System include file 

*/

extern fts_module_t fts_ucs_module;

extern fts_status_t fts_ucs_execute_command(int argc, const fts_atom_t *argv);
extern fts_status_t fts_ucs_execute_command_opcode(fts_symbol_t opcode, int argc, const fts_atom_t *argv);

#endif
