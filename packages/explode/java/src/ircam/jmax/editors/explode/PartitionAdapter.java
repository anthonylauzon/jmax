package ircam.jmax.editors.explode;

/**
 * the standard, partition-like adapter to the ScrEvent records
 * in the Explode database.
 * The x-coordinates corresponds to time, y to pitch.
 */
public class PartitionAdapter extends Adapter {
  
  float xZoomFactor;
  float yZoomFactor;
  
  public PartitionAdapter() {
    XMapper = new TimeMapper();
    YMapper = new PitchMapper();
    xZoomFactor = 1;
    yZoomFactor = 1;
  }

  public int getX(ScrEvent e) {
    return (int) (super.getX(e)*xZoomFactor);
  }

  public int getY(ScrEvent e) {
    return (int) (super.getY(e)*yZoomFactor);
  }

  /**
   * set the zoom factor for the x coordinates (percentage)
   */
  public void setXZoom(int factor) {
    xZoomFactor = factor/(float)100;
  }

  /**
   * set the zoom factor for the y coordinates (percentage)
   */
  public void setYZoom(int factor) {
    yZoomFactor = factor/100;
  }
}


