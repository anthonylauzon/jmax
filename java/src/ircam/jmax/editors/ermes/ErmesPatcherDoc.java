
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.lang.*;
import java.io.*;
import java.util.*;
import java.text.*; // tmp

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.project.*;// @@@@@@
import ircam.jmax.editors.ermes.*;
import ircam.jmax.mda.*; // temporary

/**
 * The document associated with a patcher.
 */
public class ErmesPatcherDoc  {
  private static int untitledCounter = 1;
  private static int itsSubpatcherCounter = 1;
  public ErmesSketchWindow itsSketchWindow;
  public FtsContainerObject itsPatcher;

  protected String itsTitle = ""; // !!! should be handled in the editor

  protected String itsDocumentType = "";
  
  public ErmesPatcherDoc() {
    // create an empty ErmesPatcherDoc

    itsPatcher = new FtsPatcherObject(FtsServer.getServer().getRootObject());
  }

  public ErmesPatcherDoc(FtsContainerObject theFtsPatcher) {
    // create a ErmesPatcherDoc starting from an existing FtsPatcher.

    if (((FtsObject)theFtsPatcher).getClassName().equals("patcher"))
      {
	itsTitle = theFtsPatcher.getObjectName();
      }
    else
      itsTitle  =  ((FtsObject) theFtsPatcher).getClassName();

    if (itsTitle == null || itsTitle.equals(""))
      itsTitle = "subpatcher "+ itsSubpatcherCounter++;

    itsPatcher = theFtsPatcher;
  }

  public ErmesPatcherDoc(ErmesSketchWindow theSketchWindow) {
    itsTitle  = getNewUntitledName();
    itsDocumentType = "patcher";
    itsSketchWindow = theSketchWindow;
    itsPatcher = new FtsPatcherObject(FtsServer.getServer().getRootObject());
  }

  public static String getNewUntitledName() {
    return "untitled"+(untitledCounter++);
  }

  FtsContainerObject GetFtsPatcher() {
    return itsPatcher;
  }
  public void DelWindow() {
    itsSketchWindow = null;
  }
  
  public void SetWindow(ErmesSketchWindow theSketchWindow) {
    itsSketchWindow = theSketchWindow;
  }
	
  public void CreateFtsGraphics(ErmesSketchWindow theSketchWindow)
  {
    //create the graphic descriptions for the FtsObjects, before saving them
    ErmesObject aErmesObject = null;
    FtsObject aFObject = null;
    Rectangle aRect = itsSketchWindow.getBounds();
    String ermesInfo = new String();
    
    itsPatcher.put("wx", aRect.x);
    itsPatcher.put("wy", aRect.y);
    itsPatcher.put("ww", aRect.width);
    itsPatcher.put("wh", aRect.height);

    for (Enumeration e=theSketchWindow.itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
      aErmesObject = (ErmesObject) e.nextElement();
      aFObject = aErmesObject.itsFtsObject;
           
      // Set geometrical properties
      
      aFObject.put("x", aErmesObject.itsX);
      aFObject.put("y", aErmesObject.itsY);
      aFObject.put("w", aErmesObject.currentRect.width);
      aFObject.put("h", aErmesObject.currentRect.height);

      // Set the font properties

      if (! aErmesObject.itsFont.getName().equals(aErmesObject.itsSketchPad.sketchFont.getName()))
	aFObject.put("font", aErmesObject.itsFont.getName());

      if (aErmesObject.itsFont.getSize() != aErmesObject.itsSketchPad.sketchFont.getSize())
	aFObject.put("fs", aErmesObject.itsFont.getSize());

      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjExternal &&
	  ((ErmesObjExternal)aErmesObject).itsSubWindow != null)
	CreateFtsGraphics(((ErmesObjExternal)aErmesObject).itsSubWindow); //recursive call
    }
  }
}







