package ircam.jmax.editors.explode;

import com.sun.java.swing.ImageIcon;

/**
 * The base class for tools: it handles the name and the icon,
 * the mounting of the interaction modules, and the activation/deactivation. 
 */
abstract public class ScrTool implements StatusBarClient{

  /**
   * constructor 
   */
  public ScrTool(String theName, ImageIcon theImageIcon) 
  {
    setName(theName);
    setIcon(theImageIcon);
  }

  /**
   * called when this tool is "mounted" on a Graphic context.
   * It switches the Graphic context and mount the default IM
   */ 
  public void reActivate(GraphicContext theGc)
  {
    gc = theGc;
    mountIModule(getDefaultIM());
  }
  
  /**
   * override to return the default (initial) Interaction module
   */
  abstract public InteractionModule getDefaultIM();


  abstract public void deactivate();

  /**
   * the method used to mount another interaction module in a tool
   */
  static void mountIModule(InteractionModule im) 
  {
    currentInteractionModule = im;
    currentInteractionModule.takeInteraction(gc);
  }


  /**
   * the method used to mount an interaction module in a tool
   * AND specify an initial point for the interaction.
   * This method is used when the IM is mounted when the interaction
   * is already begun.
   */
  static void mountIModule(InteractionModule im, int x, int y) 
  {
    mountIModule(im);
    currentInteractionModule.interactionBeginAt(x, y);
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
  static GraphicContext gc;
  static InteractionModule currentInteractionModule;
}




