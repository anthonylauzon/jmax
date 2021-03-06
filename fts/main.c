/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsprivate/sched.h>

#ifdef WIN32

#include <windows.h>

#define is_white(_c)   (strchr(" \t\n\r", _c) != NULL)
#define is_black(_c)   (strchr(" \t\n\r", _c) == NULL)
#define is_quote(_c)   ((_c) == '"')

/**************************************
 *
 *      WinMain
 */
int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpszCmdLine, int nCmdShow) 
{
#define MAX_ARGC  64
  char* argv[MAX_ARGC];
  int argc = 0;
  char* s;
  int state = 0;  

/*    DebugBreak(); */

  argv[argc++] = "fts";

  /* Tokenize the command line and append them to the command line arguments */
  s = lpszCmdLine;

  /* Basic state machine:
       state 0 = in white, 
       state 1 = in unquoted string, 
       state 2 = in quoted string 
  */
  while (*s != 0) {
    switch (state) {
    case 0:
      if (is_quote(*s)) {
	argv[argc++] = s + 1;
	state = 2;
      } else if (is_black(*s)) {
	argv[argc++] = s;
	state = 1;	
      }
      break;

    case 1:
      if (is_quote(*s)) {
	*s = 0;
	argv[argc++] = s + 1;
	state = 2;
      } else if (is_white(*s)) {
	*s = 0;
	state = 0;	
      }
      break;

    case 2:
      if (is_quote(*s)) {
	*s = 0;
	state = 0;
      }
      break;
    }
    s++;
  }
  argv[argc] = NULL;

  {
    char buf[2048];
    _snprintf(buf, 2048, "[winmain]: Command line: ");
    for (state = 0; state < argc; state++) {
      _snprintf(buf, 2048, "%s %s", buf, argv[state]);
    }
    _snprintf(buf, 2048, "%s\n", buf);
    fts_log(buf);
  }

  /* Initialize FTS */
  fts_init( argc, argv);

  fts_log("[winmain]: Starting fts\n");

  /* Run the scheduler */
  fts_sched_run();

  fts_log("[winmain]: Stopping fts\n");

  /* When and if the scheduler exit, run the shutdown functions and return */
  fts_shutdown();

  fts_log("[winmain]: Exiting\n");

  ExitProcess(0);
  return 0;
}

#else

/**************************************
 *
 *      main
 */
int
main( int argc, char **argv)
{
  fts_post( "FTS version %s\n", fts_get_version());
  /*  fts_post( "Copyright (C) 1994, 1995, 1998, 1999, 2002 by IRCAM - Centre Pompidou, Paris, France.\n");
  fts_post( "jMax comes with ABSOLUTELY NO WARRANTY.\n");
  fts_post( "This is free software, and you are welcome to redistribute it\nunder the conditions of the GNU GENERAL PUBLIC LICENSE.\nSee accompanying LICENSE for further informations\n");*/

  fts_init( argc, argv);

  fts_log("[main]: Run scheduler\n");

  /* run scheduler */
  fts_sched_run();

  fts_log("[main]: Shutdown FTS\n");

  /* when and if the scheduler exit, run the shutdown functions and return */
  fts_shutdown();

  return 0;
}
#endif
