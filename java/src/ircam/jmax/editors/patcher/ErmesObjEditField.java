package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;

import javax.swing.*;

//
// The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
//
public class ErmesObjEditField extends JTextArea implements KeyListener, FocusListener {

  private ErmesObjEditableObject itsOwner = null;
  private ErmesSketchPad itsSketchPad = null;
  private boolean focused = false;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjEditField( ErmesSketchPad theSketchPad) 
  {
    super("", 1, 20);

    setBackground(Color.white);

    setFont(FontCache.lookupFont(ircam.jmax.utils.Platform.FONT_NAME,
				 ircam.jmax.utils.Platform.FONT_SIZE));

    setEditable(true);

    selectAll();

    addKeyListener(this);
    addFocusListener(this);

    itsSketchPad = theSketchPad;
  }

  public void removeNotify()
  {
    removeKeyListener(this);
    removeFocusListener(this);
    super.removeNotify();
  }

  private void AbortEdit() 
  {
    setVisible( false);

    setLocation( -200, -200);
    focused = false;
    itsOwner = null;

  }

  public void setOwner(ErmesObjEditableObject owner)
  {
    itsOwner = owner;
  }

  //--------------------------------------------------------
  // lostFocus
  //--------------------------------------------------------
  public void LostFocus() 
  {
    if (! focused)
      return ;

    focused = false;

    String aTextString = getText().trim();

    if ( !itsOwner.getArgs().equals( aTextString) )
      {
	itsOwner.redefine( aTextString);

	itsSketchPad.repaint(); // @@@REPAINT
      } 

    setRows( 2);
    setColumns( 20);

    AbortEdit();
  }

  public void focusGained( FocusEvent e) 
  { 
    if (!focused)
      focused = true;
  }

  public void focusLost(FocusEvent e) 
  {
  }

  public boolean HasFocus() 
  {
    return focused;
  }

  public void keyTyped(KeyEvent e) 
  {
  }

  public void keyReleased(KeyEvent e) 
  {
  }

  public void keyPressed(KeyEvent e) 
  {
    int lenght;
    String s1, s2;
    String s = getText();
    FontMetrics fm = getFontMetrics(getFont());
    int aWidth = 0;
    int aCurrentLineWidth = 0;
    int aCurrentLineChars = 0;

    if (isEditable()) 
      {
	if (e.getKeyCode()== KeyEvent.VK_ENTER)
	  {//return
	    setRows(getRows()+1);
	    Dimension d2 = itsOwner.Size();
	    itsOwner.setWidth(d2.width);
	    itsOwner.setHeight(d2.height + fm.getHeight());
	    itsSketchPad.repaint();

	    setSize( getSize().width, getSize().height + fm.getHeight());
	    requestFocus();
	    return;
	  } 
	else 
	  {
	    aCurrentLineChars = getCurrentLineChars();
	    if (aCurrentLineChars+10 > getColumns())
	      setColumns(getColumns()+20);

	    char k = e.getKeyChar();

	    if ((k == KeyEvent.VK_DELETE)     ||
		(k == KeyEvent.VK_BACK_SPACE) ||
		(k == KeyEvent.VK_LEFT)       ||
		(k == KeyEvent.VK_RIGHT)      ||
		(k == KeyEvent.VK_UP)         ||
		(k == KeyEvent.VK_DOWN))
	      return;

	    aCurrentLineWidth = getCurrentLineWidth( fm);
	    aWidth = itsOwner.getFontMetrics().getMaxAdvance();

	    if (aCurrentLineWidth >= getSize().width-20) 
	      {
		int step;

		if ( aWidth > 20)
		  step = aWidth;
		else
		  step = 30;

		itsOwner.setWidth(itsOwner.Size().width + step);
		itsOwner.setHeight(itsOwner.Size().height);
		itsSketchPad.repaint();
		setSize( getSize().width + step, getSize().height);
		requestFocus();
	      }
	  }
      }
  }

  public int getCurrentLineWidth(FontMetrics fm) 
  {
    String theString = getText();
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;

    while ((aIndex!=-1)&&(aIndex<aPos)) 
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf("\n", aOldIndex+1);
      }

    if (aIndex==-1) 
      {
	if (aOldIndex==-1)
	  return fm.stringWidth(theString);
	else
	  return fm.stringWidth(theString.substring(aOldIndex));
      } 
    else 
      {
	if (aOldIndex==-1)
	  return fm.stringWidth(theString.substring(0,aIndex));
	else
	  return fm.stringWidth( theString.substring(aOldIndex, aIndex));
      }
  }

  public int getCurrentLineChars()
  {
    String theString = getText();
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;

    while ((aIndex!=-1)&&(aIndex<aPos)) 
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf("\n", aOldIndex+1);
      }

    if (aIndex==-1) 
      {
	if (aOldIndex==-1)
	  return theString.length();
	else
	  return theString.length()-aOldIndex;
      } 
    else 
      {
	if (aOldIndex==-1)
	  return aIndex;
	else
	  return aIndex-aOldIndex;
      }
  }


  private Dimension minimumSize = new Dimension();

  public Dimension getMinimumSize() 
  {
    minimumSize.setSize( itsOwner.getWidth() - itsOwner.getWhiteXOffset(),
			 itsOwner.getHeight() - itsOwner.getWhiteYOffset());
    return minimumSize;
  }

  public Dimension getPreferredSize() 
  {
    return getMinimumSize();
  }

  // Support for cut editing operations

  void deleteSelectedText()
  {
    String s = getText();

    setText(s.substring(0, getSelectionStart()) +  s.substring(getSelectionEnd(), s.length()));
  }
}
