package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts patch type (named patch);
 */

public class FtsPatchDataType extends MaxDataType
{
  FtsPatchDataType()
  {
    super("patch");

    FtsPatchData.setFtsPatchType(this);
  }

  public Class instanceClass()
  {
    return FtsPatchData.class;
  }

  /** Build an empty instance of FtsPatchData, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxData newInstance()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject patcher;
    Vector args = new Vector();

    args.addElement("unnamed");
    args.addElement(new Integer(0));
    args.addElement(new Integer(0));

    patcher = FtsObject.makeFtsObject(MaxApplication.getFtsServer().getRootObject(), "patcher", args);

    FtsPatchData obj = new FtsPatchData();

    obj.setPatcher(patcher);

    return obj;
  }
}
