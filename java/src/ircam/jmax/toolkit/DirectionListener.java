
package ircam.jmax.toolkit;

/**
 * The interface associated to the DirectionChooser interface module.
 * Used by objects that need to be informed on the direction choosed
 * by the user with the mouse.
 */
public interface DirectionListener {

  /** The user dragged the mouse in the resulting direction 
   */
  public abstract void directionChoosen(int theDirection);

  /** the user aborted the process (mouseReleased)
   */
  public abstract void directionAbort();

  //--- Fields
  public static final int HORIZONTAL_MOVEMENT = DirectionChooser.HORIZONTAL_MOVEMENT;
  public static final int VERTICAL_MOVEMENT = DirectionChooser.VERTICAL_MOVEMENT;
}


