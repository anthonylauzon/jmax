
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

  public int getX(ScrEvent e) {
    if (XMapper != null) return XMapper.get(e);
    else return 0;
  }

  public int getY(ScrEvent e) {
    if (YMapper != null) return YMapper.get(e);
    else return 0;
  }
}

