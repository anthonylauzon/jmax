package ircam.jmax.editors.project;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;

/**
 * The single entry in a project. It handles the interaction with the 
 * user (click, souble click, open a document...).
 */
public class ProjectEntry extends Panel implements MouseListener/*, KeyListener*/ {
  int itsFontStyle;	
  MaxDocument itsDocument;
  public Project itsProject;
  String  itsfName, itsrType, itsfSize;
  int itsSize = 0;
  String itsPath;
  boolean itsSelected = false;
  Vector itsEntryList;//List of sub-entry
  int itsAbstractionNumber = 0;
  boolean itsOpen = false;
		
  //--------------------------------------------------------
  //	CONSTRUCTOR
  
  //--------------------------------------------------------
  public ProjectEntry(MaxDocument theDocument, Project theProject) {
    
    itsEntryList = new Vector();
    itsDocument = theDocument;
    itsProject = theProject;
    itsPath = itsDocument.GetPath();
    itsfName = theDocument.GetName();
    itsrType = theDocument.GetType();
    itsfSize = String.valueOf(itsSize);
    int fontStyle = theDocument.GetSaveFlag() ? Font.PLAIN : Font.ITALIC;
    
    String aFontName = (Toolkit.getDefaultToolkit().getFontList())[0];
    Font newFont = new Font(aFontName, fontStyle, 10);
    setFont(newFont);

    addKeyListener(itsProject.itsProjectWindow);
    addMouseListener(this);
  }
	
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ProjectEntry(String theName, String theType, String thePath, Project theProject) {
    
    itsEntryList = new Vector();
    itsDocument = null;
    itsProject = theProject;
    itsfName = theName;
    itsrType = theType;
    itsfSize = String.valueOf(itsSize);
    itsPath = thePath;
    int fontStyle = Font.PLAIN;

    String aFontName = (Toolkit.getDefaultToolkit().getFontList())[0];
    Font newFont = new Font(aFontName, fontStyle, 10);
    setFont(newFont);

    // addKeyListener(this);
    addMouseListener(this);
  }

  public void SetFileName(String theFileName, String thePathName){
    itsfName = theFileName;
    itsPath = thePathName;
    repaint();
  }
  //--------------------------------------------------------
  //	GetWholeName
  //--------------------------------------------------------
  public String GetWholeName(){
    return itsPath+itsfName;
  }
  
  //--------------------------------------------------------
  //	GetDocument
  //--------------------------------------------------------
  public MaxDocument GetDocument() {
    return itsDocument;
  }
  
  //--------------------------------------------------------
  //	GetAbstractionNumber
  //--------------------------------------------------------
  public int GetAbstractionNumber() {
    return itsAbstractionNumber;
  }
	
  //--------------------------------------------------------
  //	GetType
  //--------------------------------------------------------
  public String GetType() {
    return itsrType;
  }
	
  //--------------------------------------------------------
  //	Select
  //--------------------------------------------------------
  public void Select(){
    itsSelected = true;
    setBackground(Color.gray);
    repaint();
  }
	
  //--------------------------------------------------------
  //	Deselect
  //--------------------------------------------------------
  public void Deselect(){
    itsSelected = false;
    setBackground(Color.lightGray);
    repaint();
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  /* public boolean mouseDown(Event evt,int x, int y) {
     if(evt.clickCount>1){
     if(itsDocument!=null) {
     if(itsDocument.GetWindow()!=null)
     itsDocument.GetWindow().ToFront();
     }
     else if(OpenEntryDocument()) return true;
     }
     else{
     if(itsDocument!=null){
     if(itsDocument.GetWindow()!=null){
     itsDocument.GetWindow().ToFront();
     MaxApplication.getApplication().SetCurrentWindow(itsDocument.GetWindow());
     }
     }
     else
     itsProject.SetCurrentEntry(this);
     }
     return true;
     }*/

  public void mousePressed(MouseEvent e){
    itsProject.itsProjectWindow.requestFocus();
    if(e.getClickCount()>1){
      if(itsDocument!=null) {
	if(itsDocument.GetWindow()!=null)
	  itsDocument.GetWindow().ToFront();
      }
      else if(OpenEntryDocument()) return;
    }
    else{
      if(itsDocument!=null){
	if(itsDocument.GetWindow()!=null){
	  itsDocument.GetWindow().ToFront();
	  MaxApplication.getApplication().SetCurrentWindow(itsDocument.GetWindow());
	}
      }
      else
	itsProject.SetCurrentEntry(this);
    }
  }
  public void mouseClicked(MouseEvent e){}
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}

  boolean OpenEntryDocument(){
    if(itsProject.itsProjectWindow.OpenFile(itsfName, itsPath)){
      itsDocument = MaxApplication.getApplication().itsWindow.GetDocument();
      if(itsDocument instanceof ErmesPatcherDoc){
	((ErmesSketchWindow)(itsDocument.GetWindow())).SetEntry(this);//?????
      }
      return true;
    }
    else return false;
  }
  
  //--------------------------------------------------------
  //	IncAbstractionNumber
  //--------------------------------------------------------
  public void IncAbstractionNumber(){
    itsAbstractionNumber++;
  }
  
  //--------------------------------------------------------
  //	DecAbstractionNumber
  //--------------------------------------------------------
  public void DecAbstractionNumber(){
    itsAbstractionNumber--;
  }
	
  //--------------------------------------------------------
  //	Paint
  //--------------------------------------------------------
  public void paint(Graphics g){
    FontMetrics fm = getFontMetrics(getFont());
    g.setColor(Color.black);
    if(!itsOpen) {
      g.drawLine(5,getSize().height/4,14,getSize().height/2);
      g.drawLine(14,getSize().height/2,5,getSize().height/4*3);
      g.drawLine(5,getSize().height/4*3,5,getSize().height/4);
    }
    else {
      g.drawLine(6,5,14,5);
      g.drawLine(14,5,10,14);
      g.drawLine(10,14,6,5);
    }
    g.drawString(itsfName, 20, getSize().height/3*2);
    g.drawString(itsrType, 40+fm.stringWidth(itsfName), getSize().height/3*2);
    g.drawString(itsfSize, 60+fm.stringWidth(itsfName)+fm.stringWidth(itsrType),getSize().height/3*2);
  }
			
  public void Reset(){
    itsDocument = null;
  }
  

  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(200, /*20*//*15*/5);
  }
	
  //--------------------------------------------------------
  //	preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return getMinimumSize();
  }
}








