/*
 * NewCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.editors.ermes.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "new" TCL command in ERMES. Opens a new, empty patch.
 */

class ErmesPatNewCmd implements Command {

    /**

     * This procedure is invoked to execute a "new patcher operation in Ermes

     */

    public Object CmdProc(Interp interp, CmdArgs ca) {

	if (ca.argc > 1) {	//no name for now

            throw new EvalException("wrong # args: should be \"" + ca.argv(0));

        }

	MaxApplication.getApplication().ObeyCommand(MaxApplication.NEW_COMMAND);

	return "new patcher";

    }

}

