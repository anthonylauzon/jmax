package ircam.jmax.toolkit;

import javax.swing.ImageIcon;

/**
 * The base class for tools: it handles the name and the icon,
 * the mounting of the interaction modules, and the activation/deactivation. 
 */
//impl. note: try to merge this class with the swing concept of Action
abstract public class Tool implements StatusBarClient{

  /**
   * constructor 
   */
  public Tool(String theName, ImageIcon theImageIcon) 
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

  /**
   * called when this object is unmounted */
  public void deactivate() 
  {
  }

  /**
   * the method used to mount another interaction module in a tool
   */
  static public void mountIModule(InteractionModule im) 
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
  static public void mountIModule(InteractionModule im, int x, int y) 
  {
    //ehy!
    /*currentInteractionModule*/im.interactionBeginAt(x, y);
    mountIModule(im);

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
  protected static GraphicContext gc;
  static InteractionModule currentInteractionModule;
}




