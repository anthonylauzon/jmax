
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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

class ToolButton extends JToggleButton
{
  private String description;
  private String message;
  private ToolBar toolBar;
  private boolean state = false;
  private Cursor cursor;

  static MouseListener mListener = new MouseAdapter() {
    public void mousePressed(MouseEvent e)
      {
	ToolButton button = (ToolButton)e.getSource();
	if ( !button.isSelected() )
	  {
	    button.state = true;
	    button.setSelected( true);
	    button.toolBar.buttonSelected( button);
	    button.setCursor( button.getButtonCursor());
	    button.toolBar.getSketchPad().setCursor(button.getButtonCursor());
	  }
      } 
    };

  ToolButton( ToolBar toolBar, String name)
  {
    super( JMaxClassMap.getIcon( name));

    this.cursor = JMaxClassMap.getCursor( name);
    if(!name.equals("standard"))
      this.description = name;
    else
      this.description = "";

    this.message = "";
    this.toolBar = toolBar;

    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    setToolTipText( name + " object");

    addMouseListener(ToolButton.mListener);
  }

  void reset()
  {
    state = false;
    setCursor( Cursor.getDefaultCursor());
    toolBar.getSketchPad().setCursor( Cursor.getDefaultCursor());
  }

  String getDescription()
  {
    return description;
  }

  String getMessage()
  {
    return message;
  }

  Cursor getButtonCursor()
  {
    return cursor;
  }
}







