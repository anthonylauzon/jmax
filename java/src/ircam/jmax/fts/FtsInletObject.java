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

  FtsInletObject(FtsContainerObject parent)
  {
    super(parent, "inlet", "inlet");

    parent.addInlet(this); // support for .pat

    FtsServer.getServer().newInletObject(parent, this); // create the fts inlet

    ninlets = 0;
    noutlets = 1;
  }

  
  public FtsInletObject(FtsContainerObject parent, int position)
  {
    super(parent, "inlet", "inlet " + position);

    this.position = position;

    parent.addInlet(this, position);

    FtsServer.getServer().newInletObject(parent, this, position);// create the fts inlet

    ninlets = 0;
    noutlets = 1;
  }

  public void setPosition(int i)
  {
    position = i;
    description = "inlet " + position;

    FtsServer.getServer().redefineInletObject(this, position);
  }

  public int getPosition()
  {
    return position;
  }

  public void delete()
  {
    super.delete();

    parent.removeInlet(this, position);
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






