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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class Define extends Editable implements FtsObjectErrorListener{

  private static final int DEFAULT_WIDTH = 54;
  private static final int MINIMUM_WIDTH = 20;

  private String variableName;
  private String variableValue;
  protected FontMetrics boldFontMetrics = null;
  protected Font boldFont = null;

  public Define( FtsGraphicObject theFtsObject) 
  {
    super( theFtsObject);
    setDefaultWidth(MINIMUM_WIDTH);
        
    setNameAndValue();
    boldFont = getFont().deriveFont( Font.BOLD);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
  }

  void setNameAndValue()
  {
    String descr = ftsObject.getDescription();
    if(descr != null)
      {
	int space = descr.indexOf(' ');
	if( space != -1)
	  {
	    variableName = descr.substring( 0, space);
	    variableValue = descr.substring( space+1);
	  }
	else
	  {
	    variableName = descr;
	    variableValue = "";
	  }
      }
  }

  public boolean isResizable()
  {
    return false;
  }

  public void setDefaults()
  {
    setWidth( getWidth());
    updateDimensions();
  }

  public int getVariableWidth()
  {
    if( variableName!= null && !isEditing())
      return boldFontMetrics.stringWidth( variableName) + 2;
    else
      return 0;
  }

  public String getArgs()
  {
    if( variableName != null)
      return variableName+" "+variableValue;
    else 
      return "";
  }

  // redefined from base class
  public void setWidth( int theWidth)
  {
    if ( theWidth <= 0)
      theWidth = DEFAULT_WIDTH;
    else if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
  }

  public void setFont( Font theFont)
  {
    super.forceFont( theFont);
    boldFont = getFont().deriveFont( Font.BOLD);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
    
    forceWidth( getWidth());
    forceHeight( getFontMetrics().getHeight() + getTextHeightOffset());
    fitToText(); 
    redraw();
  }

  public void setCurrentFont( Font font)
  {
    super.setCurrentFont( font);
    boldFont = font.deriveFont( Font.BOLD);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
  }

  public void redefine( String text) 
  {
    text = "define "+text;
    ((FtsPatcherObject)ftsObject.getParent()).requestRedefineObject(ftsObject, text);
    itsSketchPad.getDisplayList().remove(this);
    dispose();
  }

  public void redefined()
  {    
    SwingUtilities.invokeLater(new Runnable(){
	public void run()
	{
	  fitToText();
	  redraw();
	}
      });
  }

  public void errorChanged(boolean value) 
  {
    redraw();
  } 

  public void fitToText()
  {
    forceWidth(  getTextHeightOffset() + getVariableWidth() + 4 + getFontMetrics().stringWidth( variableValue) + 3);
  }

  public Dimension getMinimumSize() 
  {
    return null;
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 2;

  public int getTextXOffset()
  {
    return getTextHeightOffset() + getVariableWidth() +2;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return  getTextHeightOffset() + getVariableWidth() + 4;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    if(ftsObject.isError())
      {
	if(isSelected())
	  return Color.gray.darker();
	else
	  return Color.gray;
      }
    else
      return Color.black;
  }

  public Color getTextBackground()
  {
    if(isSelected())
      return Settings.sharedInstance().getObjColor().darker();
    else
      return Settings.sharedInstance().getObjColor();
  }

  public boolean isMultiline()
  {
    return false;
  }

  public void edit(Point point)
  {
    redraw();
    super.edit( point);
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int hLine = getTextHeightOffset() + getVariableWidth()+2;

    g.setColor( getTextBackground());

    g.fillRect( x + 1, y + 1, w - 2,  h - 2);
    g.fill3DRect( x + 1, y + 1, hLine - 2,  h-2, true);

    if( variableName != null && !isEditing())
      {
	g.setColor( getTextForeground());

	int bottom = getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2;
	g.setFont( boldFont);
	g.drawString( variableName, x + 4, y + bottom);
	g.setFont( getFont());
	g.drawString( variableValue, x + hLine + 1, y + bottom);
      }
    super.paint( g);
  }
}
