package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsOutletObject  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int position;

  public FtsOutletObject(FtsContainerObject parent, int position, int objId)
  {
    super(parent, "outlet", "outlet " + position, objId);

    this.position = position;

    ninlets = 1;
    noutlets = 0;
  }


  public void setPosition(int i)
  {
    position = i;
    description = "outlet " + position;
    Fts.getServer().repositionOutletObject(this, position);
    setDirty();
  }

  public int getPosition()
  {
    return position;
  }

  /** Get the number of outlets of the object 
    Overwrite the FtsObject method because inlets
    do not believe to FTS, when it say outlets have
    outlets.
    */

  public int getNumberOfOutlets()
  {
    return 0;
  }


  /** Save the object to a TCL stream. */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "inlet ..."

    writer.print("outlet " + position);

    savePropertiesAsTcl(writer);
  }
}






