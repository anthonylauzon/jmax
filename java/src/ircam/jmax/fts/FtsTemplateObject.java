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

  FtsTemplateObject(FtsContainerObject parent, String className, Vector args)
  {
    super(parent, className, args);

    // A template is a tcl function that is executed to 
    // create the actual patcher content; the patcher is
    // passed as first argument, the user argument follow
    // the patcher.


    // If there is no description1, generate one now, from the
    // the arguments before the instantiation
    // the call is fancy, but it actually generate and cache 
    // a description if it does not exists; if it exists,
    // it is a no-op.
	
    description = getDescription(); 

    // Then, reset the ftsClassName to "patcher"

    this.ftsClassName = "patcher";

    //create a 0 in 0 out patcher FtsObject

    FtsObject obj;
    Vector oargs = new Vector();
    oargs.addElement("unnamed"); // we want to have the name fixed by the template !!!
    oargs.addElement(new Integer(0));
    oargs.addElement(new Integer(0));

    MaxApplication.getFtsServer().newObject(parent, this, oargs);

    // create the empty subpatcher

    subPatcher = new FtsPatcher(this, "unnamed", 0, 0);

    // Should really do something better here in case of error !!!
    // raising exceptions ... 

    Interp interp  = MaxApplication.getTclInterp();

    try
      {
	// Call the tcl template function, with the container (this) as 
	// first argument, and the other args following.

	TclObject list = TclList.newInstance();

	TclList.append(interp, list, TclString.newInstance(FtsTemplateTable.getProc(className)));
	TclList.append(interp, list, ReflectObject.newInstance(interp, this));

	for (int i = 1; i < args.size(); i++)
	  {
	    Object arg = args.elementAt(i);

	    if (arg instanceof Integer)
	      TclList.append(interp, list, TclInteger.newInstance(((Integer)arg).intValue()));
	    else if (arg instanceof Float)
	      TclList.append(interp, list, TclDouble.newInstance(((Float)arg).doubleValue()));
	    else if (arg instanceof String)
	      TclList.append(interp, list, TclString.newInstance((String) arg));
	    else
	      TclList.append(interp, list, ReflectObject.newInstance(interp, arg));
	  }

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
   * YOu cannot change the arguments in a template.
   */

  public void setArguments(Vector args)
  {
    return;
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






