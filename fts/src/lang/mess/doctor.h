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
#ifndef _DOCTOR_H_
#define _DOCTOR_H_


extern void fts_register_object_doctor(fts_symbol_t class_name,
				       fts_object_t *(* fun)(fts_patcher_t *patcher,
							     int ac,
							     const fts_atom_t *at));

#endif
