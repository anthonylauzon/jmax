/*

 * ErmesObjDeleteCmd.java

 *

 * Copyright (c) 1997 IRCAM.

 *

 */



package ircam.jmax.editors.ermes.tcl;



import cornell.Jacl.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.editors.ermes.*;


/**
 * The "delete" TCL command in ERMES.
 */

class ErmesObjDeleteCmd implements Command {


    public Object CmdProc(Interp interp, CmdArgs ca) {
      ErmesObject aObject = null;
      Object oneObject = null;
     
      for (int i = 1; i<ca.argc; i++) {
	aObject = ((MaxInterp) interp).getErmesObject(ca.intArg(i));
	if(aObject != null){
	  aObject.itsSketchPad.itsHelper.DeleteObject(aObject);
	}
      }
      return "";
    }
}





