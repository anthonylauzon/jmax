package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

/**
 * The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
 */
public class ErmesObjEditField extends TextField implements KeyListener, FocusListener {
  boolean laidOut = false;
  boolean focused = false;    
  ErmesObjEditableObject itsOwner= null;
  ErmesSketchPad itsSketchPad = null;
	
	
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjEditField(ErmesSketchPad theSketchPad) {
    super(" ", 20);
    setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));		   
    setEditable(true); 
    selectAll();
    itsSketchPad = theSketchPad;
    //addKeyListener(((ErmesSketchWindow)itsSketchPad.itsSketchWindow).itsKeyAdapter);
    addKeyListener(this);
    addFocusListener(this);
  }
 
  ////////////////
  /*public boolean lostFocus() {
    return true;
    }*/
  //--------------------------------------------------------
  // lostFocus
  //--------------------------------------------------------
  public boolean LostFocus() {
    if (!focused) return true;
    else {
      focused = false;
      itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
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
    
    if (itsOwner == null) return false; //this happens when the instatiation fails
    if (itsOwner.itsFtsObject != null){
      itsOwner.itsArgs = aTextString;
      itsOwner.redefineFtsObject();
    }
    else {
      itsOwner.itsArgs = aTextString;
      itsOwner.makeFtsObject();
    }

    int lenght = getFontMetrics(getFont()).stringWidth(aTextString);
    
    if((lenght< getSize().width-20)&&(!itsOwner.itsResized)){
      Dimension d1 = itsOwner.Size();
      d1.width = lenght+itsOwner.WIDTH_DIFF+10;
      itsOwner.resize(d1.width, d1.height);
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
    return true;       
  }
	
  //--------------------------------------------------------
  // gotFocus
  //--------------------------------------------------------
  /*public boolean gotFocus(Event evt, Object what) {
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_OBJECT;
    if (focused) return true;
    else focused = true;
    
    if (getText().compareTo("") == 0) return true; 
    else return true;
    }*/
	
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
    int aWidth;
    
    if (isEditable()) {
      //if(e.getKeyCode()==ircam.jmax.utils.Platform.ENTER_KEY||e.getKeyCode()==ircam.jmax.utils.Platform.RETURN_KEY){//return
	//Dimension d2 = itsOwner.size();
	//d2.height += fm.getHeight();
	// itsOwner.ResizeTo(d2);
	//itsOwner.validate();
      //return;
      //}
      //else 
      if(e.getKeyCode() == Event.LEFT){//freccia a sinistra
	if(start==end){
	  if(start>0){
	    if(e.isShiftDown())
	      select(start-1, start);
	    else
	      select(start-1,start-1);
	  }
	}
	else{
	  if(e.isShiftDown())
	    select(start-1, end);
	  else
	    select(start,start);
	}
      }
      else if(e.getKeyCode() == Event.RIGHT){//freccia a destra
	if(start==end){
	  if(start < s.length()){
	    if(e.isShiftDown())
	      select(start, end+1);
	    else
	      select(start+1,start+1);
	  }
	}
	else{
	  if(e.isShiftDown())
	    select(start, end+1);
	  else
	    select(end,end);
	}
      }
      //else if((e.getKeyCode()==Event.UP)||(e.getKeyCode()== Event.DOWN))
      //return;
      else if (e.getKeyCode()==ircam.jmax.utils.Platform.DELETE_KEY || e.getKeyCode()==ircam.jmax.utils.Platform.BACKSPACE_KEY) {//cancellazione
	if(start==end){//se non c' testo selezionato
	  if(start>0){
	    if(start < s.length()){//cancella intermedio
	      s1 = s.substring(0, start-1);
	      s2 = s.substring(start, s.length());
	      s = s1+s2;
	    }
	    else//cancella in coda
	      s = s.substring(0, s.length()-1);
	    setText(s);
	    select(start-1,start-1);
	  }
	}
	else{//se c' testo selezionato
	  s1 = s.substring(0, start);
	  s2 = s.substring(end, s.length());
	  s = s1+s2;
	  setText(s);
	  select(start,start);
	}
	
	//meglio che cancellando non faccia il resize
	
	//lenght = fm.stringWidth(s);
	//if ((lenght< size().width-20)&&(lenght>itsOwner.minimumSize().width-20)){
	// Dimension d1 = itsOwner.Size();
	// d1.width -= 20;
	itsOwner.itsArgs = s;
	//itsOwner.resize(d1.width, d1.height);
	//}
      }
      else{//scrittura
	char k = e.getKeyChar();
	if(start!=end){//cancella selezione
	  s1 = s.substring(0, start);
	  s2 = s.substring(end, s.length());
	  s = s1+s2;
	  select(start,start);
	}
	if(start < s.length()){//inserisce testo intermedio
	  s1 = s.substring(0, start);
	  s2 = s.substring(start, s.length());
	  s = s1+k+s2;
	  setText(s);
	}
	else//inserisce testo in coda
	  s = s+k;
	
	lenght = fm.stringWidth(s);
	aWidth = itsOwner.itsFontMetrics.getMaxAdvance();
	if (lenght >= getSize().width-5) {
	  Dimension d = itsOwner.Size();
	  if(aWidth>20) d.width += aWidth;
	  else d.width += 30;
	  itsOwner.resize(d.width, d.height);
	}
	setText(s);
	select(start+1,start+1);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine




  //--------------------------------------------------------
  // keyDown()
  //--------------------------------------------------------
  /* public boolean keyDown(Event e, int k){
     int lenght;
     String s1, s2;
     int start = getSelectionStart();
     int end = getSelectionEnd();
     String s = getText();
     FontMetrics fm = getFontMetrics(getFont());
     int aWidth;
    
     if (isEditable()) {
     if(k == ircam.jmax.utils.Platform.ENTER_KEY || k == ircam.jmax.utils.Platform.RETURN_KEY){//return
     //Dimension d2 = itsOwner.size();
     //d2.height += fm.getHeight();
     // itsOwner.ResizeTo(d2);
     //itsOwner.validate();
     return true;
     }
     else if(k == Event.LEFT){//freccia a sinistra
     if(start==end){
     if(start>0){
     if(e.shiftDown())
     select(start-1, start);
     else
     select(start-1,start-1);
     }
     }
     else{
     if(e.shiftDown())
     select(start-1, end);
     else
     select(start,start);
     }
     }
     else if(k == Event.RIGHT){//freccia a destra
     if(start==end){
     if(start < s.length()){
     if(e.shiftDown())
     select(start, end+1);
     else
     select(start+1,start+1);
     }
     }
     else{
     if(e.shiftDown())
     select(start, end+1);
     else
     select(end,end);
     }
     }
     else if((k== Event.UP)||(k== Event.DOWN))
     return true;
     else if (k == ircam.jmax.utils.Platform.DELETE_KEY || k == ircam.jmax.utils.Platform.BACKSPACE_KEY) {//cancellazione
     if(start==end){//se non c' testo selezionato
     if(start>0){
     if(start < s.length()){//cancella intermedio
     s1 = s.substring(0, start-1);
     s2 = s.substring(start, s.length());
     s = s1+s2;
     }
     else//cancella in coda
     s = s.substring(0, s.length()-1);
     setText(s);
     select(start-1,start-1);
     }
     }
     else{//se c' testo selezionato
     s1 = s.substring(0, start);
     s2 = s.substring(end, s.length());
     s = s1+s2;
     setText(s);
     select(start,start);
     }
	
     //meglio che cancellando non faccia il resize
     
     //lenght = fm.stringWidth(s);
     //if ((lenght< size().width-20)&&(lenght>itsOwner.minimumSize().width-20)){
     // Dimension d1 = itsOwner.Size();
     // d1.width -= 20;
     itsOwner.itsArgs = s;
     //itsOwner.resize(d1.width, d1.height);
     //}
     }
     else{//scrittura
     if(start!=end){//cancella selezione
     s1 = s.substring(0, start);
     s2 = s.substring(end, s.length());
     s = s1+s2;
     select(start,start);
     }
     if(start < s.length()){//inserisce testo intermedio
     s1 = s.substring(0, start);
     s2 = s.substring(start, s.length());
     s = s1+(char)k+s2;
     setText(s);
     }
     else//inserisce testo in coda
     s = s+(char)k;
     
     
     lenght = fm.stringWidth(s);
     aWidth = itsOwner.itsFontMetrics.getMaxAdvance();
     if (lenght >= size().width-5) {
     Dimension d = itsOwner.Size();
     if(aWidth>20) d.width += aWidth;
     else d.width += 30;
     itsOwner.resize(d.width, d.height);
     }
     setText(s);
     select(start+1,start+1);
     }
     }
     return true;
     }
     */
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




