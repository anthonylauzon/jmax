package ircam.jmax;

/** An interface used to declare a tool;
  A tool finder know the name of a tool, and now
  how to open the tool.
  */

public interface MaxToolFinder
{
  /** Return the tool name, for UI purposes */

  public abstract String getToolName();

  /** Open the tool name */

  public abstract void open();
}
