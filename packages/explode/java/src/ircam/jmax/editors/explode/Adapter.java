
package ircam.jmax.editors.explode;

import java.util.*;
/**
 * the converter between Score and graphic events. 
 * It implements a mapping between the score parameters (pitch, duration...)
 * and graphic parameters (x, y, width...)
 */
abstract public class Adapter {

  Mapper XMapper;
  Mapper YMapper;
  Mapper LenghtMapper;

  public int getX(ScrEvent e) {
    if (XMapper != null) return XMapper.get(e);
    else return 0;
  }

  public int getY(ScrEvent e) {
    if (YMapper != null) return YMapper.get(e);
    else return 0;
  }

  abstract public int getInvX(int x);
  abstract public int getInvY(int y);

  public void setX(ScrEvent e, int x) {
    XMapper.set(e, x);
  }

  public void setY(ScrEvent e, int y) {
    YMapper.set(e, y);
  }

  public int getLenght(ScrEvent e) {
    if (LenghtMapper != null) return LenghtMapper.get(e);
    else return 0;
  }

  public void setLenght(ScrEvent e, int lenght) {
    LenghtMapper.set(e, lenght);
  }


}

