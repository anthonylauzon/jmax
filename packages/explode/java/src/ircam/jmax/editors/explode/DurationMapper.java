package ircam.jmax.editors.explode;

public class DurationMapper extends Mapper {
  
  public void set(ScrEvent e, int value) {
    e.setDuration(value);
  }

  public int get(ScrEvent e) {
    return e.getDuration();
  }
}
