package ircam.jmax.editors.explode;

/**
 * the standard, partition-like adapter for the ScrEvent records
 * in the Explode database.
 * The x-coordinates corresponds to time, y to pitch, lenght to duration.
 * This adapter can also be used to transpose and zoom.
 */
public class PartitionAdapter extends Adapter {
  
  /**
   * constructor.
   * It creates and assigns its mappers (Time, Pitch, Lenght), 
   * and set the initial values for the zoom, transpose and invertion fields.
   */
  public PartitionAdapter() 
  {
    XMapper = new TimeMapper();
    YMapper = new PitchMapper();
    LenghtMapper = new DurationMapper();
    xZoomFactor = 1;
    yZoomFactor = 1;
    xTranspose = 0;
    yTranspose = 0;
    xInvertion = false;
    yInvertion = false;
  }


  /**
   * inherited from Adapter.
   * it returns the X value of the event,
   * making the needed cordinate conversions.
   */
  public int getX(ScrEvent e) 
  {
    int temp = super.getX(e);
    
    if (xInvertion) temp = -temp;

    return (int) ((temp+xTranspose)*xZoomFactor);
  }


  /**
   * inherited from Adapter.
   * Returns the time associated with the value of an X coordinate after
   * the coordinate conversion.
   */
  public int getInvX(int x) 
  {
    int temp;

    if (xInvertion) temp = (int) (xTranspose -x/xZoomFactor);
    else temp = (int) (x/xZoomFactor - xTranspose);
    
    return temp;
  }


  /**
   * set the time of the event associated with the graphic X
   */
  public void setX(ScrEvent e, int x) 
  {  
    super.setX(e, getInvX(x));
  }


  /**
   * inherited from Adapter.
   * it returns the Y value of the event,
   * making the needed cordinate conversions.
   */
  public int getY(ScrEvent e) 
  {  
    int temp = super.getY(e);
    
    if (yInvertion) temp = -temp;
    
    return (int) ((temp+yTranspose)*yZoomFactor);  
  }


  /**
   * inherited from Adapter.
   * Returns the pitch associated with the value of an Y coordinate
   */  
  public int getInvY(int y) 
  {
    int temp;

    if (yInvertion) temp = (int) (yTranspose -y/yZoomFactor);
    else temp = (int) (y/yZoomFactor - yTranspose);
  
    return temp;
  }


  /**
   * set the pitch of the event associated with the graphic y
   */
  public void setY(ScrEvent e, int y) 
  {
    super.setY(e, getInvY(y));
  }


  /**
   * inherited from Adapter.
   * returns the lenght value of the event,
   * making the needed cordinate conversions (zooming).
   */
  public int getLenght(ScrEvent e) 
  {
    return (int) (super.getLenght(e)*xZoomFactor);
  }


  /**
   * set the duration of the event associated with the graphic lenght l.
   */
  public  void setLenght(ScrEvent e, int l) 
  {
    super.setLenght(e, (int) (l*xZoomFactor));
  }


  /**
   * set the zoom factor for the x coordinates (percentage)
   */
  public void setXZoom(int factor) 
  {
    xZoomFactor = factor/(float)100;
  }


  /**
   * get the zoom factor for the x coordinates (percentage)
   */
  public float getXZoom() 
  {
    return xZoomFactor;
  }

  /**
   * get the zoom factor for the y coordinates (percentage)
   */
  public float getYZoom() 
  {
    return yZoomFactor;
  }

  /**
   * set the zoom factor for the y coordinates (percentage)
   */
  public void setYZoom(int factor) 
  {
    yZoomFactor = factor/100;
  }


  /**
   * set the transposition amount for the x coordinates (pixels)
   */
  public void setXTransposition(int xT) 
  {
    xTranspose = xT;
  }

  /**
   * set the transposition amount for the y coordinates (pixels)
   */
  public void setYTransposition(int yT) 
  {
    yTranspose = yT;
  } 


  /**
   * set the inversion flag for the x coordinates
   */
  public void setXInvertion(boolean b) 
  {
    xInvertion = b;
  }


  /**
   * set the inversion flag for the y coordinates
   */
  public void setYInvertion(boolean b) 
  {
    yInvertion = b;
  }

  //------------- Fields
  float xZoomFactor;
  float yZoomFactor;
  int xTranspose;
  int yTranspose;
  boolean xInvertion;
  boolean yInvertion;


}






