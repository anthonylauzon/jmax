package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;



/**
 * This class define the TCL Command <b>patcher</b>,
 * that create a FTS patcher.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  patcher [<i>window_data</i>]
 *  patcher <i>patcher ninlets noutlets [graphic_data window_data]</i>
 * </code> <p>
 *
 * The first form (without inlets, patcher and graphic data) 
 * correspond to a root patcher.
 */



class FtsPatcherCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if ((ca.argc == 1) || (ca.argc == 2))
      {
	// Building a root patcher

	Vector args;
	FtsObject object;
	FtsObject parent;
	String    windowDescription = null;

	// Retrieve the arguments
	// this call should be substituted by a registration service call

	if (ca.argc == 2)
	  windowDescription = ca.argv(1);

	args = new Vector();
	args.addElement("unnamed");
	args.addElement(new Integer(0));
	args.addElement(new Integer(0));
    
	object = FtsObject.makeFtsObject(MaxApplication.getFtsServer().getRootObject(),
					 "patcher", args);
	
	if (windowDescription != null)
	  object.setWindowDescription(new FtsWindowDescription(windowDescription));

	return String.valueOf(object.getObjId());
      }
    else if (ca.argc >= 5)
      {
	Vector args;
	FtsObject object;
	FtsObject parent;
	int       ninlets;
	int       noutlets;
	String    name;
	String    windowDescription = null;
	String    graphicDescription = null;
	FtsServer server;

	// Retrieve the fts server (should be got from a Tcl variable ??)

	server = MaxApplication.getFtsServer();

	// Retrieve the arguments
	// this call should be substituted by a registration service call

	parent   = server.getObjectByFtsId(ca.intArg(1));
	name      = ca.argv(2);
	ninlets   = ca.intArg(3);
	noutlets  = ca.intArg(4);



	if (ca.argc > 5)
	  graphicDescription = ca.argv(5);

	if (ca.argc > 6)
	  windowDescription = ca.argv(6);

	args = new Vector();
	args.addElement(name);
	args.addElement(new Integer(ninlets));
	args.addElement(new Integer(noutlets));
    
	if (graphicDescription != null)
	  object = FtsObject.makeFtsObject(parent, "patcher", args,
					   new FtsGraphicDescription(graphicDescription), false);
	else
	  object = FtsObject.makeFtsObject(parent, "patcher", args);

	if (windowDescription != null)
	  object.setWindowDescription(new FtsWindowDescription(windowDescription));

	return String.valueOf(object.getObjId());
      }
    else
      {
	throw new EvalException("missing argument; usage: patcher <patcher> <name> <ninlets> <noutlets> [<graphic_data> <window_data>], patcher [<window_data>]");
      }
  }
}

