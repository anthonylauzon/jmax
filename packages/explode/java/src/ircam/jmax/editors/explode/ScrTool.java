package ircam.jmax.editors.explode;

import com.sun.java.swing.ImageIcon;

/**
 * the base class for tools. It handles the name and the icon of a the tool,
 * the interaction modules and the activation/deactivation. 
 */
abstract public class ScrTool {
  String itsName;
  ImageIcon itsIcon;
  InteractionModule currentInteractionModule;

  /**
   * constructor 
   */
  public ScrTool(String theName, ImageIcon theImageIcon) {
    setName(theName);
    setIcon(theImageIcon);
  }

  abstract public void activate();
  
  abstract public void deactivate();

  void mountIModule(InteractionModule im) {
    currentInteractionModule = im;
    currentInteractionModule.takeInteraction();
  }


  /**
   * returns its name
   */
  public String getName() {
    return itsName;
  }

  /**
   * sets its name
   */
  public void setName(String theName) {
    itsName = theName;
  }

  /**
   * sets its icon
   */
  public void setIcon(ImageIcon theIcon) {
    itsIcon = theIcon;
  }

  /**
   * gets the icon
   */
  public ImageIcon getIcon() {
    return itsIcon;
  }

}




