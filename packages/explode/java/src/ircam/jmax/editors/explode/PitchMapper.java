package ircam.jmax.editors.explode;

public class PitchMapper extends Mapper {
  
  public void set(ScrEvent e, int val) {

    /* snapping maybe should not be here */
    val = (val/ScoreRenderer.YINTERVAL)*ScoreRenderer.YINTERVAL;
    e.setPitch(val);
  }

  public int get(ScrEvent e) {
    return e.getPitch();
  }
}




