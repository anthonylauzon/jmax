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
import ircam.jmax.fts.*;	//?
import ircam.jmax.editors.ermes.*;

/**
 * The "ObjSetPos" TCL command in ERMES.
 */

class ErmesObjectSetPositionCmd implements Command 
{
  public Object CmdProc(Interp interp, CmdArgs ca) 
  {
    ErmesSketchPad aSketchPad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad; 
    Enumeration e = aSketchPad.itsElements.elements();
    ErmesObject aObject = null;
    
    if (ca.argc != 4) // <command> <id> <x> <y>      
      {
	throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "<id> <x> <y>\"");
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
    
    //we have the object: set the position
    //we have the object: set the position
    int theDeltaX = ca.intArg(2)-aObject.itsX; 
    int theDeltaY = ca.intArg(3)-aObject.itsY;
    aSketchPad.RemoveElementRgn(aObject);
    aObject.MoveBy(theDeltaX, theDeltaY);
    Vector aVector = new Vector();
    aVector.addElement(aObject);
    aObject.itsSketchPad.itsHelper.MoveElementListConnections(aVector, theDeltaX, theDeltaY); 
    aSketchPad.SaveOneElementRgn(aObject);		
    aSketchPad.repaint();
    return "";
  }
}
