package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.editors.patcher.objects.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
//

public class TextRenderer extends JTextArea
{
  private ErmesObjEditableObject owner;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public TextRenderer(ErmesObjEditableObject object)
  {
    super();

    owner = object;

    setEditable(false);
    setLineWrap(true);
    setWrapStyleWord(true);
  }

  public void update()
  {
    setBounds(owner.getX() + owner.getTextXOffset(),
	      owner.getY() + owner.getTextYOffset(),
	      owner.getWidth() - owner.getTextWidthOffset(),
	      owner.getHeight() - owner.getTextHeightOffset());

    setFont(owner.getFont());
    setText(owner.getArgs());
  }

  public boolean canResizeWidthTo(int width)
  {
    return width > getColumnWidth();
  }
}
