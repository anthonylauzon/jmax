package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;

abstract public class Led extends JComponent {

  public static final int OFF = 0;
  public static final int ON = 1;

  public Led()
  {
    this(  Color.red);
  }

  public Led( Color onColor)
  {
    this.state = OFF;
    this.onColor = onColor;
  }

  public void setOnColor( Color color)
  {
    onColor = color;
  }

  public Color getOnColor()
  {
    return onColor;
  }

  public void setState( int state)
  {
    this.state = state;
    repaint();
  }

  public void setState( boolean state)
  {
    setState( state ? ON : OFF);
  }

  public int getState()
  {
    return state;
  }

  public Dimension getMinimumSize()
  {
    return getPreferredSize();
  }

  public Dimension getMaximumSize()
  {
    return getPreferredSize();
  }

  protected Color onColor;
  protected int state;
}
