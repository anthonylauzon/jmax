package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;

class ErmesSwToggleButton extends JToggleButton {

  String itsName;//the name with which it is known (a *property*)

  public ErmesSwToggleButton(String theName, ImageIcon theIcon) {
    super(theIcon);
    setDoubleBuffered(false);
    itsName = theName;
  }
  
  public String getName() {
    return itsName;
  }

  public void setName(String theName) {
    itsName = theName;
  }  
}





