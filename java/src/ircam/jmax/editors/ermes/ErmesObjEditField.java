package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

/**
 * The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
 */
public class ErmesObjEditField extends TextArea implements KeyListener, FocusListener {
  boolean laidOut = false;
  boolean focused = false;    
  ErmesObjEditableObject itsOwner= null;
  ErmesSketchPad itsSketchPad = null;
	
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjEditField(ErmesSketchPad theSketchPad) {
    super(" ", 1, 20, TextArea.SCROLLBARS_NONE);
    setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));		   
    setEditable(true); 
    selectAll();
    itsSketchPad = theSketchPad;
    addKeyListener(this);
    addFocusListener(this);
  }
 

  public void AbortEdit(){
    setVisible(false);
    setLocation(-200,-200);
    focused = false;

    if(itsSketchPad.itsToolBar.locked) itsSketchPad.editStatus = ErmesSketchPad.START_ADD;
    else itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
    itsOwner.itsInEdit = false;

    if (itsSketchPad != null) itsOwner.Paint(itsSketchPad.GetOffGraphics());
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());
    itsOwner = null;  
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
      AbortEdit();
      return true; 
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
      if(itsOwner.itsArgs.equals(aTextString)){
	itsOwner.itsArgs = aTextString;
	itsOwner.ParseText(aTextString);
	AbortEdit();
	return true;
      }
      else {
	itsOwner.itsArgs = aTextString;
	
	itsOwner.ParseText(aTextString);
	
	itsOwner.redefineFtsObject();

      }
    }
    else {

      itsOwner.itsArgs = aTextString;
      
      itsOwner.ParseText(aTextString);

      itsOwner.makeFtsObject();
    }

    itsOwner.UpdateOnly(itsSketchPad.GetOffGraphics());//

    int lenght = getFontMetrics(getFont()).stringWidth(itsOwner.itsMaxString);

    if(!itsOwner.itsResized){
      Dimension d1 = itsOwner.Size();
      d1.width = lenght+2*itsOwner.WIDTH_DIFF+10;
      itsOwner.setSize(d1.width, d1.height);
    }
    int height = getFontMetrics(getFont()).getHeight()*itsOwner.itsParsedTextVector.size();
    if(!itsOwner.itsResized){
      Dimension d1 = itsOwner.Size();
      d1.height = height+2*itsOwner.HEIGHT_DIFF;
      itsOwner.setSize(d1.width, d1.height);
      if(itsOwner.itsOutletList.size()>0)
	itsOwner.MoveOutlets();
    }
    //itsOwner.Repaint();
    itsOwner.update(itsOwner.itsFtsObject);
    
    AbortEdit();

    setRows(2);
    setColumns(20);
    return true;       
  }
	
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_OBJECT;
    if (focused) return;
    else focused = true;
  }

  public void focusLost(FocusEvent e){}

  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////focusListener --fine


	
  //--------------------------------------------------------
  // HasFocus()
  //--------------------------------------------------------
  public boolean HasFocus(){
    return focused;
  }

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
	setRows(getRows()+1);
	Dimension d2 = itsOwner.Size();
	itsOwner.setSize(d2.width, d2.height+fm.getHeight());
	if(itsOwner.itsOutletList.size()>0)
	  itsOwner.MoveOutlets();
	setSize(getSize().width, getSize().height + fm.getHeight());
	requestFocus();
	return;
      }
      else{//scrittura
	aCurrentLineChars = GetCurrentLineChars(s);
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
	
	aCurrentLineWidth = GetCurrentLineWidth(fm, s);
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
  // paint()
  //--------------------------------------------------------
  public void paint(Graphics g) {
    setBackground(Color.white);
  }
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    Dimension r = itsOwner.getPreferredSize();
    Dimension d = new Dimension(r.width-itsOwner.WIDTH_DIFF, r.height-itsOwner.HEIGHT_DIFF);
    return d;
  }
  
  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return getMinimumSize();
  }
  
}







