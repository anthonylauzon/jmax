/*
 * JSetObjPos.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
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
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {  
    ErmesSketchPad aSketchPad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad; 
    Enumeration e = aSketchPad.itsElements.elements();
    
    if (argv.length == 4)
      {
	ErmesObject aObject;

	aObject = (ErmesObject) ReflectObject.get(interp, argv[1]);

	//we have the object: set the position

	int theDeltaX = TclInteger.get(interp, argv[2]) - aObject.itsX; 
	int theDeltaY = TclInteger.get(interp, argv[3]) - aObject.itsY;

	aSketchPad.RemoveElementRgn(aObject);
	aObject.MoveBy(theDeltaX, theDeltaY);

	Vector aVector = new Vector();
	aVector.addElement(aObject);
	aObject.itsSketchPad.itsHelper.MoveElementListConnections(aVector, theDeltaX, theDeltaY); 
	aSketchPad.SaveOneElementRgn(aObject);		
	aSketchPad.repaint();
      }
    else
      throw new TclException(interp, "wrong # args: should be \"" + new String(argv[0].toString()) + "<id> <x> <y>\"");
  }
}

