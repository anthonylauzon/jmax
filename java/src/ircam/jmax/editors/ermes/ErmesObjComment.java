package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "comment" graphic object
 */
class ErmesObjComment extends ErmesObject {
  protected static ErmesObjTextArea itsTextArea;
  Dimension preferredSize = null;	
  //FontMetrics itsFontMetrics = null;
  String itsArgs = " ";
  Vector itsPrintedText;
  int itsNumberOfRows = 1;
  int FIELD_HEIGHT;
  final int TEXT_INSET = 10;
  //what follows is a turnaround of the problem in the FontMetrics.getMaxAdvance() function for certain fonts (proportional)
  private final static String check = new String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ");
	
  public ErmesObjComment(){
    super();
    itsPrintedText = new Vector();
  }
	
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSketchPad = theSketchPad;
    itsFont = theSketchPad.sketchFont;			
    itsFontMetrics = theSketchPad.getFontMetrics(itsFont);
    itsX = x; itsY = y;
    if (!theString.equals("")) {//we have arguments (are you creating me from a script?)
      itsArgs = theString;
      itsNumberOfRows = FormatText(itsArgs, 20);	//make a first format on 20 columns (default)
      preferredSize = new Dimension(MaxWidth() + TEXT_INSET, MaxHeight());
      super.Init(theSketchPad, x, y, theString);
      return true;
    }
    if (itsTextArea == null)	{//this is the first text object of the day
      itsTextArea = new ErmesObjTextArea(5, 20, this);
      itsTextArea.setEditable(true);
      itsTextArea.setFont(itsFont);
      itsFontMetrics = itsTextArea.getFontMetrics(itsFont);
      FIELD_HEIGHT = itsFontMetrics.getHeight();
      preferredSize = new Dimension(itsFontMetrics.stringWidth(itsTextArea.filler),FIELD_HEIGHT*5);
      itsTextArea.setBackground(Color.white);
      currentRect = new Rectangle();
      itsSketchPad.add(itsTextArea);
      itsTextArea.setVisible(false);
    }
    else {
      itsFontMetrics = itsTextArea.getFontMetrics(itsFont);
      FIELD_HEIGHT = itsFontMetrics.getHeight();
      preferredSize = new Dimension(itsFontMetrics.stringWidth(ErmesObjTextArea.filler),FIELD_HEIGHT*5);
      itsTextArea.setFont(itsFont);
      itsTextArea.setBackground(Color.white);
      itsTextArea.setText("");
      currentRect = new Rectangle();
      itsTextArea.itsOwner = this; //redirect the only editable field to point here...
    }
    
    Reshape(itsX, itsY, preferredSize.width, preferredSize.height);//?
    
    itsTextArea.setBounds(itsX, itsY+currentRect.height,currentRect.width, currentRect.height);
    itsSketchPad.validate();
    itsTextArea.setVisible(true);
    itsTextArea.requestFocus();
    return true;
  }

  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad,FtsGraphicDescription theFtsGraphic, FtsObject theFtsObject) {
    FIELD_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.getFont()).getHeight();
    preferredSize = new Dimension(theSketchPad.getFontMetrics(theSketchPad.getFont()).stringWidth(itsTextArea.filler),FIELD_HEIGHT*5);
    super.Init(theSketchPad, theFtsGraphic, theFtsObject);
    
    if (itsTextArea == null)	{
      itsTextArea = new ErmesObjTextArea(5, 20, this);
      itsTextArea.setEditable(true);
      itsTextArea.setFont(itsFont);
      itsFontMetrics = itsTextArea.getFontMetrics(itsFont);
      FIELD_HEIGHT = itsFontMetrics.getHeight();
      preferredSize = new Dimension(itsFontMetrics.stringWidth(itsTextArea.filler),FIELD_HEIGHT*5);
      itsTextArea.setBackground(Color.white);
      currentRect = new Rectangle();
      itsSketchPad.add(itsTextArea);
      itsTextArea.setVisible(false);    
    }

    // MDC:
    itsArgs = theFtsObject.getArgumentsDescription().trim();
    
    return true;
  }
	
  
  public boolean MouseUp(MouseEvent evt,int x,int y) {
    int newCols = 20;
    //int temp = itsFontMetrics.getMaxAdvance();	//debug only
    int temp = itsFontMetrics.stringWidth(check) / check.length();
    if (itsSketchPad.itsRunMode) return false;		
    if(itsDragging) {
      itsSketchPad.RemoveElementRgn(this);
      //1) compute newCols = how many columns are needed with this (x-itsX) 
      //according to the font
      //2) call RecomputeSizes(itsArgs, newCols)
      
      newCols = (x-itsX/*-2*TEXT_INSET*/)/temp;
      itsNumberOfRows = FormatText(itsArgs, newCols);
      if (y-itsY < MaxHeight()) y = itsY + MaxHeight() + FIELD_HEIGHT*2;
      Resize1(x-itsX, y-itsY);	//at least, the user will see the dimensions he choosed
      //
      //ReComputeSizes(itsArgs, newCols);
      itsSketchPad.SaveOneElementRgn(this);
      itsDragging = false;
      itsResized = true;
    }
    return false;
  }
  
  
  public int FormatText(String s, int col) {	//returns the number of rows needed
    
    int startingFrom = 0;
    int until = 0;
    int i = 0;
    
    itsPrintedText.removeAllElements();
    while(startingFrom != -1) {
      until = AddNextRow(s, startingFrom, col);	//FindNextRow creates a new workingString
      if (until != -1) {
	itsPrintedText.addElement(new String(s.substring(startingFrom, until)));
	startingFrom = until;
	i++;
      }
      else {
	itsPrintedText.addElement(new String(s.substring(startingFrom))); 
	i++;
	break;
      }
    }
    return i;
  }
  
  private int AddNextRow(String s, int startingFrom, int col) {
    int s_lenght = s.length()/* - startingFrom*/;
    String temp = new String();
    int lookAhead = 0;
    int index;
    
    if (startingFrom >= s_lenght) return -1;
    for (index = startingFrom; index < startingFrom + col && index < s_lenght; index++) {
      temp += s.charAt(index);
    }
    if(index == s_lenght) return -1;
    //set the lookAhead index (index of last+1 char to be included in this row)
    if (index == startingFrom + col) {
      //lookAhead = startingFrom + temp.substring(startingFrom, startingFrom+col).lastIndexOf(' ');
      int temp1 = MyLastIndexOfBlank(temp);
      if (temp1 != -1) lookAhead = startingFrom + temp1;
      else lookAhead = index;
    }
    else lookAhead = index;
    return lookAhead+1;
  }
  
  int MyLastIndexOfBlank(String theString) {
    for (int i = theString.length()-1; i>=0; i--) {
      if (theString.charAt(i) == ' ' || theString.charAt(i) == '\t') return i;
    }
    return -1;
  }
  
  public void ReComputeSizes(String s, int col) {
    itsNumberOfRows = FormatText(s, col);
    Resize1(col * itsFontMetrics.getMaxAdvance() + 2*TEXT_INSET, FIELD_HEIGHT*(itsNumberOfRows+1));
  }
  
  // starting of the graphic/FTS mix

  public void makeFtsObject()
  {
    itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "comment", itsArgs);    
  }

  public void redefineFtsObject()
  {
    itsFtsObject.setArgumentsDescription(itsArgs);
  }
  
  //the 'ResizeToNewFont' method worked very well, so we're using it also in the shift-double_click  
  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    ResizeToNewFont(itsFont);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    return true;
  }

  public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
    return true;	//for now, everything is allowed
  }

  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    if (itsSketchPad.itsRunMode) return true;
    else   
      if (evt.getClickCount() > 1) { //re-edit the field.. 
	itsTextArea.setText(itsArgs);
	itsTextArea.itsOwner = this;
	itsTextArea.setBounds(itsX, itsY, itsFontMetrics.stringWidth("linea campione italiana, moltiplicata diciamo 5 righe"), itsFontMetrics.getHeight()*5);
	itsTextArea.setVisible(true);
	itsTextArea.requestFocus();
      }
      else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
  
  int MaxWidth() {
    int max = 0;
    int temp;
    
    for (int i = 0; i<itsNumberOfRows; i++) {
      temp = itsFontMetrics.stringWidth((String) itsPrintedText.elementAt(i));
      if (max < temp) max = temp;
    }
    return max;
  }
  
  int MaxHeight() {
    return itsNumberOfRows * itsFontMetrics.getHeight();
  }
  
  void ResizeToNewFont(Font theFont) {
    //look for the longest row
    //resize in terms of 1) stringWidth of this row, 2) getHeight()*number of rows 
    Resize(MaxWidth() + TEXT_INSET - currentRect.width,
	   MaxHeight() - currentRect.height);
  }
  
  public void Paint_specific(Graphics g) {
    if (itsArgs.equals("")) return;	//VERY strange case: who asked to paint an empty comment?
    if (itsPrintedText.isEmpty()) {
      itsNumberOfRows = FormatText(itsArgs, 20);	//a default initialization
      ResizeToNewFont(itsFont);
    }
    if(itsSelected) {
      g.setColor(Color.gray);
      g.fill3DRect(itsX+1,itsY+1, 4, currentRect.height-2, true);
      // paint the drag rect. 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    }
    else {
      g.setColor(itsSketchPad.getBackground());
      g.fillRect(itsX+1,itsY+1, 4, currentRect.height-2);
      // unpaint the drag rect. 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    }
    
    g.setColor(Color.black);
    
    String aString;
    int i = 1;
    g.setFont(itsFont);
    
    //center the text...
    int start_textX = itsX+(currentRect.width-MaxWidth()) / 2;
    int start_textY = itsY+(currentRect.height-MaxHeight()) / 2;
    
    for (Enumeration e = itsPrintedText.elements(); e.hasMoreElements();) {
      aString = (String) e.nextElement();
      g.drawString(aString, /*TEXT_INSET +*/start_textX , start_textY + ((i++) * itsFontMetrics.getAscent()));
    }
  }
	
  public static ErmesObjTextArea GetTextArea(){
    return itsTextArea;
  }
    //--------------------------------------------------------
	// minimumSize()
    //--------------------------------------------------------
    public Dimension getMinimumSize() {
        return getPreferredSize(); //(depending on the layout manager).
    }

    //If we don't specify this, the canvas might not show up at all
    //(depending on the layout manager).
    public Dimension getPreferredSize() {
        return preferredSize;
    }

}




