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

  /**
   * Create a FtsOutletObject object: support for .pat
   */

  FtsOutletObject(FtsContainerObject parent)
  {
    super(parent, "outlet", "outlet");

    parent.addOutlet(this); 

    FtsServer.getServer().newOutletObject(parent, this); // create the fts outlet

    ninlets = 1;
    noutlets = 0;
  }


  public FtsOutletObject(FtsContainerObject parent, int position)
  {
    super(parent, "outlet", "outlet " + position);

    this.position = position;

    parent.addOutlet(this, position);

    FtsServer.getServer().newOutletObject(parent, this, position);// create the fts inlet

    ninlets = 1;
    noutlets = 0;
  }


  public void setPosition(int i)
  {
    position = i;
    description = "outlet " + position;
    FtsServer.getServer().redefineOutletObject(this, position);
  }

  public int getPosition()
  {
    return position;
  }

  public void delete()
  {
    super.delete();

    parent.removeOutlet(this, position);
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






