package ircam.jmax.editors.frobber;

//
// The graphic outlet on the bottom of the objects.
//
public class ErmesObjOutlet extends ErmesObjInOutlet {

  public ErmesObjOutlet( int theOutletNum, ErmesObject theOwner, int x, int y)
  {
    super( theOutletNum, theOwner, x, y + theOwner.getHeight() - 2);
  }

  protected int getAnchorY()
  {
    return itsY + VISIBLE_HEIGHT;
  }

  protected int getVisibleY()
  {
    return itsY;
  }
}
