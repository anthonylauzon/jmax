/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.69 $ IRCAM $Date: 1997/07/11 14:37:39 $
 *
 *  Eric Viara for Ircam, January 1995
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

extern void fts_units_init(void);

static void  fts_utils_init(void);

fts_module_t fts_utils_module = {"Utils", "Message system utilities", fts_utils_init, 0};

/* Forward declarations  */

static void
fts_utils_init(void)
{
  fts_units_init();  
}









