//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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

  ErmesToolButton(ErmesToolBar  toolBar, String description, ImageIcon theIcon, String message)
  {
    super( theIcon);

    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    this.description = description;
    this.message = message;
    this.toolBar = toolBar;

    addChangeListener(new ChangeListener()
		      {
			public void stateChanged(ChangeEvent e)
			  {
			    ErmesToolButton.this.toolBar.buttonChanged(ErmesToolButton.this);
			  }
		      });
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


