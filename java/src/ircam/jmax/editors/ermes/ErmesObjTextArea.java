package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.utils.*;

//
// The text area contained in a "comment" object (ErmesObjComment)
//
class ErmesObjTextArea extends TextArea implements KeyListener, FocusListener {

  protected ErmesObjComment itsOwner = null;
  private ErmesSketchPad itsSketchPad = null;
  private boolean focused = false;
  
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjTextArea( ErmesSketchPad theSketchPad) 
  {
    super( " ", 5, 20, TextArea.SCROLLBARS_NONE);

    setFont( new Font( ircam.jmax.utils.Platform.FONT_NAME,Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));

    setEditable( true);

    selectAll();
    
    addKeyListener( this);
    addFocusListener( this);

    itsSketchPad = theSketchPad;
  }

  public void removeNotify()
  {
    removeKeyListener(this);
    removeFocusListener(this);
    super.removeNotify();
  }

  public void AbortEdit()
  {
    setVisible( false);

    setLocation( -200,-200);

    focused = false;
    
    if( itsSketchPad.itsToolBar.locked) 
      itsSketchPad.editStatus = ErmesSketchPad.START_ADD;
    else
      itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;

    itsSketchPad.repaint();

    itsOwner = null;  
  }

  protected void LostFocus() 
  {
    if ( !focused) 
      return;

    focused = false;
    itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
    itsSketchPad.itsSketchWindow.requestFocus();

    String aTextString = getText().trim();

    if ( !itsOwner.getArgs().equals( aTextString) )
      {
	itsOwner.redefine( aTextString);

	itsSketchPad.repaint();
      }

    setRows( 5);
    setColumns( 20);

    AbortEdit();
  }
	
  public void focusGained( FocusEvent e)
  {
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_COMMENT;

    if ( !focused)
      focused = true;
  }

  public void focusLost( FocusEvent e)
  {
  }

  public void keyTyped( KeyEvent e)
  {
  }

  public void keyReleased( KeyEvent e)
  {
  }

  public void keyPressed( KeyEvent e)
  {
    String s1, s2;
    int start = getSelectionStart();
    int end = getSelectionEnd();
    String s = getText();
    FontMetrics fm = getFontMetrics( getFont());
    int aWidth = 0;
    int aCurrentLineWidth = 0;
    int aCurrentLineChars = 0;
    
    if ( isEditable()) 
      {
	if( e.getKeyCode() == ircam.jmax.utils.Platform.ENTER_KEY
	    || e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY)
	  {
	    requestFocus();
	    return;
	  }
	else if( (e.getKeyCode() == 37) || (e.getKeyCode() == 38) 
		 || (e.getKeyCode() == 39) || (e.getKeyCode() == 40))
	  return;
	else
	  {//scrittura
	    aCurrentLineChars = GetCurrentLineChars( s);
	    if( aCurrentLineChars + 10 > getColumns())
	      setColumns( getColumns() + 20);

	    char k = e.getKeyChar();

	    if( start != end)
	      {//cancella selezione
		if( !e.isShiftDown())
		  {
		    s1 = s.substring( 0, start);
		    s2 = s.substring( end, s.length());
		    s = s1+s2;
		  }
	      }
	    if( start < s.length())
	      {//inserisce testo intermedio
		s1 = s.substring( 0, start);
		s2 = s.substring( start, s.length());
		s = s1 + k + s2;
	      }
	    else//inserisce testo in coda
	      s = s+k;
	
	    aCurrentLineWidth = GetCurrentLineWidth( fm, s);//fm.stringWidth( s)-itsOldLineWidth;
	    aWidth = itsOwner.itsFontMetrics.getMaxAdvance();
	    if ( aCurrentLineWidth >= getSize().width - 5) 
	      {
		int step;
		if( aWidth > 20) 
		  step = aWidth;
		else
		  step = 30;
  
		//itsOwner.setSize( itsOwner.Size().width + step, itsOwner.Size().height);
		setSize( getSize().width + step, getSize().height);
		requestFocus();
	      } 
	  }
      }
  }
  
  public int GetCurrentLineWidth( FontMetrics fm, String theString)
  {
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf( "\n");
    int aOldIndex = -1;

    while( (aIndex != -1) && (aIndex < aPos))
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf( "\n", aOldIndex + 1);
      } 

    if( aIndex == -1)
      {
	if (aOldIndex == -1) 
	  return fm.stringWidth( theString);
	else 
	  return fm.stringWidth( theString.substring( aOldIndex));
      }
    else 
      {
	if( aOldIndex==-1) 
	  return fm.stringWidth( theString.substring( 0,aIndex));
	else 
	  return fm.stringWidth( theString.substring( aOldIndex, aIndex));
      }
  }  
  
  public int GetCurrentLineChars( String theString)
  {
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf( "\n");
    int aOldIndex = -1;

    while ( (aIndex != -1) && (aIndex < aPos))
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf( "\n", aOldIndex + 1);
      } 
    if (aIndex == -1)
      {
	if (aOldIndex == -1)
	  return theString.length();
	else
	  return theString.length() - aOldIndex;
      }
    else
      {
	if (aOldIndex == -1) 
	  return aIndex;
	else
	  return aIndex - aOldIndex;
      }
  }

  private Dimension minimumSize = new Dimension();

  public Dimension getMinimumSize() 
  {
    minimumSize.setSize( itsOwner.getWidth() - itsOwner.getWhiteXOffset(), itsOwner.getHeight() - itsOwner.getWhiteYOffset());
    return minimumSize;
  }

  public Dimension getPreferredSize() 
  {
    return getMinimumSize();
  }    
}
