package ircam.jmax.editors.console;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import java.io.*;
import java.util.*;
/**
  Window containing a tcl console
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

public class ConsoleWindow extends MaxEditor {
  StringBuffer itsSbuf = new StringBuffer();
  Console itsConsole;
  ConsoleDocument itsConsoleDocument = new ConsoleDocument(this);

  public ConsoleWindow(Console theConsole, String theTitle) {
    super(theTitle);
    itsConsole = theConsole;
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    getContentPane().setLayout(gridbag);
    
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(theConsole, c);
    getContentPane().add(itsConsole);
    itsConsole.SetContainer(this);
  }
  
  public boolean OpenFile(File file){
    String aExtension = GetExtension(file);
    MaxResourceId aResId = null;
    if(aExtension.equals("tpa")){
	try {
	  MaxApplication.getTclInterp().evalFile(file.getPath());
	}
	catch (Exception e){
	  System.out.println("error while opening .tpa " + file +" : " + e.toString());
	  e.printStackTrace();
	  return false;
	}
	MaxApplication.itsSketchWindowList.addElement(MaxApplication.itsSketchWindow);
	MaxApplication.AddThisWindowToMenus(MaxApplication.itsSketchWindow);
	MaxApplication.itsSketchWindow.itsDocument.SetFile(file);
	MaxApplication.itsSketchWindow.setTitle(file.getName());
	return true;
    }
    else if(aExtension.equals("pat")){
      MaxApplication.Load(file);
      return true;
    }
    else if ((aResId = ResIdWithExtension("."+aExtension))!=null) {
      Object placeHolder;
      MaxDocument aDocument;
      try {
	placeHolder = Class.forName("editors." + aResId.resourceName +"." + aResId.preferred_resource_handler).newInstance();
      }catch(ClassNotFoundException k) {
	System.out.println("editor not found:" + aResId.preferred_resource_handler); 
	return false;
      }
      catch(IllegalAccessException k) {
	System.out.println("cannot execute " + aResId.preferred_resource_handler + " illegal access - is this \"public\"?");
	return false;
      }
      catch(InstantiationException k) {
	System.out.println("cannot execute " + aResId.preferred_resource_handler + ": instantiation error");
	return false;
      }     
      if(placeHolder instanceof MaxDocument){
	if(placeHolder instanceof MaxEditor){
	  MaxEditor aEditor = (MaxEditor) placeHolder;
	  aEditor.Init(/*itsProject*/);
	}
	
	aDocument = (MaxDocument) placeHolder;
	aDocument.InitDoc(file.getName(), file/*, itsProject*/);
	MaxApplication.itsEditorsFrameList.addElement(aDocument.GetWindow());
	MaxApplication.SetCurrentWindow(aDocument.GetWindow());
	MaxApplication.AddThisFrameToMenus(aDocument.GetTitle());
      }
      return true;
    }
    //tutto il resto viene aperto con il text editor
    else {
      // ...
    }
    return true;
  }
  
  public String GetExtension(File theFile){
    String theFileName = theFile.getName();
    int aLength = theFileName.length();
    

    if (aLength > 4){
      String aExtension = theFileName.substring(aLength-3, aLength);
      if(theFileName.endsWith("."+aExtension)) return aExtension;
      else return "";
    }
    else
      return "";
  }

  MaxResourceId ResIdWithExtension(String theExtension) {
    MaxResourceId aResId = null;
    String aExtension;
    for(Enumeration e = MaxApplication.resourceVector.elements(); e.hasMoreElements();) {
      aResId = (MaxResourceId) e.nextElement();
      for (Enumeration e1=aResId.resourceExtensions.elements(); e1.hasMoreElements();) {
	aExtension = (String) e1.nextElement();
	if(aExtension.equals(theExtension))
	  return aResId;
      }
    }
    return null;//resource extension is unknown in the current system settings...
  }


  public MaxDocument GetDocument(){
    return itsConsoleDocument;
  }
  
  public void SetupMenu(){
    GetCloseMenu().setEnabled(false);
    GetSaveMenu().setEnabled(false);
    GetSaveAsMenu().setEnabled(false);
    GetPrintMenu().setEnabled(false);
    GetCutMenu().setEnabled(false);
    GetCopyMenu().setEnabled(false);
    GetPasteMenu().setEnabled(false);
    GetClearMenu().setEnabled(false);
  }

  public boolean CustomMenuActionPerformed(MenuItem theMenuItem, String theString){
    return true;
  }

  public boolean CustomMenuItemStateChanged(CheckboxMenuItem theMenuItem, String theString){
    return true;
  }
}






