package ircam.jmax.editors.explode;

public class TimeMapper extends Mapper {
  
  public void set(ScrEvent e, int value) {
    e.setTime(value);
  }

  public int get(ScrEvent e) {
    return e.getTime();
  }
}
