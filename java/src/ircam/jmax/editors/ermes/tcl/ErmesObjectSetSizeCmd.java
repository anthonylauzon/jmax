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
import ircam.jmax.fts.*;  //?
import ircam.jmax.editors.ermes.*;

/**
 * The "ObjSetSize" TCL command in ERMES.
 */

class ErmesObjectSetSizeCmd implements Command 
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {  
    ErmesSketchPad aSketchPad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad; 
    Enumeration e = aSketchPad.itsElements.elements();
    
    if (argv.length == 4)
      {
	ErmesObject aObject;

	aObject = (ErmesObject) ReflectObject.get(interp, argv[1]);

	//we have the object: set the size

	int width  = TclInteger.get(interp, argv[2]);
	int height = TclInteger.get(interp, argv[3]);
	
	aSketchPad.RemoveElementRgn(aObject);

	if(aObject.IsResizeTextCompat(width, height)) 
	  aObject.Resize(width - aObject.currentRect.width, height - aObject.currentRect.height);
	else 
	  aObject.ResizeToText(width - aObject.currentRect.width, height - aObject.currentRect.height);

	aSketchPad.SaveOneElementRgn(aObject);
    
	aSketchPad.repaint();
      }
    else
      throw new TclException(interp, "wrong # args: should be \"" + argv[0].toString() + "<id> <width> <height>\"");
  }
}







