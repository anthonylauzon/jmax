package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsInletObject  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int position;

  /**
   * Create a FtsInletObject object.
   */
  
  public FtsInletObject(FtsContainerObject parent, int position, int objId)
  {
    super(parent, "inlet", "inlet " + position, objId);

    this.position = position;

    ninlets = 0;
    noutlets = 1;
  }

  public void setPosition(int i)
  {
    position = i;
    description = "inlet " + position;

    Fts.getServer().repositionInletObject(this, position);
    setDirty();
  }

  public int getPosition()
  {
    return position;
  }

  /** Get the number of inlets of the object 
    Overwrite the FtsObject method because inlets
    do not believe to FTS, when it say inlets have
    inlets.
   */

  public int getNumberOfInlets()
  {
    return 0;
  }

  /** Save the object to a TCL stream. */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "inlet ..."

    writer.print("inlet " + position);

    savePropertiesAsTcl(writer);
  }
}






