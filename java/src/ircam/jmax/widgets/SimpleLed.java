package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

public class SimpleLed extends Led {

  public Dimension getPreferredSize()
  {
    return new Dimension( 14, 14);
  }

  public void paint( Graphics g)
  {
    int size = getSize().width;

    g.setColor( getForeground());
    g.drawRect( 0, 0, size - 1, size - 1);

    if ( state == ON)
      {
	g.setColor( onColor);
	g.fillRect( 2, 2, size - 4, size - 4);
      }
  }
}

