/*

 * MaxRunHooksCmd.java

 *

 * Copyright (c) 1997 IRCAM.

 *

 */



package ircam.jmax.tcl;


import java.util.*;

import cornell.Jacl.*;
import ircam.jmax.*;


/**
 * The "when" TCL command in ERMES.
 */

class MaxRunHooksCmd implements Command {

  public Object CmdProc(Interp interp, CmdArgs ca) {

    if (ca.argc != 2)
      throw new EvalException("usage: runHooks name");

    MaxApplication.getApplication().runHooks(ca.argv(1));

    return ca.argv(1);
  }
}

