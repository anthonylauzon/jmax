package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>_patcher</b>,
 * used by the tcl function patcher, that create a FTS patcher.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  _patcher <i>patcher ninlets noutlets [graphic_data window_data]</i>
 * </code> <p>
 *
 * The first form (without inlets, patcher and graphic data) 
 * correspond to a root patcher.
 */



class FtsPatcherCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length == 1) || (argv.length == 2))
      {
	// Building a root patcher

	Vector args;
	FtsContainerObject object;
	String    windowDescription = null;

	// Retrieve the arguments
	// this call should be substituted by a registration service call

	if (argv.length == 2)
	  windowDescription = new String(argv[1].toString());

	args = new Vector();
	args.addElement("unnamed");
	args.addElement(new Integer(0));
	args.addElement(new Integer(0));
    
	// The Patcher command will change in the new file format
	// object = (FtsContainerObject) FtsObject.makeFtsObject(MaxApplication.getFtsServer().getRootObject(),
	// "patcher", args);
	
	//	if (windowDescription != null)
	// object.setWindowDescription(new FtsWindowDescription(windowDescription));

	// interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else if (argv.length >= 5) 
      {
	Vector args;
	FtsContainerObject object;
	FtsContainerObject parent;
	int       ninlets;
	int       noutlets;
	String    name;
	String    windowDescription = null;
	String    graphicDescription = null;

	// Retrieve the arguments
	// this call should be substituted by a registration service call

	parent    = (FtsContainerObject) ReflectObject.get(interp, argv[1]);
	name      = new String(argv[2].toString());
	ninlets   = TclInteger.get(interp, argv[3]);
	noutlets  = TclInteger.get(interp, argv[4]);


	if (argv.length > 5) 
	  graphicDescription = new String(argv[5].toString());

	if (argv.length > 6) 
	  windowDescription = new String(argv[6].toString());

	args = new Vector();
	args.addElement(name);
	args.addElement(new Integer(ninlets));
	args.addElement(new Integer(noutlets));

	// object = (FtsContainerObject) FtsObject.makeFtsObject(parent, "patcher", args);    

	// if (graphicDescription != null)
	// object.setGraphicDescription(new FtsGraphicDescription(graphicDescription));

	// if (windowDescription != null)
	// object.setWindowDescription(new FtsWindowDescription(windowDescription));

	// interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<patcher> <name> <ninlets> <noutlets> [<graphic_data> <window_data>], patcher [<window_data>]");
      }
  }
}


