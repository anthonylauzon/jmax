package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;

class ErmesSwToggleButton extends JToggleButton {

  Class itsClass;

  ErmesSwToggleButton(Class theClass, ImageIcon theIcon) {
    super(theIcon);
    setDoubleBuffered(false);
    itsClass = theClass;
  }
  
  Class getNewObjectClass() {
    return itsClass;
  }
}





