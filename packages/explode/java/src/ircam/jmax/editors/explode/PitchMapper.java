package ircam.jmax.editors.explode;

/**
 * a mapper into the pitch value of the ScrEvents
 */
public class PitchMapper extends Mapper {
  
  /**
   * set the given pitch in the given event
   */
  public void set(ScrEvent e, int val) 
  {
    e.setPitch(val);
  }


  /**
   * get the pitch from the given event
   */
  public int get(ScrEvent e) 
  {
    return e.getPitch();
  }


  /**
   * access the static instance of the class */
  static Mapper getMapper() 
  {
    return itsPitchMapper;
  }

  //-- Fields

  static PitchMapper itsPitchMapper = new PitchMapper();
}




