
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.lang.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.project.*;// @@@@@@
import ircam.jmax.editors.ermes.*;


/**
 * The document associated with a patcher.
 */
public class ErmesPatcherDoc implements MaxDocument {
  private static int untitledCounter = 1;
  private static int itsSubpatcherCounter = 1;
  public ErmesSketchWindow itsSketchWindow;
  public FtsObject itsPatcher;
  static final ExtensionFilter itsTpaFilter = new ExtensionFilter(".tpa");
  protected String itsFileName = "";
  protected String itsPathName = "";
  protected String itsDocumentType = "";
  public boolean alreadySaved =/*false*/true;
  protected boolean neverSaved =true;
  
  public void InitDoc(String theTitle, String thePath, String thefName, Project theProject) {
  }
  
  public ErmesPatcherDoc() {
    // create an empty ErmesPatcherDoc
    Vector args = new Vector();
    args.addElement("unnamed");
    args.addElement(new Integer(0));
    args.addElement(new Integer(0));
    itsPatcher = FtsObject.makeFtsObject(MaxApplication.getFtsServer().getRootObject(), "patcher", args);
  }
  public ErmesPatcherDoc(FtsObject theFtsPatcher) {
    // create a ErmesPatcherDoc starting from an existing FtsPatcher.
    itsFileName  = theFtsPatcher.getWindowDescription().ermesInfo;
    if(itsFileName == null || itsFileName.equals("")) itsFileName = "subpatcher "+ itsSubpatcherCounter++;
    itsPatcher = theFtsPatcher;
  }

  public ErmesPatcherDoc(ErmesSketchWindow theSketchWindow) {
    itsFileName  = "untitled" + (untitledCounter++);
    itsDocumentType = "patcher";
    itsSketchWindow = theSketchWindow;
    Vector args = new Vector();
    args.addElement("unnamed");
    args.addElement(new Integer(0));
    args.addElement(new Integer(0));
    itsPatcher = FtsObject.makeFtsObject(MaxApplication.getFtsServer().getRootObject(), "patcher", args);
  }

  public void Load(String file, String path) {
    itsFileName = file;
    itsPathName = path;
    itsDocumentType = "patcher";
    alreadySaved = true;
    Import(path+file);         //this load is used only for old .pat loading...
  }
  
  FtsObject GetFtsPatcher() {
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
  
  public String GetWholeName() {
    return itsPathName + itsFileName;
  }
	
  public String GetName() {
    return itsFileName; 
  }
	
  public String GetType() {
    return itsDocumentType; 
  }
	
  public String GetPath() {
    return itsPathName; 
  }
	
  public boolean GetSaveFlag() {
    return alreadySaved;
  }

   public boolean GetNeverSavedFlag() {
    return neverSaved;
   }
	
  public void SetFileName(String theName) {
    itsFileName = theName;
  }
  
  public void SetPathName(String theName) {
    itsPathName = theName;
  }
  
  public boolean LoadTpa(String theFile, String thePath){
    itsFileName = theFile;
    itsPathName = thePath;
    itsDocumentType = "patcher";
    alreadySaved = true;
    neverSaved = false;
    return Load(itsPathName+itsFileName);
  }

  public boolean Load(String theName){   //this is the new format (tpa) loading routine
    // itsMaxApplication.ObeyCommand(MaxApplication.NEW_COMMAND);
    try {
      MaxApplication.getTclInterp().evalFile(theName);
    } catch (tcl.lang.TclException e) {
      MaxApplication.GetPrintStream().println("error reading .tpa "+ theName + e);
      e.printStackTrace();// proviamo ...
    }
    return true;  //change this!
  }


  public boolean Import(String theWholeName) {
    String FMiller = theWholeName;
    try {
      itsPatcher = FtsDotPatParser.importPatcher(MaxApplication.getFtsServer(), new File(FMiller));
      itsPatcher.open();
    } catch (Exception e) {
      MaxApplication.GetPrintStream().println(e.toString() + " can't import "+ theWholeName);
      e.printStackTrace(); // temporary, MDC
      return false;
    }
		
    return false;
  }
  


  boolean ActualSave(OutputStream o) {
    boolean temp = true;
    try {
      temp = itsSketchWindow.itsSketchPad.SaveTo(o);
    }
    catch (IOException e) {
      MaxApplication.GetPrintStream().println("ERROR while writing " + GetWholeName());
      e.printStackTrace(); // temporary, MDC
      return false;
    }
    return true;
  }

  public void ToSave(){
    alreadySaved = false;
  }

  public boolean Save() {
    // first, tentative implementation:
    // the FILE is constructed now, and the ErmesSketchPad SaveTo method is invoked.
    // we should RECEIVE this FILE, or contruct it when we load this document
    FileOutputStream fs;
    String aOldFileName = itsFileName;
		
    if (itsFileName.equals("") || itsPathName.equals("")) {
      FileDialog fd = new FileDialog(itsSketchWindow, "FileDialog", FileDialog.SAVE);
      fd.setFilenameFilter(itsTpaFilter);
      fd.show();

      if((fd.getFile()==null)||(fd.getDirectory()==null)) return false;
      
      itsFileName = fd.getFile(); 
      itsPathName = fd.getDirectory();
    }
    if(itsFileName.equals("")) return false;
    
    try {
      fs = new FileOutputStream(GetWholeName());
    }
    catch(IOException e) {
      MaxApplication.GetPrintStream().println("ERROR while opening " + GetWholeName());
      e.printStackTrace(); // temporary, MDC
      return false;
    }

    CreateFtsGraphics(itsSketchWindow);
    PrintStream ps = new PrintStream(fs);
    itsPatcher.saveTo(fs);

    try {
      fs.close();
    } catch (IOException e) {
      MaxApplication.GetPrintStream().println("ERROR while closing " + GetWholeName());
      e.printStackTrace(); // temporary, MDC
      return false;
    }
    alreadySaved = true;
    neverSaved = false;
    
    if(!aOldFileName.equals(itsFileName)){
      MaxApplication.getApplication().ChangeWinNameMenus(aOldFileName, itsFileName);
      itsSketchWindow.setTitle(itsFileName);
      if (itsSketchWindow.itsProjectEntry != null)
	itsSketchWindow.itsProjectEntry.SetFileName(itsFileName, itsPathName);
    }
    return true;
  }
  
  void CreateFtsGraphics(ErmesSketchWindow theSketchWindow) {
    //create the graphic descriptions for the FtsObjects, before saving them
    ErmesObject aErmesObject = null;
    FtsObject aFObject = null;
    FtsGraphicDescription aGDescription = null;
    Rectangle aRect = itsSketchWindow.getBounds();
    String ermesInfo = new String();
    
    itsPatcher.setWindowDescription(new FtsWindowDescription(""+aRect.x+" "+aRect.y+" "+aRect.width+" "+aRect.height+" "+"1"+" \"(name:"+itsFileName+")\""));
    //itsPatcher.setWindowDescription(new FtsWindowDescription(aRect.x, aRect.y, aRect.width, aRect.height));
    
    for (Enumeration e=theSketchWindow.itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
      aErmesObject = (ErmesObject) e.nextElement();
      aFObject = aErmesObject.itsFtsObject;

      if (!(aErmesObject.itsFont.getName().equals(aErmesObject.itsSketchPad.sketchFont.getName()))||
	  ( aErmesObject.itsFont.getSize() != aErmesObject.itsSketchPad.sketchFont.getSize()))
	ermesInfo = "(font:"+aErmesObject.itsFont.getName()+", "+aErmesObject.itsFont.getSize()+")";

      aGDescription = new FtsGraphicDescription(""+ErmesSketchHelper.SearchErmesName(aErmesObject.getClass().getName())+" "+aErmesObject.itsX+" "+aErmesObject.itsY+" "+aErmesObject.currentRect.width+" "+aErmesObject.currentRect.height+" "+"\""+ermesInfo+"\"");

      aFObject.setGraphicDescription(aGDescription);
      if (aErmesObject instanceof ircam.jmax.editors.ermes.ErmesObjExternal && ((ErmesObjExternal)aErmesObject).itsSubWindow != null) CreateFtsGraphics(((ErmesObjExternal)aErmesObject).itsSubWindow); //recursive call
    }
  }

  public boolean SaveAs(String theWholeName) {
    SetFileName("");
    SetPathName("");
    return Save();
  }
}







