/*
 * JElementsCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.editors.ermes.tcl;



import cornell.Jacl.*;

import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;	//?
import ircam.jmax.editors.ermes.*;

/**
 * The "elements" TCL command in ERMES. 
 * Returns the list of all the objects in the sketch.
 */

class ErmesElementsCmd implements Command {



    public Object CmdProc(Interp interp, CmdArgs ca) {

	Enumeration e = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsElements.elements();
	ErmesObject aObject = null;
	
	if (ca.argc != 1) {	//only the word "selected"

            throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "\"");

        }

	StringBuffer sbuf = new StringBuffer();

	while (e.hasMoreElements()) {

		aObject = (ErmesObject) e.nextElement();
		
	    String key = String.valueOf(aObject.itsFtsObject.getObjId());

	    cornell.Jacl.Util.AppendElement(sbuf, key);

	}

	return sbuf.toString();

    }
}

