package ircam.jmax.editors.table;

/**
 * The interface used to communicate between the LinerTool and its
 * LineDrawer interface module.
 */
public interface LineListener {

  public abstract void lineStart(int x, int y, int modifiers);
  public abstract void lineEnd(int x, int y);
}
