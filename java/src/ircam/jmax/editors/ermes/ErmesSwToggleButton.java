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
