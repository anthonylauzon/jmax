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

    MaxApplication.getFtsServer().newOutletObject(parent, this); // create the fts outlet

    ninlets = 1;
    noutlets = 1;
  }


  public FtsOutletObject(FtsContainerObject parent, int position)
  {
    super(parent, "outlet", "outlet " + position);

    this.position = position;

    parent.addOutlet(this, position);

    MaxApplication.getFtsServer().newOutletObject(parent, this, position);// create the fts inlet

    ninlets = 1;
    noutlets = 1;
  }


  public void setPosition(int i)
  {
    position = i;
    description = "outlet " + position;
    MaxApplication.getFtsServer().redefineOutletObject(this, position);
  }

  public int getPosition()
  {
    return position;
  }

  /** Save the object to a TCL stream. */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "inlet ..."

    writer.print("outlet " + position);

    savePropertiesAsTcl(writer);
  }
}






