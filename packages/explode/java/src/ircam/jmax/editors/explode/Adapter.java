
package ircam.jmax.editors.explode;

import java.util.*;

/**
 * the converter between Score and graphic events. 
 * It implements a mapping between the score parameters (pitch, duration...)
 * and graphic parameters (x, y, width...)
 */
abstract public class Adapter {


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


  //--- Fields
  Mapper XMapper;
  Mapper YMapper;
  Mapper LenghtMapper;

}

