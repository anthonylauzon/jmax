package ircam.jmax.mda;

import java.io.*;

import ircam.jmax.fts.*;

/** This interface define the minimal requirements on 
 * a MaxData instance in order to save and store this instance
 * in an Fts Object; this also a way to check if an instance can
 * be set to fts.
 * A MaxFtsData usually mirrow an FTS Data object of some kind.
 * the keepSynced flag is used to keep the mirrowing "live".
 * 
 * You can load a MaxFtsData in any supported way (for example,
 * using a tcl file if the data implements the MaxTclData interface)
 * and then assign to it a MaxFtsDataSource, and with a save you have loaded
 * the new data set from the editor; same thing for saving a data set to a file.
 * 
 *These functions have no argument; the fts object to save to/from must be found
 * out thru the source.
 */


public interface MaxFtsData 
{
  /** One shot save function */

  abstract public void saveToFtsObject() throws MaxDataException;

  /** One shot load function */

  abstract public void loadFromFtsObject() throws MaxDataException;
}










