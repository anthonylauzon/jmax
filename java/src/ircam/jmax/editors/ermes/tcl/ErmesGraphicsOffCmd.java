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
 * The "graphicsoff" TCL commands. Every graphic operation will
 * have no effect until the "graphicson" TCL command.
 * Use with care.
 */
class ErmesGraphicsOffCmd implements Command {


    /**

     * This procedure is invoked to execute a "new patcher operation in Ermes

     */

    public Object CmdProc(Interp interp, CmdArgs ca) {

	if (ca.argc > 1) {	//no name for now

            throw new EvalException("wrong # args: should be \"" + ca.argv(0));

        }

	MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsGraphicsOn = false;

	return "";

    }

}

