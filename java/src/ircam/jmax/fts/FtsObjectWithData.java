package ircam.jmax.fts;

import ircam.jmax.mda.*;

/**
 *Objects having data that can be edited (FtsObjectData) should
 * implement this interface. 
 * Editors can use this interface to find out the data
 * associated with an FTS Object
 */

public interface FtsObjectWithData
{
  abstract MaxData getData();
  abstract void setData(MaxData data) throws FtsException;
}
  




