package ircam.jmax.fts;

import ircam.jmax.mda.*;

/** FtsObject Objects having data to be edited as MaxData should
  implement this interface.  Note that the method defined by this
  interface should not be used by the editor, that should instead use
  the standard MDA method (MaxData.makeInstance), passing a MaxDataSource
  built either from a FtsLocation or a name.
  FtsLocation can be built from a name or directly from an object.
  */

public interface FtsDataObject
{
  abstract  MaxDataType getObjectDataType();
}
  
