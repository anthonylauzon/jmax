package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;

class ErmesSwToggleButton extends JToggleButton
{
  String itsObjectDescription;

  ErmesSwToggleButton(String description, ImageIcon theIcon) {
    super(theIcon);
    setDoubleBuffered(false);
    itsObjectDescription = description;
  }
    
  String getNewObjectDescription() {
    return itsObjectDescription;
  }
}





