package ircam.jmax.editors.explode;

public class PitchMapper extends Mapper {
  
  public void set(ScrEvent e, int val) {

    e.setPitch(val);
  }

  public int get(ScrEvent e) {
    return e.getPitch();
  }
}




