
package ircam.jmax.toolkit;

/**
 * The interface associated to the DirectionChooser interface module
 */
public interface DirectionListener {

  /** The user dragged the mouse in the resulting direction 
   */
  public abstract void directionChoosen(int theDirection);

  /** the user aborted the process (mouseReleased)
   */
  public abstract void directionAbort();

}


