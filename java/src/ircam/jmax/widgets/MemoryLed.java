package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.lang.*;

import javax.swing.*;

public class MemoryLed extends Led {

  public MemoryLed()
  {
    memory = 0;

    addMouseListener( new MouseAdapter() {
      public void mouseClicked( MouseEvent e)
	{
	  reset();
	}
      });
  }

  public void setState( int state)
  {
    if ( state == Led.ON)
      memory = Led.ON;

    super.setState( state);
  }

  public Dimension getPreferredSize()
  {
    return new Dimension( size, size);
  }

  public void reset()
  {
    if ( state == OFF)
      {
	memory = OFF;
	repaint();
      }
  }

  public void paint( Graphics g)
  {
    g.setColor( getForeground());
    g.drawRect( 0, 0, size - 1, size - 1);

    if ( state == ON)
      {
	int xState = (size - sizeState) / 2;

	g.setColor( onColor);
	g.fillRect( xState, xState, sizeState, sizeState);
      }
    else if ( memory == ON)
      {
	int xMemory = (size - sizeMemory) / 2;

	g.setColor( onColor);
	g.fillRect( xMemory, xMemory, sizeMemory, sizeMemory);
      }
  }

  protected static final int size = 12;
  protected static final int sizeState = 8;
  protected static final int sizeMemory = 4;

  protected int memory;
}

