package ircam.jmax.editors.explode;

/**
 * the standard, partition-like adapter to the ScrEvent records
 * in the Explode database.
 * The x-coordinates corresponds to time, y to pitch.
 * This adapter can also be used to transpose and zoom.
 */
public class PartitionAdapter extends Adapter {
  
  float xZoomFactor;
  float yZoomFactor;
  int xTranspose;
  int yTranspose;
  boolean xInvertion;
  boolean yInvertion;

  public PartitionAdapter() {
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

  public int getX(ScrEvent e) {
    int temp = super.getX(e);
    if (xInvertion) temp = -temp;
    return (int) ((temp+xTranspose)*xZoomFactor);
  }

  public int getInvX(int x) {
    int temp;

    if (xInvertion) temp = (int) (xTranspose -x/xZoomFactor);
    else temp = (int) (x/xZoomFactor - xTranspose);
    
    return temp;
  }

  public void setX(ScrEvent e, int x) {
    
    super.setX(e, getInvX(x));

  }

  public int getY(ScrEvent e) {
    
    int temp = super.getY(e);
    if (yInvertion) temp = -temp;
    return (int) ((temp+yTranspose)*yZoomFactor);
  
  }

  public int getInvY(int y) {
    int temp;

    if (yInvertion) temp = (int) (yTranspose -y/yZoomFactor);
    else temp = (int) (y/yZoomFactor - yTranspose);
  
    return temp;
  }

  public void setY(ScrEvent e, int y) {

    super.setY(e, getInvY(y));
  }

  public int getLenght(ScrEvent e) {
    return (int) (super.getLenght(e)*xZoomFactor);
  }

  public  void setLenght(ScrEvent e, int l) {
    super.setLenght(e, (int) (l*xZoomFactor));
  }

  /**
   * set the zoom factor for the x coordinates (percentage)
   */
  public void setXZoom(int factor) {
    xZoomFactor = factor/(float)100;
  }

  public float getXZoom() {
    return xZoomFactor;
  }

  public float getYZoom() {
    return yZoomFactor;
  }

  /**
   * set the zoom factor for the y coordinates (percentage)
   */
  public void setYZoom(int factor) {
    yZoomFactor = factor/100;
  }

  /**
   * set the transposition amount for the x coordinates (pixels)
   */
  public void setXTransposition(int xT) {
    xTranspose = xT;
  }

  /**
   * set the transposition amount for the y coordinates (pixels)
   */
  public void setYTransposition(int yT) {
    yTranspose = yT;
  } 

  public void setXInvertion(boolean b) {
    xInvertion = b;
  }

  public void setYInvertion(boolean b) {
    yInvertion = b;
  }
}






