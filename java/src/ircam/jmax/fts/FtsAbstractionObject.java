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

public class FtsAbstractionObject  extends FtsContainerObject
{
  static boolean  expandAbstractions = true;

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsAbstractionObject object.
   */

  FtsAbstractionObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);

    // An abstraction is translated in its expansion (code from the FtsDotPat parser)
    // But, it is stored back as an object

    String patname;

    // Get the pathname (here, we are sure the abstraction exists, no tests)

    patname = FtsAbstractionTable.getFilename(className);
    
    //create a 0 in 0 out patcher FtsObject

    FtsServer.getServer().newPatcherObject(parent, this, "unnamed", 0, 0);

    // Parse the description to get the argument for the abstraction expander
    
    Vector args = new Vector();
    className = FtsParse.parseObject(description, args);

    // load the patcher content from the file

    // Should really do something better here in case of error !!!
    // raising exceptions ... 

    try
      {
	FtsDotPatParser.importAbstraction(this, new File(patname), args);
      }
    catch (FtsException e)
      {
	System.out.println("Error " + e + " in reading abstraction " + className);
      }
    catch (java.io.IOException e)
      {
	System.out.println("I/O Error " + e + " in reading abstraction " + className);
      }

    setObjectName("unnamed");
    assignInOutlets();

    loaded();	// activate the post-load init, like loadbangs

    if (parent.isOpen())
      FtsServer.getServer().syncToFts();
  }


  // For an abstraction loaded from FTS, just behave as a standard object.

  FtsAbstractionObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);

    this.className = className;
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/


  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






