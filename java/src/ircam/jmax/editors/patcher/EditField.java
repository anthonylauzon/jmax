package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
//

public class EditField extends JTextArea
{
  private ErmesObjEditableObject owner = null;
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
      if (e.getKeyCode() == KeyEvent.VK_CONTROL)
	consumeNext = true;
    }

    public  void keyReleased(KeyEvent e)
    {
    }
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

  EditFieldListener  listener = new EditFieldListener();

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  EditField(ErmesSketchPad editor) 
  {
    super();

    setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));

    sketch = editor;

    setFont(FontCache.lookupFont(ircam.jmax.utils.Platform.FONT_NAME,
				 ircam.jmax.utils.Platform.FONT_SIZE));

    Keymap myMap = JTextComponent.addKeymap(null, getKeymap());

    setKeymap(myMap);      

    myMap.addActionForKeyStroke(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0),
				new StopEditAction());

    addKeyListener(new EditFieldKeyListener());

    setEditable(true);
    setLineWrap(true);
    setWrapStyleWord(true);

    selectAll();
  }

  // actually starts the edit operation.

  private Point location = new Point();
  private boolean doneOnce = false;

  public void doEdit(ErmesObjEditableObject obj, Point p)
  {
    owner = obj;
    owner.setEditing(true);
    setFont(owner.getFont());
    setText(owner.getArgs());

    setBackground(owner.getTextBackground());

    setBounds(owner.getX() + owner.getTextXOffset(),
	      owner.getY() + owner.getTextYOffset(),
	      owner.getWidth() - owner.getTextWidthOffset(),
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

	    if (pos >= 0)
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

    if (! owner.getArgs().equals(aTextString) )
      {
	owner.redefine(aTextString);
	owner.redraw();
      } 

    setVisible(false);

    sketch.requestFocus();
    owner = null;
  }


  void resizeIfNeeded()
  {
    if (getText() != null)
      {
	Dimension d = getPreferredSize();

	owner.redraw();
	owner.setHeight(d.height + owner.getTextHeightOffset());
	owner.redraw();
	setSize(d);
	sketch.fixSize();
      }
  }
       
  // Support for cut editing operations

  void deleteSelectedText()
  {
    String s = getText();

    setText(s.substring(0, getSelectionStart()) +  s.substring(getSelectionEnd(), s.length()));
  }
}
