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


  /**
   * Create a FtsInletObject object.
   */

  FtsInletObject(FtsContainerObject parent, Vector args)
  {
    super(parent, "inlet", args);
    
    if (args.size() >= 1)
      parent.getSubPatcher().addInlet(this, ((Integer)args.elementAt(0)).intValue());
    else
      parent.getSubPatcher().addInlet(this); // support for .pat

    MaxApplication.getFtsServer().newObject(parent, this, args);// create the fts object

    ninlets = 1;
    noutlets = 1;
  }

  /**
   * Set the arguments.
   * An inlet can't be refined.
   */

  public void setArguments(Vector args)
  {
    return;
  }

  /** Get the number of inlets of the object (valid only if the patcher is open). */

  public int getNumberOfInlets()
  {
    return 1;
  }

  /** Get the number of outlets of the object (valid only if the patcher is open). */

  public int getNumberOfOutlets()
  {
    return 1;
  }

  /** Save the object to a TCL stream. */

  void saveAsTcl(FtsSaveStream stream)
  {
    // Save as "object ..."

    stream.print("object $objs(" + parent.idx + ") ");

    saveArgsAsTcl(stream);

    stream.print(" ");

    if (graphicDescr != null)
      graphicDescr.saveAsTcl(stream);
  }
}






