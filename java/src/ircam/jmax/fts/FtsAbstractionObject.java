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
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsAbstractionObject object.
   */

  FtsAbstractionObject(FtsContainerObject parent, String className, Vector args)
  {
    super(parent, className, args);

    // An abstraction is translated in its expansion (code from the FtsDotPat parser)
    // But, it is stored back as an object

    String patname;
    String realName;

    // If there is no description, generate one now, from the
    // the arguments before the instantiation
    // the call is fancy, but it actually generate and cache 
    // a description if it does not exists; if it exists,
    // it is a no-op.
	
    description = getDescription(); 

    // Then, reset the ftsClassName to "patcher"

    this.ftsClassName = "patcher";

    // First, remove the .pat or the .abs if present, and
    // compute the real name

    if (className.endsWith(".pat"))
      realName = className.substring(0, className.lastIndexOf(".pat"));
    else if (className.endsWith(".abs"))
      realName = className.substring(0, className.lastIndexOf(".abs"));
    else
      realName = className;

    if (FtsAbstractionTable.exists(realName))
      {
	patname = FtsAbstractionTable.getFilename(realName);
      }
    else
      patname = realName;
    
    //create a 0 in 0 out patcher FtsObject

    FtsObject obj;
    Vector oargs = new Vector();

    oargs.addElement("unnamed");
    oargs.addElement(new Integer(0));
    oargs.addElement(new Integer(0));

    MaxApplication.getFtsServer().newObject(parent, this, oargs);

    // load the patcher content from the file

    subPatcher = new FtsPatcher(this, "unnamed", 0, 0);

    // Should really do something better here in case of error !!!
    // raising exceptions ... 

    try
      {
	FtsDotPatParser.importAbstraction(this, new File(patname), args);
      }
    catch (FtsDotPatException e)
      {
	System.out.println("Error " + e + " in reading abstraction " + realName);
      }
    catch (java.io.IOException e)
      {
	System.out.println("I/O Error " + e + " in reading abstraction " + realName);
      }

    subPatcher.assignInOutletsAndName("unnamed");

    subPatcher.loaded();	// activate the post-load init, like loadbangs

    if (parent.isOpen())
      {
	updated = true;
	getProperty("ninlets");
	getProperty("noutlets");

	MaxApplication.getFtsServer().syncToFts();
      }
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/


  /**
   * Set the arguments.
   * YOu cannot change the arguments in an abstraction
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
    // Save as "object ..."

    stream.print("object $objs(" + parent.idx + ") ");

    saveArgsAsTcl(stream);

    stream.print(" ");

    if (graphicDescr != null)
      graphicDescr.saveAsTcl(stream);
  }
}






