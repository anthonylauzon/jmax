package ircam.jmax.editors.explode;

/**
 * The base class of mappings between score parameters and graphic
 * parameters
 */
public abstract class Mapper {
  public abstract void set(ScrEvent e, int val);
  public abstract int get(ScrEvent e);
}
