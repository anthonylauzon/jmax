/* $Id$
 *
 * jMax
 * Copyright (C) 1994-2002 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 
 */

/*
 * This file's authors:  Diemo Schwarz, 20.8.2002
 */

/* 
 * $Log$
 */


#include "jmaxsdif.h"


#if JMAXSDIF_JMAX3
fts_timebase_t *timebase;
#endif
fts_symbol_t readsdif_symbol;
fts_symbol_t writesdif_symbol;
fts_symbol_t s_define;
fts_symbol_t s_open;
fts_symbol_t s_record;
fts_symbol_t s_close;


#if !JMAXSDIF_JMAX3
fts_module_t sdif_module = {
    "sdif", 
    "Objects for reading/writing in Sound Description Interchange Format", 
    sdif_config, NULL, NULL
};
#endif



/* init package */
void sdif_config(void)
{
    /* init sdif library, set callbacks to be called in case of
       errors, because we don't want fts to exit just like that.
       todo: take sdiftypes location from package variable */
    SdifGenInitCond(NULL);
    SdifSetExitFunc(jmaxsdif_exit);
    SdifSetErrorFunc(jmaxsdif_errorhandler);
    SdifSetWarningFunc(jmaxsdif_errorhandler);

    /* declare objects */
    readsdif_symbol  = fts_new_symbol("readsdif");
    writesdif_symbol = fts_new_symbol("writesdif");
    
    fts_metaclass_install(readsdif_symbol,  readsdif_instantiate, 
			  fts_first_arg_equiv /*???*/);
    fts_metaclass_install(writesdif_symbol, writesdif_instantiate, 
			  fts_first_arg_equiv /*???*/);

    /* initialise global variables/constants */
#if JMAXSDIF_JMAX3
    timebase = fts_get_timebase();
#endif
    s_define = fts_new_symbol("define");
    s_open   = fts_new_symbol("open");
    s_record = fts_new_symbol("record");
    s_close  = fts_new_symbol("close");

    post("sdif package version %s built %s\n", JMAXSDIFVERSION, BUILD_DATE);
}



/* exit function: called when the sdif lib wants to exit */
void jmaxsdif_exit(void)
{
    post("SDIF exit!\n");
    /* todo: signal error to fts??? */
}


/* handler for sdif library errors and warnings */
void jmaxsdif_errorhandler(int		    error_tag, 
			   SdifErrorLevelET error_level, 
			   char*	    error_msg, 
			   SdifFileT*	    error_file, 
			   SdifErrorT*	    error_ptr, 
			   char*	    source_file, 
			   int		    source_line)
{
    jmaxsdif_t *base   = (jmaxsdif_t *) (error_file  
					 ?  SdifFGetUserData(error_file, 0)
					 :  NULL);
    const char *levstr = error_level == eWarning  ?  "Warning"  :  "Error";


    if (base  &&  error_level != eWarning)
    {    /* stop reading */
	if (error_tag != eUnDefined)
	{   /* ignore undefined types, print message just once */
	    static int seen = 0;

	    if (!seen)
	    {
		post("SDIF %s (ignored): %s\n", levstr, error_msg);
		seen = 1;
	    }
	}
	else
	{
	    base->state = state_stopped;
	    post("SDIF %s: %sstopping %s\n", levstr, error_msg, base->objectname);
	    jmaxsdif_closefile(base);
	}
    }
    else
    {   /* debug */
	post("SDIF %s (ignored): %s\n", levstr, error_msg);
    }

    /* todo: signal error to fts??? */
}



/* return: success */
int jmaxsdif_closefile(jmaxsdif_t *base)
{
    if (base->file)
    {
	SdifFClose(base->file);
	base->file = NULL;
    }
    return 1;
}
