package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object;
 * This is the expansion of a Template.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsTemplateObject extends FtsContainerObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsTemplateObject object.
   */

  FtsTemplateObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);

    // A template is a tcl function that is executed to 
    // create the actual patcher content; the patcher is
    // passed as first argument, the user argument follow
    // the patcher.

    //create a 0 in 0 out patcher FtsObject

    MaxApplication.getFtsServer().newPatcherObject(parent, this, "unnamed", 0, 0);

    // Set  the object property

    // Should really do something better here in case of error !!!
    // raising exceptions ... 

    Interp interp  = MaxApplication.getTclInterp();

    try
      {
	// Call the tcl template function, with the container (this) as 
	// first argument, and the other args following. (or, the args are one
	// argument ???)

	TclObject list = TclList.newInstance();

	TclList.append(interp, list, TclString.newInstance(FtsTemplateTable.getProc(className)));
	TclList.append(interp, list, ReflectObject.newInstance(interp, this));
	TclList.append(interp, list, TclString.newInstance(description));

	interp.eval(list, 0);
      }
    catch (tcl.lang.TclException e)
      {
	System.out.println("TCL Error in template " + className + ":" + interp.getResult());
      }

    loaded();	// activate the post-load init, like loadbangs

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


  /** Save the object to a TCL stream. */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






