//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

class ErmesSwToggleButton extends JToggleButton
{
  String itsObjectDescription;

  ErmesSwToggleButton(String description, ImageIcon theIcon)
  {
    super( theIcon);
    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    itsObjectDescription = description;
  }
    
  String getNewObjectDescription()
  {
    return itsObjectDescription;
  }
}
