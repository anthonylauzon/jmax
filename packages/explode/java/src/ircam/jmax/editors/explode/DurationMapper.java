package ircam.jmax.editors.explode;

/**
 * A simple Mapper that accesses the duration parameter
 */ 
public class DurationMapper extends Mapper {
  
  /**
   * set the duration of this event
   */
  public void set(ScrEvent e, int value) 
  {
    e.setDuration(value);
  }

  /**
   * get the duration of this event
   */
  public int get(ScrEvent e) 
  {
    return e.getDuration();
  }
}

