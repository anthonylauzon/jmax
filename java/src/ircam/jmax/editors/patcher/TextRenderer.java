package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.editors.patcher.objects.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (Message, Standard).
// Must subclass JTextArea in order to  use it, because the getColumnWidth is
// protected in JTextArea, and we *need* to know.

public class TextRenderer implements ObjectRenderer
{
  class RenderTextArea extends JTextArea
  {
    public boolean canResizeWidthTo(int width)
    {
      return width > getColumnWidth();
    }
  }

  private Editable owner;
  private RenderTextArea area;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public TextRenderer(Editable object)
  {
    super();

    owner = object;
    area = new RenderTextArea();

    area.setEditable(false);
    area.setLineWrap(true);
    area.setWrapStyleWord(true);
  }

  public void update()
  {
    if (owner.getHeight() > 0)
      area.setBounds(owner.getX() + owner.getTextXOffset(),
		     owner.getY() + owner.getTextYOffset(),
		     owner.getWidth() - owner.getTextWidthOffset(),
		     owner.getHeight() - owner.getTextHeightOffset());
    else
      area.setBounds(owner.getX() + owner.getTextXOffset(),
		     owner.getY() + owner.getTextYOffset(),
		     owner.getWidth() - owner.getTextWidthOffset(),
		     5);


    area.setFont(owner.getFont());
    area.setText(owner.getArgs());
  }

  public boolean canResizeWidthTo(int width)
  {
    return area.canResizeWidthTo(width);
  }

  public int getHeight()
  {
    return area.getPreferredSize().height;
  }

  public int getWidth()
  {
    return area.getPreferredSize().width;
  }

  public void setBackground(Color color)
  {
    area.setBackground(color);
  }

  static Container ic = new Panel();

  public void render(Graphics g, int x, int y, int w, int h)
  {
    SwingUtilities.paintComponent(g, area, ic, x, y, w, h);
  }
}

