
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
public class ErmesPatcherDoc implements MaxDocument {
  private static int untitledCounter = 1;
  private static int itsSubpatcherCounter = 1;
  public ErmesSketchWindow itsSketchWindow;
  public FtsContainerObject itsPatcher;
  static final ExtensionFilter itsTpaFilter = new ExtensionFilter(".tpa");

  protected String itsTitle = ""; // !!! should be handled in the editor

  protected File itsFile = null;
  protected String itsDocumentType = "";
  public boolean alreadySaved =/*false*/true;
  protected boolean neverSaved =true;
  
  public void InitDoc(String theTitle, File theFile, Project theProject) {
  }
  
  public ErmesPatcherDoc() {
    // create an empty ErmesPatcherDoc

    itsPatcher = new FtsPatcherObject(MaxApplication.getFtsServer().getRootObject(), "unnamed", 0, 0);
  }

  public ErmesPatcherDoc(FtsContainerObject theFtsPatcher) {
    // create a ErmesPatcherDoc starting from an existing FtsPatcher.

    if (itsFile != null)
      {
	itsTitle = itsFile.getName();
      }
    else if (((FtsObject)theFtsPatcher).getClassName().equals("patcher"))
      {
	itsTitle = theFtsPatcher.getName();
      }
    else
      itsTitle  =  ((FtsObject) theFtsPatcher).getClassName();

    if (itsTitle == null || itsTitle.equals(""))
      itsTitle = "subpatcher "+ itsSubpatcherCounter++;

    itsPatcher = theFtsPatcher;
  }

  public ErmesPatcherDoc(ErmesSketchWindow theSketchWindow) {
    itsTitle  = "untitled" + (untitledCounter++);
    itsDocumentType = "patcher";
    itsSketchWindow = theSketchWindow;
    itsPatcher = new FtsPatcherObject(MaxApplication.getFtsServer().getRootObject(), "unnamed", 0, 0);
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
	
  public void finalize() {
    //itsPatcher.delete();
    //super.finalize();
  }
  
  public MaxWindow GetWindow(){
    return itsSketchWindow;
  }
   
  public String GetTitle() {
    return itsTitle;
  }

  public File GetFile() {
    return itsFile;
  }
	
  public String GetType() {
    return itsDocumentType; 
  }
	
	
  public boolean GetSaveFlag() {
    return alreadySaved;
  }

  public boolean GetNeverSavedFlag() {
    return neverSaved;
  }

  public void SetFile(File theFile) {
    itsFile = theFile;

    if (itsFile != null)
      {
	itsTitle = itsFile.getName();
	
	if (itsSketchWindow != null)
	  itsSketchWindow.setTitle(itsFile.getName());
      }
  }
  
  
  public boolean Load(File theFile)
  {
    //this is the new format (tpa) loading routine
    // itsMaxApplication.ObeyCommand(MaxApplication.NEW_COMMAND);

    try {
      MaxApplication.getTclInterp().evalFile(itsFile.getPath());
    } catch (tcl.lang.TclException e) {
      System.out.println("TCL error reading file "+ theFile + " : " + MaxApplication.getTclInterp().getResult());
    }
    return true;  //change this!
  }


  public void Import(File file) {
    itsTitle = file.getName();
    itsDocumentType = "patcher";

    try {
      itsPatcher = (FtsContainerObject) FtsDotPatParser.importPatcher(MaxApplication.getFtsServer(), file);
      itsPatcher.open();
    } catch (Exception e) {
      System.out.println(e.toString() + " can't import "+ file);
      e.printStackTrace(); // temporary, MDC
      return;
    }

    // Reset the file, to avoid writing on top of .pat or other .tpa files

    itsFile = null;

    // This file is not already saved, beacause it do not correspond to a .tpa file !!

    ToSave();
  }
  


  public void ToSave(){
    alreadySaved = false;
  }

  public boolean Save() {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
    FileOutputStream fs;
    String oldTitle = itsTitle;
		
    if (itsFile == null)
      itsFile = MaxFileChooser.chooseFileToSave(itsSketchWindow, "Save Patcher");

    if (itsFile == null)
      return false;
    
    itsTitle = itsFile.getName();

    CreateFtsGraphics(itsSketchWindow);

    // This code is temporary, just to test the MDA
    // save architecture; real code will substitute
    // the whole thing.

    try
      {
	FtsPatchData data = new FtsPatchData();
	data.setContent(itsPatcher);
	data.setDataSource(MaxDataSource.makeDataSource(itsFile));
	data.setInfo("Saved " + DateFormat.getDateInstance(DateFormat.FULL).format(new Date()));
	data.setName(itsFile.getName());
	data.save();
      }
    catch (MaxDataException e)
      {
	System.out.println("ERROR " + e + " while saving " + itsFile);
	e.printStackTrace(); // temporary, MDC
	return false;
      }

    alreadySaved = true;
    neverSaved = false;
    
    if(!oldTitle.equals(itsTitle)){
      MaxApplication.ChangeWinNameMenus(oldTitle, itsTitle);
      itsSketchWindow.setTitle(itsTitle);
      if (itsSketchWindow.itsProjectEntry != null)
	itsSketchWindow.itsProjectEntry.SetFile(itsFile);
    }
    return true;
  }
  
  public void CreateFtsGraphics(ErmesSketchWindow theSketchWindow)
  {
    //create the graphic descriptions for the FtsObjects, before saving them
    ErmesObject aErmesObject = null;
    FtsObject aFObject = null;
    FtsGraphicDescription aGDescription = null;
    Rectangle aRect = itsSketchWindow.getBounds();
    String ermesInfo = new String();
    
    itsPatcher.setWindowDescription(new FtsWindowDescription(aRect.x, aRect.y, aRect.width, aRect.height));

    //itsPatcher.setWindowDescription(new FtsWindowDescription(aRect.x, aRect.y, aRect.width, aRect.height));
    
    for (Enumeration e=theSketchWindow.itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
      aErmesObject = (ErmesObject) e.nextElement();
      aFObject = aErmesObject.itsFtsObject;
      
           
      // if (!(aErmesObject.itsFont.getName().equals(aErmesObject.itsSketchPad.sketchFont.getName()))||
      // ( aErmesObject.itsFont.getSize() != aErmesObject.itsSketchPad.sketchFont.getSize()))
      // ermesInfo = "(font:"+aErmesObject.itsFont.getName()+", "+aErmesObject.itsFont.getSize()+")";

      // Note that ermesInfo is not used anymore; a new function will come soon !! MDC
      
      
      aGDescription = new FtsGraphicDescription(aErmesObject.itsX, aErmesObject.itsY,
						aErmesObject.currentRect.width,
						aErmesObject.currentRect.height);

      aFObject.setGraphicDescription(aGDescription);

      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjExternal &&
	  ((ErmesObjExternal)aErmesObject).itsSubWindow != null)
	CreateFtsGraphics(((ErmesObjExternal)aErmesObject).itsSubWindow); //recursive call
    }
  }
}







