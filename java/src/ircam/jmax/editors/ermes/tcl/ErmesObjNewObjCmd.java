/*
 * NewObjCmd.java
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
 * The "newobj" TCL command in ERMES. Create a new object on the sketch, 
 * given its name, its position, its arguments.
 */

class ErmesObjNewObjCmd implements Command {



    /**

     * This procedure is invoked to execute a "new object" operation in Ermes

     */

    public Object CmdProc(Interp interp, CmdArgs ca) {
	ErmesObject aObject = null;
	if (ca.argc < 4) {	//at least newobj, name, x, y

            throw new EvalException("wrong # args: should be \"" + ca.argv(0) +
            						" <name> <x> <y>");

        }
	if (ca.argc == 4) //all the modules except the externals and messages
	aObject  = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.AddObjectByName(
										ca.argv(1), ca.intArg(2), ca.intArg(3), "");
	else if (ca.argc == 5) // externals and messages
	aObject = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.AddObjectByName(
										ca.argv(1), ca.intArg(2), ca.intArg(3), ca.argv(4));

	if (aObject != null) return String.valueOf(aObject.itsFtsObject.getObjId());
	else return "Object creation error";

    }
}

