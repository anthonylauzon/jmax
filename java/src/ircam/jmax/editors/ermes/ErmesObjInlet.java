package ircam.jmax.editors.ermes;

//
// The graphic inlet that appears on the top af the objects.
//
class ErmesObjInlet extends ErmesObjInOutlet {

  ErmesObjInlet( int theInletNum, ErmesObject theOwner, int x, int y) 
  {
    super( theInletNum, theOwner, x, y - SENSIBLE_HEIGHT);
  }

  protected int getAnchorY()
  {
    return itsY + (SENSIBLE_HEIGHT - VISIBLE_HEIGHT) + 1;
  }

  protected int getVisibleY()
  {
    return getAnchorY() + 1;
  }
}
