package ircam.jmax.mda.tcl;

import tcl.lang.*;

import ircam.jmax.mda.*;

/**
 * This command install a named tcl function as "editor factory"
 * for a named Max Data type; this function should create an editor
 * for it.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     jmax <i> doc-type [version] name info doc-body
 * </code>
 */

class JMaxCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length == 5) || (argv.length == 6))
      {
	String docType;
	String version; // ignored in this implementation
	String name;
	String info;
	TclObject body;
	MaxData data;

	if (argv.length == 5)
	  {
	    docType = argv[1].toString();
	    name    = argv[2].toString();
	    info    = argv[3].toString();
	    body    = argv[4];
	  }
	else
	  {
	    docType = argv[1].toString();
	    version = argv[2].toString();
	    name    = argv[3].toString();
	    info    = argv[4].toString();
	    body    = argv[5];
	  }

	// Create a new instance of the type

	data = MaxDataType.getTypeByName(docType).newInstance();

	if (data instanceof MaxTclData)
	  {
	    // Set the name and info

	    data.setName(name);
	    data.setInfo(info);

	    // Eval the body inside the data instance

	    ((MaxTclData) data).eval(interp, body);

	    // Finally, return the data to the tcl interpreter
    
	    interp.setResult(ReflectObject.newInstance(interp, data));
	  }
	else
	  new TclException(interp, docType + " is not a tcl based jMax Type");
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "doc-type [version] name info doc-body");
  }
}










  
