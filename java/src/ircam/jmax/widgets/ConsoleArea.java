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
// Based on Max/ISPW by Miller Puckette.
//
// Author: Francois Dechelle.
// 

package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * A <code>ConsoleArea</code> Swing component is a component for
 * fast display of text lines.
 * It is read-only and cannot be used for copying text.
 * @author	François Déchelle
 * @since	2.4.9
 */

public class ConsoleArea extends JComponent implements Scrollable {

  /**
   * Constants
   */
  private static final int DEFAULT_ROWS = 24;
  private static final int DEFAULT_COLUMNS = 80;

  /**
   * The circular buffer of lines
   */
  private String buffer[];

  /**
   * Number of rows and columns
   */
  private int rows;
  private int columns;

  /*
   * First line, number of lines
   */
  private int first;
  private int nLines;

  /*
   * Caching of font Metrics informations
   */
  private int lineHeight;
  private int maxAscent;

  /*
   * Caching of viewport, if parent is a viewport, and of visibleRect, 
   */
  private JViewport viewport;
  private int visibleY;
  private int visibleHeight;

  /*
   * Number of lines to jump
   */
  private int jump;

  /*
   * Constructs a new console area, with a default number
   * of rows and columns
   */
  public ConsoleArea()
  {
    this( DEFAULT_ROWS, DEFAULT_COLUMNS);
  }

    /**
     * Constructs a new ConsoleArea with the specified number of
     * rows and columns.
     * @param rows the number of rows
     * @param columns the number of columns
     */
  public ConsoleArea( int rows, int columns)
  {
    setRows( rows);
    setColumns( columns);

    first = 0;
    nLines = 0;

    lineHeight = 0;
    maxAscent = 0;

    viewport = null;
    visibleY = 0;
    visibleHeight = 0;
    jump = 0;

    setOpaque( true);
    setBackground( Color.white);

    addComponentListener( new ComponentAdapter() {
      public void componentShown( ComponentEvent e)
	{
	  System.err.println( "componentShown " + getFont());
	  updateMetrics();
	}
    });

    setFont( new Font( "SansSerif", Font.PLAIN, 12));
  }

  /*
   * Sets the number of rows of the stored text.
   * @param	rows  the number of rows
   * @see	ircam.jmax.widgets.ConsoleArea#getRows
   */
  public void setRows( int rows)
  {
    this.rows = rows;
    buffer = new String[rows];
  }

  /*
   * Gets the number of rows of the stored text.
   * @return	the number of rows of the stored text
   * @see	ircam.jmax.widgets.ConsoleArea#setRows
   */
  public int getRows()
  {
    return rows;
  }

  /*
   * Sets the number of columns of the stored text.
   * @param	columns  the number of columns
   * @see	ircam.jmax.widgets.ConsoleArea#getColumns
   */
  public void setColumns( int columns)
  {
    this.columns = columns;
  }

  /*
   * Gets the number of columns of the stored text.
   * @return	the number of columns of the stored text
   * @see	ircam.jmax.widgets.ConsoleArea#setColumns
   */
  public int getColumns()
  {
    return columns;
  }

  /*
   * Update the cached informations about the font metrics.
   */
  private void updateMetrics()
  {
    FontMetrics fm = getFontMetrics( getFont());

    if (fm != null)
      {
	lineHeight = fm.getHeight();
	maxAscent = fm.getMaxAscent();
      }
  }
  
    /**
     * Determines the preferred size of a console area
     * @return    the preferred dimensions required to display 
     *                       this console area
     * @see       javax.swing.jComponent#getPreferredSize
     */
  public Dimension getPreferredSize()
  {
    FontMetrics fm = getFontMetrics(getFont());
    int maxAdvance;

    if (fm != null)
      {
	maxAdvance = fm.getMaxAdvance();

	if (maxAdvance <= 0)
	  maxAdvance = 7;
      }
    else
      maxAdvance = 0;

    return new Dimension( getColumns()*maxAdvance, getRows()*lineHeight);
  }

    /** 
     * Sets the font of this component.
     * Update the cached font metrics informations and call super.setFont()
     * @param <code>f</code> The font to become this component's font.
     */
  public void setFont( Font f)
  {
    super.setFont( f);

    updateMetrics();
  }

  public void addNotify()
  {
    super.addNotify();

    Container parent = getParent();

    if (parent instanceof JViewport)
      {
	viewport = (JViewport)parent;

	viewport.addChangeListener( new ChangeListener() {
	  public void stateChanged(ChangeEvent e)
	    {
	      Rectangle visibleRect = viewport.getViewRect();
	      visibleY = visibleRect.y;
	      visibleHeight = visibleRect.height;
	      jump = visibleHeight / lineHeight - 2;
	    }
	});
      }
  }

  // Methods from Scrollable interface

    /**
     * Returns the preferred size of the viewport for a view component.
     * @return The preferredSize of a JViewport whose view is this Scrollable.
     */
  public Dimension getPreferredScrollableViewportSize()
  {
    FontMetrics fm = getFontMetrics( getFont());

    return new Dimension( getColumns() * fm.getMaxAdvance(), getRows() * fm.getHeight());
  }

  /**
   * Computes the scroll increment that will completely expose one block of rows or 
   * columns, depending on the value of orientation.
   * @param visibleRect The view area visible within the viewport
   * @param orientation Either SwingConstants.VERTICAL or SwingConstants.HORIZONTAL.
   * @param direction Less than zero to scroll up/left, greater than zero for down/right.
   * @return The "block" increment for scrolling in the specified direction.
   */
  public int getScrollableBlockIncrement( Rectangle visibleRect, int orientation, int direction)
  {
    if (orientation == SwingConstants.HORIZONTAL)
      return visibleRect.width;
    else
      return visibleRect.height;
  }

  /**
   * Return true if a viewport should always force the height of this 
   * Scrollable to match the height of the viewport.
   *
   * @return True if a viewport should force the Scrollables height to match its own.
   */
  public boolean getScrollableTracksViewportHeight()
  {
    return false;
  }

  /**
   * Return true if a viewport should always force the width of this 
   * Scrollable to match the width of the viewport.
   *
   * @return True if a viewport should force the Scrollables width to match its own.
   */
  public boolean getScrollableTracksViewportWidth()
  {
    return false;
  }

  /**
   * Components that display logical rows or columns should compute
   * the scroll increment that will completely expose one new row
   * or column, depending on the value of orientation.
   *
   * @param visibleRect The view area visible within the viewport
   * @param orientation Either SwingConstants.VERTICAL or SwingConstants.HORIZONTAL.
   * @param direction Less than zero to scroll up/left, greater than zero for down/right.
   * @return The "unit" increment for scrolling in the specified direction
   */
  public int getScrollableUnitIncrement( Rectangle visibleRect, int orientation, int direction)
  {
    if (orientation == SwingConstants.HORIZONTAL)
      return getFontMetrics( getFont()).getMaxAdvance();
    else
      return getFontMetrics( getFont()).getHeight();
  }

  /**
   * Appends the given line of text to the end of the console area.
   * @param     line the line to append.
   */
  public void append( String line)
  {
    // Possible optimization: append an array of strings and do the repaint at the end

    // update the number of columns so that long lines are not truncated.
    int l = line.length();
    if ( l >= getColumns())
      {
	setColumns( l + 2);
	invalidate();
      }

    boolean mustJump = (nLines >= rows);

    // Buffer is full: create a hole of 'jump' lines
    if ( mustJump)
      {
	first = (first + jump) % rows;
	nLines -= jump;
      }

    // add the line to the buffer
    buffer[ (first + nLines) % rows] = line;
    nLines++;

    int y = (nLines - 1) * lineHeight;

    if ( viewport != null)
      {
	// The panel is in a viewport. Is the appended line visible ?

	if ( (y < visibleY) || (visibleY + visibleHeight < y) )
	  {
	    // make it visible. This will trigger a repaint()
	    viewport.setViewPosition( new Point( 0, y - 2 * lineHeight));
	    return;
	  }
      }

    if (mustJump)
      repaint();
    else
      repaint( 0, y, getSize().width, lineHeight);
  }

  public void paintComponent( Graphics g)
  {
    /*****************/
    //jdk117-->jdk1.3//
    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
    /*****************/

    Rectangle clip = g.getClipBounds();
    int clipWidth = clip.width;
    int clipHeight = clip.height;
    int clipX = clip.x;
    int clipY = clip.y;

    g.setColor( getBackground());

    g.fillRect( clipX, clipY, clipWidth, clipHeight);

    g.setColor( getForeground());

    int lineToPaint = clipY / lineHeight;

    if (lineToPaint >= nLines)
      return;

    int lastLineToPaint = lineToPaint + (clipHeight / lineHeight) + 1;

    lastLineToPaint = ( lastLineToPaint >= nLines ) ? nLines : lastLineToPaint;

    int y = maxAscent + lineToPaint * lineHeight;

    while( lineToPaint < lastLineToPaint)
      {
	g.drawString( buffer[(lineToPaint + first) % rows], 2, y);

	y += lineHeight;

	lineToPaint++;
      }
  }
  
  public void print( Graphics g)
  {
    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

    Rectangle clip = g.getClipBounds();
    int clipWidth = clip.width;
    int clipHeight = clip.height;
    int clipX = clip.x;
    int clipY = clip.y;

    g.setColor( getBackground());

    g.fillRect( clipX, clipY, clipWidth, clipHeight);

    g.setColor( getForeground());

    int lineToPaint = 0;

    if (lineToPaint >= nLines)
      return;

    int lastLineToPaint = nLines;

    int y = maxAscent + lineToPaint * lineHeight;

    while( lineToPaint < lastLineToPaint)
      {
	g.drawString( buffer[lineToPaint], 2, y);

	y += lineHeight;

	lineToPaint++;
      }
  }
}

