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
  int itsOldLineWidth = 0;
  int itsOldLineChars = 0;
	
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
      aTextString = aTextString.substring(0, aTextString.length()-1);
    }
    
    if (itsOwner == null) return false; //this happens when the instatiation fails
    if (itsOwner.itsFtsObject != null){
      itsOwner.itsArgs = aTextString;

      ParseText(aTextString);

      itsOwner.redefineFtsObject();
    }
    else {
      itsOwner.itsArgs = aTextString;
      
      ParseText(aTextString);

      itsOwner.makeFtsObject();
    }
    ///qui accorcia se il testo e' piu' corto dell'oggetto
    //dovrebbe farlo anche per l'altezza...

    int lenght = getFontMetrics(getFont()).stringWidth(aTextString);
  
    if((lenght< getSize().width-20)&&(!itsOwner.itsResized)){
      Dimension d1 = itsOwner.Size();
      d1.width = lenght+itsOwner.WIDTH_DIFF+10;
      itsOwner.setSize(d1.width, d1.height);
    }
    itsOwner.update(itsOwner.itsFtsObject);
    
    itsOwner.itsInEdit = false;
    
    setVisible(false);
    setLocation(-200,-200);
    focused = false;
    itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
    
    if (itsSketchPad != null) itsOwner.Paint(itsSketchPad.GetOffGraphics());
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());//end bug 12
    itsOwner = null;

    setRows(1);
    setColumns(20);
    return true;       
  }
	
  private void ParseText(String theString){
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;
    int aLastIndex = theString.lastIndexOf("\n");
    String aString;
    int length = 0;
    int i = 0;
    while(aIndex!=-1){
      aString = theString.substring(aOldIndex+1, aIndex);
      length = getFontMetrics(getFont()).stringWidth(aString);
      if(length> getFontMetrics(getFont()).stringWidth(itsOwner.itsMaxString)) 
	itsOwner.itsMaxString = aString;
      itsOwner.itsParsedTextVector.addElement(aString);
      aOldIndex = aIndex;
      aIndex = theString.indexOf("\n", aOldIndex+1);
      i++;
    }
    aString = theString.substring(aOldIndex+1);
    length = getFontMetrics(getFont()).stringWidth(aString);
    if(length> getFontMetrics(getFont()).stringWidth(itsOwner.itsMaxString)) 
      itsOwner.itsMaxString = aString;
    itsOwner.itsParsedTextVector.addElement(aString);
  }

  ///////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////focusListener --inizio
  public void focusGained(FocusEvent e){
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_OBJECT;
    if (focused) return;
    else focused = true;
    
    //if (getText().compareTo("") == 0) return true; 
    //else return true;
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
	itsOldLineWidth = fm.stringWidth(s);
	itsOldLineChars = s.length(); 
	Dimension d2 = itsOwner.Size();
	itsOwner.setSize(d2.width, d2.height+fm.getHeight());
	setSize(getSize().width, getSize().height + fm.getHeight());//non sposta le outlet se ci sono
	requestFocus();
	return;
      }
      //else if(e.getKeyCode() == Event.LEFT){//freccia a sinistra
      //if(start==end){
      //if(start>0){
      //if(e.isShiftDown())
      //select(start-1, start);
      //    else
      //    select(start-1,start-1);
      // }
      //	}
      //else{
      //if(e.isShiftDown())
      //  select(start-1, end);
      //else
      // select(start,start);
      //}
      // return;
      //}
      //else if(e.getKeyCode() == Event.RIGHT){//freccia a destra
      //if(start==end){
      //if(start < s.length()){
      //if(e.isShiftDown())
      //select(start, end+1);
      //	    else
      //      select(start+1,start+1);
      //}
      //}
      //else{
      //if(e.isShiftDown())
      //select(start, end+1);
      // else
      // select(end,end);
      //}
      //return;
      //}
      //else if((e.getKeyCode()==Event.UP)||(e.getKeyCode()== Event.DOWN))
      //	return;
      //else if (e.getKeyCode()==ircam.jmax.utils.Platform.DELETE_KEY || e.getKeyCode()==ircam.jmax.utils.Platform.BACKSPACE_KEY) {//cancellazione
      //if(start==end){//se non c' testo selezionato
      // if(start>0){
      // if(start < s.length()){//cancella intermedio
      // s1 = s.substring(0, start-1);
      // s2 = s.substring(start, s.length());
      // s = s1+s2;
      // }
      // else//cancella in coda
      // s = s.substring(0, s.length()-1);
      
      //setText(s);
      //select(start-1,start-1);
      // }
      //}
      //else{//se c' testo selezionato
      //s1 = s.substring(0, start+1);
      //  s2 = s.substring(end, s.length());
      //  s = s1+s2;
      //  setText(s);
      //  select(start+1,start+1);
      //}
      //itsOwner.itsArgs = s;
      //}
      else{//scrittura
	aCurrentLineChars = s.length() - itsOldLineChars;
	if(aCurrentLineChars+5 > getColumns())
	  setColumns(getColumns()+20);

	char k = e.getKeyChar();
	if(start!=end){//cancella selezione
	  if(!e.isShiftDown()){
	    s1 = s.substring(0, start);
	    s2 = s.substring(end, s.length());
	    s = s1+s2;
	    // setText(s);
	    //select(start,start);
	  }
	}
	if(start < s.length()){//inserisce testo intermedio
	  s1 = s.substring(0, start);
	  s2 = s.substring(start, s.length());
	  s = s1+k+s2;
	  //e.e.setText(s);
	}
	else//inserisce testo in coda
	  s = s+k;
	
	aCurrentLineWidth = fm.stringWidth(s)-itsOldLineWidth;
	aWidth = itsOwner.itsFontMetrics.getMaxAdvance();
	//System.out.println("fm.stringWidth(s) = "+fm.stringWidth(s));
	//System.out.println("itsOldLineWidth = "+aOldLineWidth);
	//System.out.println("aCurrentLineWidth = "+aCurrentLineWidth);
	//if (lenght >= getSize().width-5) {
	if (aCurrentLineWidth >= getSize().width-5) {  
	  int step;
	  if(aWidth>20) step = aWidth;
	  else step = 30;
  
	  itsOwner.setSize(itsOwner.Size().width+step, itsOwner.Size().height);
	  setSize(getSize().width+step, getSize().height);
	  requestFocus();
	} 
	//select(start,start);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

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





