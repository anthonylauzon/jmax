/* associated to this, there should be a
   DoubleBuffer interface, with the methods
   - lostOwnership
   - drawOffScreen
*/

/* how to use this class: 
 * Suppose to share the same OffscreenManager between several object 
 of the same CLASS (it is not mandatory, but more efficient.
 * A component that wants to share such a doubleBuffer, just subscribe.
 * When it needs the offScreen, it calls requestOffScreen. From now on, 
 * the component can draw into the offGraphics, and use it for alternating
 * it with the "inGraphics". 
 * If someone else requests the offScreen, the old owner is prevented via the
 * lostOwnership call.*/



/**
 * a class that handles an offscreen shared between different components
 */


class OffScreenManager {
  Image OffImage;
  Graphics offGraphics;
  Component currentOwner;
  Component previousOwner;
  boolean OwnerAlreadyDrawn;

  public OffScreenManager() {
    //initialize the image and everything
  }
  
  /**
   * Ask for the ownership of the offscreen.
   * This function returns true if the offScreen can be assigned to the caller.
   */
  public boolean requestOffScreen(Component c) {
    //we should implement an assignement politic here, like:
    // Give the offscreen only if the dimensions are the same (or less)...
    previousOwner = currentOwner;
    currentOwner = c;
    //realloc the offscreen if littler?
  }

  /**
   * Enter in the pool of components that share the same offScreen
   */
  public boolean subscribe(Component c) {
    //eh eh.. vedere se le cose sono possibili!
  }

  /**
   * Used by a double-buffered component when it should draw its content.
   */
  public doubleBuffer(Component c) {
    /* if the component has the ownership,
       just copy the offscreen into the c.getGraphics().
       Otherwise, ask the component to paint in the offScreen, 
       and then copy the result.
       */
  }
  
  public void copyInTheOffScreen(Component c, Rectangle r) {
    /* fill the offScreen with the content of the component's
       visible area*/
  }

  public Graphics getTheOffGraphics(Component c) {
    /*test the ownership*/
  }

}
