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
 * The "ObjSetSize" TCL command in ERMES.
 */

class ErmesObjectSetSizeCmd implements Command 
{
  public Object CmdProc(Interp interp, CmdArgs ca) 
  {  
    ErmesSketchPad aSketchPad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad; 
    Enumeration e = aSketchPad.itsElements.elements();
    ErmesObject aObject = null;
    
    if (ca.argc != 4) // <command> <id> <w> <h>
      {
	throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "<id> <width> <height>\"");
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
    
    //we have the object: set the size
    int width = ca.intArg(2);
    int height = ca.intArg(3);
    aSketchPad.RemoveElementRgn(aObject);

    if(aObject.IsResizeTextCompat(width, height)) 
      aObject.Resize(width - aObject.currentRect.width, height - aObject.currentRect.height);
    else 
      aObject.ResizeToText(width - aObject.currentRect.width, height - aObject.currentRect.height);

    aSketchPad.SaveOneElementRgn(aObject);
    
    aSketchPad.repaint();
    return "";
  }
}

