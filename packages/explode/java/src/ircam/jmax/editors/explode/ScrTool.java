package ircam.jmax.editors.explode;

import com.sun.java.swing.ImageIcon;

/**
 * The base class for tools: it handles the name and the icon,
 * the mounting of the interaction modules, and the activation/deactivation. 
 */
abstract public class ScrTool {

  /**
   * constructor 
   */
  public ScrTool(String theName, ImageIcon theImageIcon) 
  {
    setName(theName);
    setIcon(theImageIcon);
  }

  abstract public void activate();
  
  abstract public void deactivate();

  void mountIModule(InteractionModule im) 
  {
    currentInteractionModule = im;
    currentInteractionModule.takeInteraction();
  }


  /**
   * returns its name
   */
  public String getName() 
  {
    return itsName;
  }

  /**
   * sets its name
   */
  public void setName(String theName) 
  {
    itsName = theName;
  }


  /**
   * sets its icon
   */
  public void setIcon(ImageIcon theIcon) 
  {
    itsIcon = theIcon;
  }


  /**
   * gets the icon
   */
  public ImageIcon getIcon() 
  {
    return itsIcon;
  }

  //---- Fields
  String itsName;
  ImageIcon itsIcon;
  InteractionModule currentInteractionModule;
}




