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

public class FtsPatcherObject extends FtsContainerObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * The empty constructor.
   * Used only to build the special
   * root object.
   */

  FtsPatcherObject()
  {
    super();
  }

  /**
   * Create a FtsPatcherObject object.
   */

  FtsPatcherObject(FtsContainerObject parent, Vector args)
  {
    super(parent, "patcher", args);

    ninlets =  ((Integer)args.elementAt(1)).intValue();
    noutlets = ((Integer)args.elementAt(2)).intValue();

    // This go to the FtsPatcherObject class

    subPatcher = new FtsPatcher(this, (String) args.elementAt(0), ninlets, noutlets);

    MaxApplication.getFtsServer().newObject(parent, this,  args);// create the fts object

    if (parent.isOpen())
      {
	updated = true;
	MaxApplication.getFtsServer().syncToFts();
      }
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /* Accessors and selectors. */

  /**
   * Set the arguments.
   * This may require an object redefinition
   * on the FTS side; this may then imply some comunication with FTS.
   */

  public void setArguments(Vector args)
  {
    this.args = args;

    // Use fts class name, not user class name

    MaxApplication.getFtsServer().redefineObject(this, getFtsClassName(), args);

    ninlets =  ((Integer)args.elementAt(1)).intValue();
    noutlets = ((Integer)args.elementAt(2)).intValue();

    subPatcher.redefine(ninlets, noutlets);
  }


  /** Save the object to a TCL stream. */

  void saveAsTcl(FtsSaveStream stream)
  {
    if (parent == MaxApplication.getFtsServer().getRootObject())
      {
	stream.print("patcher ");

	if (subPatcher.windowDescr != null)
	  subPatcher.windowDescr.saveAsTcl(stream);

	// This is a root patcher
      }
    else
      {
	// Save as "patcher ..."

	stream.print("patcher $objs(" + parent.idx + ") " +
		     (String) args.elementAt(0) + " " +
		     subPatcher.ninlets + " " + subPatcher.noutlets + " ");

	if (graphicDescr != null)
	  graphicDescr.saveAsTcl(stream);

	stream.print(" ");

	if (subPatcher.windowDescr != null)
	  subPatcher.windowDescr.saveAsTcl(stream);
      }
  }

}






