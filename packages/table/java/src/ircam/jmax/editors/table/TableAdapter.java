
package ircam.jmax.editors.table;

/**
 * A simple coordinates converter in a Table window. 
 * It handles the conversion of x, y coordinates, an origin, and two zoom factors.
 */
public class TableAdapter {


  public TableAdapter()
  {
  }

  /**
   * returns the graphic x corresponding to the index */
  public int getX(int index)
  {
    return (int) ((index-oX) * itsXZoom);
  }

  /**
   * returns the index in the table corresponding to the graphic x */
  public int getInvX(int x)
  {
    return (int) (x/itsXZoom+oX);
  }

  /**
   * returns the graphic y corresponding to the value.
   * The y coordinates are implicitily inverted. */
  public int getY(int value)
  {
    if (value < 0);

    return (int) (oY-value * itsYZoom);
  }

  /**
   * returns the value corresponding to the graphic y */ 
  public int getInvY(int y)
  {
    return (int) ((oY-y)/itsYZoom);
  }

  //--- Fields & accessors ---//
  float itsXZoom;
  float itsYZoom;
  /** the first index in the table we're showing */
  int oX; 
  /** the graphic x offset of oX */
  int xOffset;
  /** the first value in the table we're vertically showing */
  int oY;
  /** the graphic y offset of oY */
  int yOffset;

  public float getXZoom()
  {
    return itsXZoom;
  }

  public void setXZoom(float zoom)
  {
    itsXZoom = zoom;
  }

  public float getYZoom()
  {
    return itsYZoom;
  }

  public void setYZoom(float zoom)
  {
    itsYZoom = zoom;
  }

  public int getOY()
  {
    return oY;
  }

  public void setOY(int theOY)
  {
    oY = theOY;
  }

  public int getOX()
  {
    return oY;
  }

  public void setOX(int theOY)
  {
    oY = theOY;
  }

  public int getYOffset()
  {
    return yOffset;
  }

  public void setYOffset(int theOY)
  {
    yOffset = theOY;
  }

  public int getXOffset()
  {
    return xOffset;
  }

  public void setXOffset(int theOX)
  {
    xOffset = theOX;
  }

}



