package ircam.jmax.toolkit;

/**
 * The interface of the objects that needs to be informed 
 * when a position have been choosen by the user.
 * @see MouseTracker
 */
public interface PositionListener {
  /**
   * A position have been choosen.
   * This callback communicates also the modifier keys
   * pressed at the moment of the choice.
   */
  public abstract void positionChoosen(int x, int y, int modifier);
}
