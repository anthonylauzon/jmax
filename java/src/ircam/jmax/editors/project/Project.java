package ircam.jmax.editors.project;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

/**
 * The project manager.
 * Currently it just handles the list of the entries. 
 * Most of the "clever" work is actually done by 
 * ProjectWindow and ProjectEntry.
 */
public class Project extends Panel implements AdjustmentListener{

  Dimension preferredSize;
  public ProjectWindow	itsProjectWindow;
  Vector itsItems = new Vector();
  ProjectEntry itsCurrentEntry;
  boolean itsAdd = false;
  String itsOldString = "";
  int itsAbstractionNum = 0;
  Vector itsPanelList = new Vector();//list of unused panels 
  
       
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public Project(ProjectWindow theProjectWindow) {    
    super();
    preferredSize = new Dimension(800, 1000);
    itsProjectWindow = theProjectWindow;
    setLayout(new GridLayout(0,1,0, 2));
    //aggiunge pannelli vuoti al project
    Panel aPanel;
    for(int i=0; i<35; i++){
      aPanel = new Panel();
      aPanel.addKeyListener(itsProjectWindow);
      aPanel.addMouseListener(itsProjectWindow);
      add(aPanel);
      itsPanelList.addElement(aPanel);
    }
    validate();
    addKeyListener(itsProjectWindow);
    addMouseListener(itsProjectWindow);
  }
	
  //--------------------------------------------------------
  //	GetItems
  //--------------------------------------------------------
  public Vector GetItems(){
    return itsItems;
  }
  

  //--------------------------------------------------------
  //	AddToProject
  //--------------------------------------------------------
  public void AddToProject(MaxDocument theDocument, MaxWindow theWindow) {
    if(!HaveAEntry(theDocument)){
      itsAdd = true;
      ProjectEntry newEntry = new ProjectEntry(theDocument, this);
      itsItems.addElement(newEntry);
      //solo per estetica
      if(itsPanelList.size()>0) {
	Panel aPanel = (Panel)itsPanelList.elementAt(0);
	itsPanelList.removeElement(aPanel);
	remove(aPanel);
      }
      //////////
      if(itsItems.size()>1)
	itsCurrentEntry.Deselect();
      itsCurrentEntry = newEntry;
      itsCurrentEntry.Select();
      add(newEntry, itsItems.size()-1-itsAbstractionNum);//-itsAbstractinoNum per mettere le abstractino tutte assieme
      itsOldString = newEntry.GetWholeName();
      if(theWindow instanceof ErmesSketchWindow) ((ErmesSketchWindow)theWindow).SetEntry(newEntry);
      validate();
      repaint();

      if(theWindow instanceof ErmesSketchWindow){
	ErmesObjPatcher aPatcher;
	for(Enumeration e = ((ErmesSketchWindow)theWindow).GetSketchPad().GetPatcherElements().elements() ; e.hasMoreElements() ;) {
	  aPatcher = (ErmesObjPatcher) e.nextElement();
	  AddToProject(aPatcher.GetName(),"Abstraction",aPatcher.GetPath());
	}
      }
    }
  }
	
  public void AddToProject(String theName, String theType, String thePath) {
    if(!HaveAEntry(theName+thePath)){
      itsAdd = true;
      ProjectEntry newEntry = new ProjectEntry(theName, theType, thePath, this);
      itsItems.addElement(newEntry);
      //solo per estetica
      if(itsPanelList.size()>0) {
	Panel aPanel = (Panel)itsPanelList.elementAt(0);
	itsPanelList.removeElement(aPanel);
	remove(aPanel);
      }
      //////////
      if(itsItems.size()>1)
	itsCurrentEntry.Deselect();
      itsCurrentEntry = newEntry;
      itsCurrentEntry.Select();
      if(theType.compareTo("Abstraction")==0) {
	add(newEntry, itsItems.size()-1);
	newEntry.IncAbstractionNumber();
	itsAbstractionNum++;
      }
      else add(newEntry, itsItems.size()-1-itsAbstractionNum);
      itsOldString = theName+thePath;
      validate();
      repaint();
    }
    else if(theType.compareTo("Abstraction")==0){
      GetTheEntry(theName+thePath).IncAbstractionNumber();
    }
  }
	
  //--------------------------------------------------------
  //	HaveAEntry
  //--------------------------------------------------------
  public boolean HaveAEntry(MaxDocument theDocument){
    ProjectEntry aEntry;
    String aName = theDocument.GetWholeName();
    for(int i=0; i<itsItems.size();i++){
      aEntry = (ProjectEntry) itsItems.elementAt(i);
      if(aName.equals(aEntry.GetWholeName())) return true;
    }
    return false;
  }
  
  public boolean HaveAEntry(String theName){
    ProjectEntry aEntry;
    for(int i=0; i<itsItems.size();i++){
      aEntry = (ProjectEntry) itsItems.elementAt(i);
      if(theName.equals(aEntry.GetWholeName())) return true;
    }
    return false;
  }

  /*only for the communication between Tabler/TextEditor and MaxApplication*/
  public void CloseThisWindow(){
    MaxApplication.getApplication().ObeyCommand(MaxApplication.CLOSE_WINDOW);
  }

  public void SetThisWindow(MaxWindow theWindow){
    MaxApplication.getApplication().SetCurrentWindow(theWindow);
  }

  //--------------------------------------------------------
  //	GetTheEntry
  //--------------------------------------------------------
  public ProjectEntry GetTheEntry(String theName){
    ProjectEntry aEntry = null;
    for(int i=0; i<itsItems.size();i++){
      aEntry = (ProjectEntry) itsItems.elementAt(i);
      if(theName.equals(aEntry.GetWholeName())) 
	break;
    }
    return aEntry;
  }
  
  //--------------------------------------------------------
  //	SelectPreviousEntry
  //--------------------------------------------------------
  public void SelectPreviousEntry(){
    int index = itsItems.indexOf(itsCurrentEntry) - 1;
    if(index>=0){
      ProjectEntry aEntry = (ProjectEntry)itsItems.elementAt(index);
      SetCurrentEntry(aEntry);
      if(itsCurrentEntry.itsDocument!=null) 
	if(itsCurrentEntry.itsDocument.GetWindow()!=null)itsCurrentEntry.itsDocument.GetWindow().ToFront();
    }
  }

  //--------------------------------------------------------
  //   SelectNextEntry
  //--------------------------------------------------------
  public void SelectNextEntry(){
    int index = itsItems.indexOf(itsCurrentEntry)+1;
    if(index<itsItems.size()){
      ProjectEntry aEntry = (ProjectEntry)itsItems.elementAt(index);
      SetCurrentEntry(aEntry);
      if(itsCurrentEntry.itsDocument!=null) 
	if(itsCurrentEntry.itsDocument.GetWindow()!=null)itsCurrentEntry.itsDocument.GetWindow().ToFront();
    }
  }
  
  //--------------------------------------------------------
  //	SetCurrentEntry
  //--------------------------------------------------------
  public void SetCurrentEntry(String theString){
    if(!itsAdd){
      if(!theString.equals(itsOldString)){//!!!
	if(itsItems.size()>1){
	  itsCurrentEntry.Deselect();
	}
	//cerca la entry corrispondente al documento
	ProjectEntry aEntry = null;
	
	for (int i=0; i<itsItems.size(); i++) {
	  aEntry = (ProjectEntry)itsItems.elementAt(i);
	  if (aEntry.GetWholeName().equals(theString))
	    break;
	}
	
	if(aEntry!=null){
	  itsCurrentEntry = aEntry;
	  itsCurrentEntry.Select();
	}
	itsOldString = theString;
      }//esce se viene chiamata per sbaglio//!!!
    }
    else itsAdd = false;
  }
	
  public void SetCurrentEntry(ProjectEntry theEntry){
    if(itsItems.size()>1){
      itsCurrentEntry.Deselect();
    }
    itsCurrentEntry = theEntry;
    itsCurrentEntry.Select();
  }
  
  public void RemoveFromFrameList(MaxWindow theWindow){
    MaxApplication.itsEditorsFrameList.removeElement(theWindow);
  }
  

  public void ResetEntry(MaxDocument theDocument) {
    ProjectEntry aEntry;
    String aString = theDocument.GetWholeName();
    for (int i=0; i<itsItems.size(); i++) {
      aEntry = (ProjectEntry) itsItems.elementAt(i);
      if(aEntry.GetWholeName().equals(aString)){
	aEntry.itsDocument = null;
	break;
      }
    }
  }	

  public void ResetMenus(MaxWindow theWindow){
    MaxApplication.getApplication().RemoveThisWindowFromMenus(theWindow);
  }
	
  //--------------------------------------------------------
  //	RemoveFromProject
  //--------------------------------------------------------
  public void RemoveFromProject(MaxDocument theDocument) {
    ProjectEntry aEntry;
    String aString = theDocument.GetWholeName();
    for (int i=0; i<itsItems.size(); i++) {
      aEntry = (ProjectEntry) itsItems.elementAt(i);
      if(aEntry.GetWholeName().equals(aString)){
	itsItems.removeElement(aEntry);			
	remove(aEntry);
	break;
      }
    }
    ////solo per estetica
    Panel aPanel = new Panel();
    add(aPanel);
    itsPanelList.addElement(aPanel);
    aPanel.addMouseListener(itsProjectWindow);
    aPanel.addKeyListener(itsProjectWindow);
    doLayout();
    validate();
    repaint();
  }
	
  public void RemoveFromProject(){
    itsItems.removeElement(itsCurrentEntry);	
    remove(itsCurrentEntry);
    if(itsCurrentEntry.GetType().equals("Abstraction")) itsAbstractionNum--;
    ////solo per estetica
    Panel aPanel = new Panel();
    add(aPanel);
    itsPanelList.addElement(aPanel);
    aPanel.addMouseListener(itsProjectWindow);
    aPanel.addKeyListener(itsProjectWindow);
    doLayout();//???
    validate();
    repaint();
  }
	
  public void RemoveFromProject(ProjectEntry theEntry){
    itsItems.removeElement(theEntry);	
    remove(theEntry);
    if(theEntry.GetType().equals("Abstraction")) itsAbstractionNum--;
    ////solo per estetica
    Panel aPanel = new Panel();
    add(aPanel);
    itsPanelList.addElement(aPanel);
    aPanel.addMouseListener(itsProjectWindow);
    aPanel.addKeyListener(itsProjectWindow);
    doLayout();//???
    validate();
    repaint();
  }
  
  public void UpdateProjectMenu(){
    MaxApplication.getApplication().UpdateProjectMenu();
  }

  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(30, 20);
  }
	
  //--------------------------------------------------------
  //	preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }

  public void adjustmentValueChanged(AdjustmentEvent e){
    return;
  }
}








