package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * A void object is invisible and is not saved; is used
 * for .pat objects that are saved empty;
 */

public class FtsVoidObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  public FtsVoidObject(FtsContainerObject parent)
  {
    super(parent, "void", "void");
  }

  /** This object is not persistent */

  protected boolean isPersistent()
  {
    return false;
  }

  /** This object is not visible */

  public boolean isRepresented()
  {
    return false;
  }

  /** This object is never saved */

  public void saveAsTcl(PrintWriter writer)
  {
    // Never saved !!!
  }
}






