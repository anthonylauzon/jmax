package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

/**
 * The text area contained in a "comment" object (ErmesObjComment)
 */
class ErmesObjTextArea extends TextArea implements KeyListener, FocusListener{
  static String filler = " ";
  ErmesObjComment itsOwner = null;
  ErmesSketchPad itsSketchPad = null;
  boolean laidOut = false;
  boolean focused = false;
  int DEFAULT_COLS = 20;
  
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjTextArea(ErmesSketchPad theSketchPad) {
    super(" ", 5, 20, TextArea.SCROLLBARS_NONE);
    setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME,Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));
    setEditable(true);
    selectAll();
    itsSketchPad = theSketchPad;
    focused = true;
    
    addKeyListener(this);
    addFocusListener(this);
  }

  //--------------------------------------------------------
  // lostFocus
  //--------------------------------------------------------
  public boolean LostFocus() {
    if (!focused) return true;
    else {
      focused = false;
      itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
      itsSketchPad.itsSketchWindow.requestFocus();
    }
    //try to test if this lost foscus happens in the following conditions:
    // an object was put on the sketchpad without writing into it
    // the TEXT into an object was deleted (this would require a delete of the object... see next comment)
    String aTextString = getText();
    if (aTextString.compareTo("") == 0 || aTextString.compareTo(" ") == 0) {
      setVisible(false);
      setLocation(-200,-200);
      
      if (itsSketchPad != null) itsOwner.Paint(itsSketchPad.GetOffGraphics());
      itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());//end bug 12
      itsOwner = null;	//seems to be crazy but...
      
      return true; //(immediately)
    }
    //try to test if the object was already instantiated; in case, delete the object... (how?)
    
    //qui si prova a togliere gli spazi in fondo dalla parola....
    if(aTextString.endsWith(" ")){
      while(aTextString.endsWith(" ")){
	aTextString = aTextString.substring(0, aTextString.length()-1);
      }
    }
    if(aTextString.endsWith("\n")){
      while(aTextString.endsWith("\n")){
	aTextString = aTextString.substring(0, aTextString.length()-1);
      }
    }
    
    if (itsOwner == null) return false; //this happens when the instatiation fails
    
    if (itsOwner.itsFtsObject != null){
      itsOwner.itsArgs = aTextString;
      itsOwner.ParseText(aTextString);
      itsOwner.redefineFtsObject();
    }
    else {
      itsOwner.itsArgs = aTextString;
      itsOwner.ParseText(aTextString);
      itsOwner.makeFtsObject();
    }
    ///qui accorcia se il testo e' piu' corto dell'oggetto
    //dovrebbe farlo anche per l'altezza...

    //int lenght = getFontMetrics(getFont()).stringWidth(aTextString);
    int lenght = getFontMetrics(getFont()).stringWidth(itsOwner.itsMaxString);
  
    if((lenght< getSize().width-20)&&(!itsOwner.itsResized)){
      Dimension d1 = itsOwner.Size();
      d1.width = lenght+10;
      itsOwner.setSize(d1.width, d1.height);
    }
    int height = getFontMetrics(getFont()).getHeight()*itsOwner.itsParsedTextVector.size();
    if((height< getSize().height-10)&&(!itsOwner.itsResized)){
      Dimension d1 = itsOwner.Size();
      d1.height = height;
      itsOwner.setSize(d1.width, d1.height);
    }
        
    setVisible(false);
    setLocation(-200,-200);
    focused = false;
    itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
    
    if(itsSketchPad != null) itsOwner.Paint(itsSketchPad.GetOffGraphics());
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());//end bug 12
    itsOwner = null;

    setRows(5);
    setColumns(20);
    return true;       
  }
	
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_COMMENT;
    if (focused) return;
    else focused = true;
  }

  public void focusLost(FocusEvent e){}
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine

  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  public void keyPressed(KeyEvent e){
    int lenght;
    String s1, s2;
    int start = getSelectionStart();
    int end = getSelectionEnd();
    String s = getText();
    FontMetrics fm = getFontMetrics(getFont());
    int aWidth = 0;
    int aCurrentLineWidth = 0;
    int aCurrentLineChars = 0;
    
    if (isEditable()) {
      if(e.getKeyCode()==ircam.jmax.utils.Platform.ENTER_KEY||e.getKeyCode()==ircam.jmax.utils.Platform.RETURN_KEY){//return
	itsOwner.itsTextRowNumber++;
	if(itsOwner.itsTextRowNumber>4){
	  setRows(getRows()+1);
	  Dimension d2 = itsOwner.Size();
	  itsOwner.setSize(d2.width, d2.height+fm.getHeight());
	  setSize(getSize().width, getSize().height + fm.getHeight());
	  requestFocus();
	}
	return;
      }
      else{//scrittura
	aCurrentLineChars = GetCurrentLineChars(s);//s.length() - itsOldLineChars;
	if(aCurrentLineChars+10 > getColumns())
	  setColumns(getColumns()+20);

	char k = e.getKeyChar();
	if(start!=end){//cancella selezione
	  if(!e.isShiftDown()){
	    s1 = s.substring(0, start);
	    s2 = s.substring(end, s.length());
	    s = s1+s2;
	  }
	}
	if(start < s.length()){//inserisce testo intermedio
	  s1 = s.substring(0, start);
	  s2 = s.substring(start, s.length());
	  s = s1+k+s2;
	}
	else//inserisce testo in coda
	  s = s+k;
	
	aCurrentLineWidth = GetCurrentLineWidth(fm, s);//fm.stringWidth(s)-itsOldLineWidth;
	aWidth = itsOwner.itsFontMetrics.getMaxAdvance();
	if (aCurrentLineWidth >= getSize().width-5) {  
	  int step;
	  if(aWidth>20) step = aWidth;
	  else step = 30;
  
	  itsOwner.setSize(itsOwner.Size().width+step, itsOwner.Size().height);
	  setSize(getSize().width+step, getSize().height);
	  requestFocus();
	} 
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
  
  public int GetCurrentLineWidth(FontMetrics fm, String theString){
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;
    while((aIndex!=-1)&&(aIndex<aPos)){
      aOldIndex = aIndex;
      aIndex = theString.indexOf("\n", aOldIndex+1);
    } 
    if(aIndex==-1){
      if(aOldIndex==-1) return fm.stringWidth(theString);
      else return fm.stringWidth(theString.substring(aOldIndex));
    }
    else return fm.stringWidth(theString.substring(aOldIndex, aIndex));
  }  
  
  public int GetCurrentLineChars(String theString){
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;
    while((aIndex!=-1)&&(aIndex<aPos)){
      aOldIndex = aIndex;
      aIndex = theString.indexOf("\n", aOldIndex+1);
    } 
    if(aIndex==-1){
      if(aOldIndex==-1) return theString.length();
      else return theString.length()-aOldIndex;
    }
    else return aIndex-aOldIndex;
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    Dimension r = itsOwner.getPreferredSize();
    Dimension d = new Dimension(r.width-2, r.height-2);
    return d;
  }

  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return getMinimumSize();
  }    
}














