
package ircam.jmax.toolkit;

import java.awt.datatransfer.*;

/**
 * The interface of the data (models) that supports clipboard operations */
public interface ClipableData {

  /**
   * ask the model to cut the selected data */
  public abstract void cut();

  /**
   * ask the model to copy the selected elements */
  public abstract void copy();

  /**
   * ask the model to paste the content of the clipboard */
  public abstract void paste();

}

