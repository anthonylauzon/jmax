package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts group type (named patch);
 * Added by enzo.
 */

public class FtsGroupDataType extends MaxDataType
{
  public FtsGroupDataType()
  {
    super("group");
  }

  /** Build an empty instance of FtsGroupData, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxData newInstance()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    Vector aVector = new Vector();

    FtsGroupData obj = new FtsGroupData();
    
    obj.setGroup(aVector);
    
    return obj;
  }
}




