/*
 * MaxWhenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import ircam.jmax.*;
import cornell.Jacl.*;
import java.util.*;


/**
 * The "when" TCL command in ERMES.
 */

class MaxWhenCmd implements Command {

  public Object CmdProc(Interp interp, CmdArgs ca) {

    if (ca.argc != 3)
      throw new EvalException("usage: when name { body } ");

    MaxApplication.getApplication().addHook(ca.argv(1), ca.argv(2));

    return ca.argv(1);
  }
}

