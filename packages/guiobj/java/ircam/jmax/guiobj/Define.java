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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
//import javax.swing.*;
import javax.swing.SwingUtilities;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.toolkit.*;

public class Define extends Editable implements FtsObjectErrorListener{

  private static final int DEFAULT_WIDTH = 44;
  private static final int MINIMUM_WIDTH = 20;

  protected FontMetrics boldFontMetrics = null;
  protected Font boldFont = null;

  int typeWidth = 0;

  private String varName = null;
  private int varWidth = 0;

  public Define( FtsDefineObject theFtsObject) 
  {
    super( theFtsObject);
    setDefaultWidth(MINIMUM_WIDTH);
        
    boldFont = getFont().deriveFont( Font.BOLD | Font.ITALIC);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
    
    typeWidth = boldFontMetrics.stringWidth( theFtsObject.getType());
  }

  public boolean isResizable()
  {
    return false;
  }

  public void setDefaults()
  {
    setWidth( getWidth());
  }

  public void setType( String type)
  {
    ((FtsDefineObject)ftsObject).requestSetType( type);
    typeWidth = boldFontMetrics.stringWidth( type);
    redraw();
  }
  
  public int getTypeWidth()
  {
    return typeWidth;
  }

  public String getArgs()
  {
    return ((FtsDefineObject)ftsObject).getExpression();
  }

  public void setCurrentName( String name)
  {
    if( name.equals(""))
      {
	varName = "";
	varWidth = 0;
      }
    else
      {
	varName = name;	
	varWidth = getFontMetrics().stringWidth( varName) + 6;
      }

    redraw();
  }

  public String getName()
  {
    return varName;
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

  public void setWidthToText(String text)
  {
    int space;
    if( text.equals(""))
      space = getFontMetrics().stringWidth("    ");
    else
      space = getFontMetrics().stringWidth(" ");
    
    int tw =  getTextHeightOffset() + getTypeWidth() + space + getFontMetrics().stringWidth( text) + 6 + getVariableWidth();

    if( getWidth() != tw)
      forceWidth( tw);
  }

  public void setFont( Font theFont)
  {
    super.forceFont( theFont);
    boldFont = getFont().deriveFont( Font.BOLD | Font.ITALIC);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
    
    if( varName != null && !varName.equals(""))
    {
      int oldw = getWidth() - varWidth;
      varWidth = getFontMetrics().stringWidth( varName) + 6;
    }
    
    typeWidth = boldFontMetrics.stringWidth( ((FtsDefineObject)ftsObject).getType());
    
    forceWidth( getWidth());
    forceHeight( getFontMetrics().getHeight() + getTextHeightOffset());
    fitToText(); 
    redraw();
  }

  public void setCurrentFont( Font font)
  {
    super.setCurrentFont( font);
    boldFont = font.deriveFont( Font.BOLD | Font.ITALIC);
    boldFontMetrics = itsSketchPad.getFontMetrics( boldFont);
    
    typeWidth = boldFontMetrics.stringWidth( ((FtsDefineObject)ftsObject).getType());
    
    if( varName != null)
    {
      int oldw = getWidth() - varWidth;
      varWidth = getFontMetrics().stringWidth( varName) + 6;
      setWidth( oldw + varWidth);
    }
  }

  public int getVariableWidth()
  {
    return varWidth;
  }

  public void redefine( String text) 
  {
    ((FtsDefineObject)ftsObject).requestSetExpression( text);
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

  public void typeChanged(String type) 
  {
    typeWidth = boldFontMetrics.stringWidth( type);
    fitToText();
    redraw();
  } 

  public void expressionChanged(String expr) 
  {
    fitToText();
    redraw();
  } 
    
  public void fitToText()
  {
    setWidthToText( ((FtsDefineObject)ftsObject).getExpression());
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
    return getTextHeightOffset() + getTypeWidth() + 4;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return  getTextHeightOffset() + getTypeWidth() + 6;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    if( !ftsObject.isError())
      return Color.black;
    else
      {
	if(isSelected())
	  return Color.gray.darker();
	else
	  return Color.gray;
      }
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

  public ObjectControlPanel getControlPanel()
  {
    return new DefineControlPanel( this);
  }

  public boolean isInspectable()
  {
    return true;
  }
  
  public String getType()
  {
    return ((FtsDefineObject)ftsObject).getType();
  }

  public boolean instantEdit()
  {
    String expr = ((FtsDefineObject)ftsObject).getExpression().trim();
    return expr.equals("");
  }  
  
  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth() - varWidth;
    int h = getHeight();

    g.setColor( getTextBackground());

    g.fillRect( x + 1, y + 1, w - 2,  h - 2);
    g.fill3DRect( x + 1, y + 1, getTextXOffset() - 2,  h-2, true);

    g.setColor( getTextForeground());

    int bottom = getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2;
    g.setFont( boldFont);
    g.drawString( ((FtsDefineObject)ftsObject).getType(), x + 4, y + bottom);
        
     if( !isEditing())
      {       
	g.setFont( getFont());
	g.drawString( ((FtsDefineObject)ftsObject).getExpression(), x + getTextXOffset(), y + bottom);
      }
    
        /* draw variable name */

    if( varName!= null)
      {	  
        if( isSelected())
          g.setColor( selVarColor);
        else
          g.setColor( varColor);

	g.fillRect( x+w, y+1, varWidth-1, h-2);

        if( isSelected())
          g.setColor( Settings.sharedInstance().getObjColor().darker());
        else
          g.setColor( Settings.sharedInstance().getObjColor());

	g.drawLine( x+w-2, y+1, x+w-2, y+h-2);
	g.setColor( Color.black);
	g.drawLine( x+w-1, y+1, x+w-1, y+h-2);
	g.setFont( getFont());
	g.drawString( varName, x+w+2, y + getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2);
      }
    
    super.paint( g);
  }
  
  Color varColor = new Color( 153, 204, 204, 100);
  Color selVarColor = new Color( 107, 142, 142, 100);
}

