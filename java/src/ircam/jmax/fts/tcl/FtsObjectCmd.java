package ircam.jmax.fts.tcl;


import tcl.lang.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>object</b>,
 * that create an FTS object.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *      object <i> <description> <properties> <i>
 * </code>
 */

class FtsObjectCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	FtsObject object;
	FtsContainerObject parent=null;
	String    description;
	TclObject properties;
	String    className;

	// Retrieve the arguments

	try
	  {
	    parent = (FtsContainerObject) FtsContainerObject.containerStack.peek();
	  }
	catch(EmptyStackException e)
	  {
	    throw new TclException(interp, " context not defined for 'object' command");
	  }

	if (parent == null)
	  throw new TclException(interp, " context not defined for 'object' command");

	description = argv[1].toString();
	properties = argv[2];

	/* Extract the className */

	{
	  int idx;

	  idx = description.indexOf(' ');

	  if (idx == -1)
	    className = description; 
	  else
	    className = description.substring(0, description.indexOf(' '));
	}

	try
	  {
	    if (className.equals("messbox"))
	      {
		String content = description.substring(description.indexOf('{') + 1,
						       description.lastIndexOf('}'));

		object = FtsObject.makeFtsObject(parent, "messbox", content);
		object.parseTclProperties(interp, properties);

		interp.setResult(ReflectObject.newInstance(interp, object));
	      }
	    else
	      {
		object = FtsObject.makeFtsObject(parent, description);
		object.parseTclProperties(interp, properties);
		interp.setResult(ReflectObject.newInstance(interp, object));
	      }
	  }
	catch (FtsException e)
	  {
	    // Should actually post an error to the FTS queue ??
	    throw new TclException(interp, e.toString());
	  }
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<object> <description> <properties>");
      }
  }
}


