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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.toolkit.*;

public class Standard extends Editable implements FtsObjectErrorListener
{
  private String varName = null;
  private int varWidth = 0;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public Standard( FtsGraphicObject theFtsObject) 
  {
    super( theFtsObject);
  }
    
  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  public String getArgs()
  {
    return ftsObject.getDescription();
  }
    
  public void errorChanged(boolean value) 
  {
    redraw();
  }
    
  /* Inspector */
    
  public void inspect() 
  {
  }

  public void redefine( String text) 
  {
    ((FtsPatcherObject)ftsObject.getParent()).requestRedefineObject(ftsObject, text);    
  }

  public void editContent()
  {
    if( !ftsObject.isError() && ( ftsObject instanceof FtsObjectWithEditor))
      {
	itsSketchPad.waiting();

	if(ftsObject instanceof FtsPatcherObject)
	   ((FtsPatcherObject)ftsObject).requestSubPatcherUpload();
	else
	  ((FtsObjectWithEditor)ftsObject).requestOpenEditor();

	((FtsPatcherObject)ftsObject.getParent()).requestStopWaiting(null);
      }
  }

  public boolean hasContent()
  {
    return true;
  }

  public void setCurrentName( String name)
  {
    if( varName == null)
      {
	varName = name;	
	varWidth = getFontMetrics().stringWidth( varName) + 6;
	setWidth( getWidth());
      }
    else
      if( name.equals(""))
	{
	  varName = null;
	  int oldw = getWidth() - varWidth;	  
	  varWidth = 0;
	  setWidth( oldw);
	}
      else
	if( !varName.equals( name))
	  {
	    varName = name;
	    int oldw = getWidth() - varWidth;
	    varWidth = getFontMetrics().stringWidth( varName) + 6;
	    setWidth( oldw + varWidth);
	  }
    redraw();
  }

  public void setFont( Font theFont)
  {
    super.setFont( theFont);
    if( varName != null)
      {
	int oldw = getWidth() - varWidth;
	varWidth = getFontMetrics().stringWidth( varName) + 6;
	setWidth( oldw + varWidth);
      }
  }

  public void setCurrentFont( Font theFont)
  {
    super.setCurrentFont( theFont);
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

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 3; /*was 4 */
  private static final int TEXT_Y_OFFSET = 2;

  public int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return 5;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    if (ftsObject.isError())
      return Color.gray;
    else
      return Color.black;
  }

  public Color getTextBackground()
  {
    if (isSelected())
	if (ftsObject.isError())
	    return Color.lightGray;
	else
	    return Settings.sharedInstance().getObjColor();
    else	
	if(isEditing())
	    return Settings.sharedInstance().getEditBackgroundColor();
	else
	    return Color.white;	    
  }

  public boolean isMultiline()
  {
      return true;
  }
 
  // ----------------------------------------
  // Paint stuff
  // ----------------------------------------

  public void paint(Graphics g) 
  {
    if (ftsObject.isError())
      {
	if (isSelected())
	  g.setColor( Color.gray);
	else
	  g.setColor( Color.lightGray);
      }
    else
      {
	  if (isSelected())
	      g.setColor( Settings.sharedInstance().getObjColor().darker());
	  else
	      g.setColor( Settings.sharedInstance().getObjColor());
      }

    int x = getX();
    int y = getY();
    int w = getWidth() - varWidth;
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);

    drawContent( g);

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

  /**************  popup interaction ********************/ 
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    getControlPanel().update(this);
    ObjectPopUp.getInstance().add((JPanel)getControlPanel());
    ObjectPopUp.getInstance().revalidate();
    ObjectPopUp.getInstance().pack();
  }
  public void popUpReset()
  {
    super.popUpReset();
    getControlPanel().done();
    ObjectPopUp.getInstance().remove((JPanel)getControlPanel());
  }  
  public ObjectControlPanel getControlPanel()
  {
    return this.controlPanel;
  }
  public static StandardControlPanel controlPanel = new StandardControlPanel();
  Color varColor = new Color( 153, 204, 204, 100);
  Color selVarColor = new Color( 107, 142, 142, 100);
}


