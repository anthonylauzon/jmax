/*
 * JSetObjPos.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */
package ircam.jmax.editors.ermes.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;  //?
import ircam.jmax.editors.ermes.*;

/**
 * The "ObjGetPosX" TCL command in ERMES.
 */

class ErmesObjectGetPositionXCmd implements Command 
{

  public Object CmdProc(Interp interp, CmdArgs ca) 
  {  
    ErmesSketchPad aSketchPad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad; 
    Enumeration e = aSketchPad.itsElements.elements();
    ErmesObject aObject = null;
    
    if (ca.argc != 2) // <command> <id>
      {
	throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "<id>\"");
      }
    
    //looking for the object associated with the id
    int id = ca.intArg(1);
    
    for(; e.hasMoreElements();) 
      {
	aObject = (ErmesObject) e.nextElement();
	if (aObject.itsFtsObject.getObjId() == id) 
	  {
	    break;
	  }
      }
    
    if (aObject == null) 
      {
	throw new EvalException("no such id ("+id+")");
      }
    
    StringBuffer sbuf = new StringBuffer();
    
    String key = String.valueOf(aObject.itsX);
    cornell.Jacl.Util.AppendElement(sbuf, key);

    return sbuf.toString();    
  }
}
