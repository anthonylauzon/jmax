
package ircam.jmax.editors.explode;

import java.util.*;
import java.awt.Frame;

/**
 * the converter between Score and graphic events. 
 * It implements a mapping between the score parameters (pitch, duration...)
 * and graphic parameters (x, y, width...).
 */
abstract public class Adapter implements MappingListener{


  /**
   * get the X value for this event
   */
  public int getX(ScrEvent e) 
  {
    if (XMapper != null) return XMapper.get(e);
    else return 0;
  }


  /**
   * get the Y value for this event
   */
  public int getY(ScrEvent e) 
  {
    if (YMapper != null) return YMapper.get(e);
    else return 0;
  }


  /**
   * get the value of the parameter associated with the graphic X
   */
  abstract public int getInvX(int x);
  

  /**
   * get the value of the parameter associated with the graphic Y
   */
  abstract public int getInvY(int y);


  /**
   * set the parameter of the event associated with the graphic X
   */
  public void setX(ScrEvent e, int x) 
  {
    XMapper.set(e, x);
  }


  /**
   * set the parameter of the event associated with the Y
   */
  public void setY(ScrEvent e, int y) 
  {
    YMapper.set(e, y);
  }
  

  /**
   * get the Lenght value for this event
   */
  public int getLenght(ScrEvent e) 
  {
    if (LenghtMapper != null) return LenghtMapper.get(e);
    else return 0;
  }

  /**
   * sets the parameter of the event associated with the Lenght
   */
  public void setLenght(ScrEvent e, int lenght) 
  {
    LenghtMapper.set(e, lenght);
  }

  /**
   * get the label value (int) for this event
   */
  public int getLabel(ScrEvent e) 
  {
    if (LabelMapper != null) return LabelMapper.get(e);
    else return 0;
  }

  /**
   * sets the parameter of the event associated with the Label
   */
  public void setLabel(ScrEvent e, int lenght) 
  {
    LabelMapper.set(e, lenght);
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


  /**
   * edit this adapter (settings dialog)
   */
  public void edit(Frame theFrame) 
  {
    SettingsDialog asd = SettingsDialog.createSettingsDialog(this, theFrame);
    asd.pack();
    asd.setVisible(true);
  }


  /**
   * return the Adapter's name
   */
  public String getName() 
  {
    return itsName;
  } 

  /**
   * MappingListener interface
   */
  public void mappingChanged(String graphicName, String scoreName) {}

  //--- Fields
  Mapper XMapper;
  Mapper YMapper;
  Mapper LenghtMapper;
  Mapper LabelMapper;

  float xZoomFactor;
  float yZoomFactor;
  int xTranspose;
  int yTranspose;
  boolean xInvertion;
  boolean yInvertion;

  String itsName;
}

