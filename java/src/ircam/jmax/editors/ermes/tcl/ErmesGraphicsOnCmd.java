/*

 * .java

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
 * The "graphicson" TCL commands. Reset the normal state of the graphic 
 * after a "graphicoff" TCL command.
 * Use with care.
 */
class ErmesGraphicsOnCmd implements Command {



    /**

     * This procedure is invoked to execute a "new patcher operation in Ermes

     */

    public Object CmdProc(Interp interp, CmdArgs ca) {

	if (ca.argc > 1) {	//no name for now

            throw new EvalException("wrong # args: should be \"" + ca.argv(0));

        }

	MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsGraphicsOn = true;
	MaxApplication.getApplication().itsSketchWindow.itsSketchPad.repaint();

	return "";

    }

}

