//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;

// import javax.swing.*;
import javax.swing.JTextArea;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

// import javax.swing.text.*;
import javax.swing.text.JTextComponent;
import javax.swing.text.Keymap;
import javax.swing.text.TextAction;

// import javax.swing.event.*;
import javax.swing.event.CaretEvent;
import javax.swing.event.CaretListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

//
// The edit field contained in the editable objects (Message, Editable).
//

public class EditField extends JTextArea implements FocusListener
{
  private Editable owner = null;
  private ErmesSketchPad sketch = null;

  // Private action classes 

  static class StopEditAction extends TextAction
  {
    StopEditAction()
    {
      super("stop");
    }
    
    public void actionPerformed(ActionEvent e)
    {
      EditField f = (EditField) getTextComponent(e);
      
      f.sketch.stopTextEditing();
    }
  }


  // Key Event Listener for Edit Field
  // Horrible hack; it seems that there is bug in our
  // version of Java/AWT and or Swwing, so that the event sequence is not
  // correct, and we need to "filter it"; Pressing Ctrl-E generate:
  //
  // Pressed VK_CONTROL
  // Typed   'e'
  // Pressed Ctrl-E
  // 
  // .. and no released events; this is completely inconsistent with
  // what the doc say !!!

  class EditFieldKeyListener implements KeyListener
  {
    boolean consumeNext = false;

    public void keyTyped(KeyEvent e)
    {
      if (consumeNext)
	{
	  e.consume();
	  consumeNext = false;
	}
    }

    public void keyPressed(KeyEvent e)
    {
      if((e.getKeyCode() == KeyEvent.VK_CONTROL)||(e.isControlDown()))
	consumeNext = true;
      else if((!EditField.this.owner.isMultiline())&&(e.getKeyCode() == KeyEvent.VK_ENTER))
	  {
	      e.consume();
	      EditField.this.sketch.stopTextEditing();
	  }
    }

    public  void keyReleased(KeyEvent e){}
  }


  // The document listener that handle automatic resize

  class EditFieldListener implements DocumentListener
  {
    public void insertUpdate(DocumentEvent e)
    {
      resizeIfNeeded();
    }

    public void removeUpdate(DocumentEvent e)
    {
      resizeIfNeeded();
    }

    public void changedUpdate(DocumentEvent e)
    {
      resizeIfNeeded();
    }
  }

  private EditFieldListener  listener = new EditFieldListener();

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  EditField(ErmesSketchPad editor) 
  {
    super();
    setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));

    sketch = editor;

    setFont(FontCache.lookupFont(ircam.jmax.Platform.FONT_NAME,
				 ircam.jmax.Platform.FONT_SIZE,
				 ircam.jmax.Platform.FONT_STYLE));

    Keymap myMap = JTextComponent.addKeymap(null, getKeymap());

    setKeymap(myMap);      

    myMap.addActionForKeyStroke(KeyStroke.getKeyStroke(KeyEvent.VK_END, 0),
				new StopEditAction());

    addKeyListener(new EditFieldKeyListener());

    setEditable(true);
    setLineWrap(true);
    setWrapStyleWord(true);

    addFocusListener(this);

    //bug fix: to avoid caret position bigger than text length after a drag selection
    addCaretListener(new CaretListener(){
	    public void caretUpdate(CaretEvent e)
	    {
		int lt = EditField.this.getText().length();
		if(e.getDot()>lt)
		    setSelectionEnd(lt);
	    };
	});
  }

  // actually starts the edit operation.

  private Point location = new Point();
  private boolean doneOnce = false;

  public void doEdit(Editable obj, Point p)
  {
    owner = obj;
    owner.setEditing(true);
    setFont(owner.getFont());

    setText(owner.getArgs());

    setBackground(owner.getTextBackground());

    setBounds(owner.getX() + owner.getTextXOffset(),
	      owner.getY() + owner.getTextYOffset(),
	      owner.getWidth() - owner.getVariableWidth() - owner.getTextWidthOffset(),
	      owner.getHeight() - owner.getTextHeightOffset());

    if (p != null)
      {
	location.setLocation(p);
	location.x = location.x - getX();
	location.y = location.y - getY();
	
	// Workaround: viewToModel don't work if this is the
	// first time we make the JTextArea visible; it probabily
	// have something to do with the fact that the object is completed
	// by some calls in the event thread, and the information is not yet
	// available after the setVisible(true); so, the first time we
	// do the cursor placement using an invokeLater call ..

	if (doneOnce)
	  {
	    int pos = viewToModel(location);

	    if ((pos >= 0) && (pos <= owner.getArgs().length()))
	      setCaretPosition(pos);
	    else
	      setCaretPosition(owner.getArgs().length());
	  }
	else
	  {

	    SwingUtilities.invokeLater(new Runnable()
	      {
		public void run()
		{
		  int pos = viewToModel(location);
		  
		  if (pos >= 0)
		    setCaretPosition(pos);
		  else
		    setCaretPosition(owner.getArgs().length());
		}
	      });
	    doneOnce = true;
	  }
      }

    getDocument().addDocumentListener(listener);
    setVisible(true);
    requestFocus();
  }

    
  public void endEdit() 
  {
    String aTextString = getText().trim();

    owner.setEditing(false);

    getDocument().removeDocumentListener(listener);

    setVisible(false);

    if (! owner.getArgs().equals(aTextString) )
      {
	owner.redefine(aTextString);
      } 
    else{
      owner.redraw();
      owner.redrawConnections();
    }

    owner = null;
  }

  public void abortEdit() 
  {
    owner.setEditing(false);
    getDocument().removeDocumentListener(listener);
    setVisible(false);
    owner.redraw();
    owner.redrawConnections();
  }

  /** Resize the box if needed; must be done in an invoke later,
    otherwise we get one character of error; don't ask me why
    */
    
  void resizeIfNeeded()
  {
    SwingUtilities.invokeLater(new Runnable()
      {
	public void run()
	{
	  if (getText() != null)
	    {
	      Dimension d;
	      
	      owner.redraw();
	      owner.redrawConnections();
	      
	      if(owner.isMultiline())
		{
		  d = getPreferredSize();
		  owner.setHeight(d.height + owner.getTextHeightOffset());
		}					 
	      else
		{
		  owner.setWidthToText(getText());
		  
		  d = getSize();
		  d.width = owner.getWidth() - owner.getVariableWidth() - owner.getTextWidthOffset();
		}
	      
	      owner.redraw();
	      owner.redrawConnections();
	      
	      setSize(d);
	      sketch.fixSize();
	    }
	}
      });
  }
				 
       
  // Support for cut editing operations

  void deleteSelectedText()
  {
    String s = getText();
    setText(s.substring(0, getSelectionStart()) +  s.substring(getSelectionEnd(), s.length()));
  }

  public void focusGained(FocusEvent e)
  {
    // Ignore
  }

  public void focusLost(FocusEvent e)
  {
    if (owner != null)
      sketch.stopTextEditing();
  }
  public void paintComponent(Graphics g)
  {
      ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);	
      super.paintComponent(g);
  }
}











