package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;

public class Led extends JComponent {

  public static final int OFF = 0;
  public static final int ON = 1;

  public Led()
  {
    this( OFF);
  }

  public Led( int state)
  {
    this( state, Color.red, Color.white);
  }

  public Led( int state, Color onColor, Color offColor)
  {
    this.state = state;
    this.onColor = onColor;
    this.offColor = offColor;
  }

  public void setOnColor( Color color)
  {
    onColor = color;
  }

  public Color getOnColor()
  {
    return onColor;
  }

  public void setOffColor( Color color)
  {
    offColor = color;
  }

  public Color getOffColor()
  {
    return offColor;
  }

  public void setState( int state)
  {
    this.state = state;
    repaint();
  }

  public int getState()
  {
    return state;
  }

  public Dimension getPreferredSize()
  {
    return getMaximumSize();
  }

  public Dimension getMaximumSize()
  {
    return new Dimension( 14, 14);
  }

  public void paint( Graphics g)
  {
    int size = getSize().width;

    g.setColor( Color.white);
    g.drawRect( 1, 1, size - 2, size - 2);

    g.setColor( Color.black);
    g.drawRect( 0, 0, size - 2, size - 2);

    g.setColor( state == OFF ? offColor: onColor);
    g.fillRect( 2, 2, size - 5, size - 5);
  }

  private Color onColor, offColor;
  private int state;
}

