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

package ircam.jmax.editors.patcher;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

class ErmesToolButton extends JToggleButton 
{
  String description;
  String message;
  ErmesToolBar  toolBar;
  boolean state = false;

  static ChangeListener listener = new ChangeListener(){
    public void stateChanged(ChangeEvent e)
    {
      ErmesToolButton button = (ErmesToolButton)e.getSource();

      if ( button.state != button.isSelected() )
      { 
	button.state = ! button.state;
	button.toolBar.buttonChanged( button);
      }
      else
      button.setSelected(!button.state);
    }
  };

  ErmesToolButton(ErmesToolBar  toolBar, String description, ImageIcon theIcon, String message)
  {
    super( theIcon);

    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    this.description = description;
    this.message = message;
    this.toolBar = toolBar;

    addChangeListener(ErmesToolButton.listener);    
  }

  String getDescription()
  {
    return description;
  }

  String getMessage()
  {
    return message;
  }
}







