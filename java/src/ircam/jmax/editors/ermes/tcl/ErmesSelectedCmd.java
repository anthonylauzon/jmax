/*
 * JSelectedCmd.java
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
 * The "selected" TCL command in ERMES.
 * Returns the list of the selected objects.
 */

class ErmesSelectedCmd implements Command 
{
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    Enumeration e;
    
    if (MaxApplication.getApplication().itsSketchWindow != null)
    e =  MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsSelectedList.elements();
    else {
     throw new EvalException("no windows!"); 
    }
    ErmesObject aObject = null;
    
    if (ca.argc != 1) 
      {	//only the word "selected"
	throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "\"");
      }
    
    StringBuffer sbuf = new StringBuffer();
    
    
    while (e.hasMoreElements()) 
      {
	aObject = (ErmesObject) e.nextElement();
	String key = String.valueOf(aObject.itsFtsObject.getObjId());
	cornell.Jacl.Util.AppendElement(sbuf, key);
      }

    return sbuf.toString();    
  }
}
