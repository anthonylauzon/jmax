package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * Parse a .pat file.
 * A class providing, as a set of static method, a parser
 * for old Max .pat format files.
 */

public class FtsDotPatParser
{
  /**
   * Create a new patcher from a .pat file.
   * The patcher is always a top level patcher.
   * 
   * @param server the server where the patcher is loaded.
   * @param inputFile the file to read.
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */
    
  static public FtsContainerObject importPatcher(FtsServer server, File inputFile) throws java.io.IOException, FtsDotPatException
  {
    FtsDotPatTokenizer in = null; 

    // Build a new FtsObject, a patcher 0 in 0 out

    FtsContainerObject obj;
    Vector oargs = new Vector();

    oargs.addElement("unnamed");
    oargs.addElement(new Integer(0));
    oargs.addElement(new Integer(0));

    obj = (FtsContainerObject) FtsObject.makeFtsObject(server.getRootObject(), "patcher", oargs);

    try
      {
	in  = new FtsDotPatTokenizer(new FileInputStream(inputFile));
      }
    catch (FileNotFoundException e)
      {
	System.out.println("file not found" + inputFile.getName()); // Should raise an exception
      }
    catch (IOException e)
      {
	System.out.println("io error" + inputFile.getName()); // Should raise an exception
      }

    readFromFtsDotPatTokenizer(obj, in);

    obj.getSubPatcher().assignInOutletsAndName("unnamed");

    obj.loaded();	// activate the post-load init, like loadbangs

    return obj;
  }

  
  /**
   * Method implementing the actual reading and parsing.
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */
  
  static void readFromFtsDotPatTokenizer(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    // skip the header from the file,

    in.nextToken();
	
    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("max"))
      throw new FtsDotPatException("file not in .pat format (header error)");

    in.nextToken(); 

    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("v2"))
      throw new FtsDotPatException("file not in .pat format (header error)");

    // Skip possible declarations

    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
      in.nextToken();

    //    in.nextToken(); // Skip the ';'
    in.nextToken(); // Skip the '#N'

    // call the parser for a patcher, implemented
    // as a patcher constructor, with the stream pointing
    // to the "vpatcher" word.

    parsePatcher(parent, in);

    // Should do something on the FTS side ... !!! later
  }

  /**
   * Parse a patcher.
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void parsePatcher(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    FtsObject lastNObject = null;
    String lastNObjectType = null;

    try
      {
	// We don't try to do many checks, for the moment

	in.nextToken(); // Skip the 'vpatcher'

	parent.setWindowDescription(parseWindow(in)); 

	// skip the optional processor number

	in.nextToken();

	if (in.ttype !=  FtsDotPatTokenizer.TT_EOC)
	  {
	    in.nextToken();	//skip ';'

	  }

	while (in.ttype != FtsDotPatTokenizer.TT_EOF)
	  {
	    in.nextToken(); 


	    if (in.ttype != FtsDotPatTokenizer.TT_STRING)
	      throw new FtsDotPatException("System Error parsing .pat file (" + (char) in.ttype + ")");

	    if (in.sval.equals("#N"))
	      {

		in.nextToken();

		if (in.sval.equals("vpatcher"))
		  {
		    in.pushBack();

		    Vector args = new Vector();

		    args.addElement("unnamed");
		    args.addElement(new Integer(0));
		    args.addElement(new Integer(0));

		    lastNObject = FtsObject.makeFtsObject(parent, "patcher", args);

		    parsePatcher((FtsContainerObject) lastNObject, in);

		    lastNObjectType = "patcher";
		  }
		else if (in.sval.equals("qlist"))
		  {
		    lastNObject = FtsObject.makeFtsObject(parent, "qlist", (Vector) null);
		    lastNObjectType = "qlist";

		    // skip the rest of the command: #N qlist argument are ignored

		    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
		      in.nextToken();
		  }
		else if (in.sval.equals("vtable"))
		  {
		    Vector oargs = new Vector();
		    Vector args = new Vector();

		    // Get the size and the name,
		    // ignore everything else (data
		    // that regard the editor)

		    readObjectArguments(oargs, in);

		    // get the name

		    if (oargs.size() >= 8)
		      args.addElement(oargs.elementAt(7));
		    else
		      args.addElement("table");

		    // get the size

		    args.addElement(oargs.elementAt(0));

		    // Make the table

		    lastNObject = FtsObject.makeFtsObject(parent, "table", args);
		    lastNObjectType = "table";

		    // skip the ';'

		    in.nextToken();
		  }
		else if (in.sval.equals("explode"))
		  {
		    lastNObject = FtsObject.makeFtsObject(parent, "explode", (Vector) null);
		    lastNObjectType = "explode";

		    // skip the rest of the command

		    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
		      in.nextToken();
		  }
	      }
	    else if (in.sval.equals("#P"))
	      {
		// get the line name

		in.nextToken(); 

		if (in.ttype != FtsDotPatTokenizer.TT_STRING) 
		  throw new FtsDotPatException("file not in .pat format (syntax error)");

		if (in.sval.equals("connect"))
		  {
		    parseConnection(parent, in);
		  }
		else if (in.sval.equals("pop"))
		  {
		    in.nextToken();	// skip ';'
		    
		    if  (in.ttype != FtsDotPatTokenizer.TT_EOC)
		      {
			// the open patcher flag has been specified

			in.nextToken();	// skip ';'
		      }
		    
		    return ;
		  }
		else
		  {
		    // Otherwise, we are parsing an object

		    parseObject(parent, in, lastNObject, lastNObjectType);
		    lastNObject = null;
		    lastNObjectType = null;
		  }

		in.nextToken();//skip ';'
	      }
	    else if (in.sval.equals("#T"))
	      {
		in.nextToken(); // get the command

		if (in.sval.equals("set"))
		  {
		    Vector args = new Vector();

		    readObjectArguments(args, in);
		    lastNObject.sendMessage(0, "_set", args);

		    in.nextToken();//skip ';' ??
		  }
		else
		  throw new FtsDotPatException("Syntax error in a #T (table content)");		  
	      }
	    else if (in.sval.equals("#X"))
	      {
		if (lastNObjectType.equals("qlist"))
		  {
		    in.nextToken(); // get the command

		    String selector = in.sval;

		    Vector args = new Vector();

		    readObjectArguments(args, in);

		    lastNObject.sendMessage(0, selector, args);

		    in.nextToken();//skip ';' ??
		  }
		else if (lastNObjectType.equals("explode"))
		  {
		    in.nextToken(); // get the command

		    if (in.ttype == FtsDotPatTokenizer.TT_STRING)
		      {
			// restore or stop commands

			lastNObject.sendMessage(0, in.sval, (Vector) null);
		      }
		    else
		      {
			// append command (without the keyword, sic. !!)
			
			in.pushBack();

			Vector args = new Vector();
			readObjectArguments(args, in);
			lastNObject.sendMessage(0, "append", args);
		      }

		    in.nextToken();//skip ';' ??
		  }
		else 
		  throw new FtsDotPatException("Syntax error: #X in a .pat file, " +
					       "after something different from qlist or table");
	      }
	    else
	      {
		System.out.println("format not supported (#" + (char) in.ttype + ")");

		// skip until the next ';'

		while ((in.ttype != FtsDotPatTokenizer.TT_EOC) && (in.ttype != FtsDotPatTokenizer.TT_EOF))
		  in.nextToken();//skip ';'
	      }
	  }
      }
    catch (java.io.IOException e)
      {
	System.out.println("parse error !!!");  // Should raise an exception
      }

  }

  /**
   * Load an abstraction, substituing the arguments.
   * 
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */

  static public FtsObject importAbstraction(FtsContainerObject parent, File inputFile, Vector env) throws java.io.IOException, FtsDotPatException
  {
    FtsDotPatTokenizer in = null; 

    // open the file
    
    try
      {
	in  = new FtsDotPatTokenizer(new FileInputStream(inputFile), env);

	readFromFtsDotPatTokenizer(parent, in);

	return parent;
      }
    catch (FileNotFoundException e)
      {
	System.out.println("Loading abstraction : file not found: " + inputFile.getName()); // Should raise an exception
	return null;
      }
    catch (IOException e)
      {
	System.out.println("Loading abstraction : io error: " + inputFile.getName()); // Should raise an exception
	return null;
      }
  }


  /**
   * Parse a connection. 
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsConnection parseConnection(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {

    FtsObject from;
    FtsObject to;
    int inlet;
    int outlet;

    Vector objects;

    objects = parent.getContainedObjects();

    in.nextToken(); 
    from = (FtsObject) objects.elementAt(objects.size() - (int) in.nval - 1);

    in.nextToken(); 
    outlet = (int) in.nval;

    in.nextToken(); 
    to = (FtsObject) objects.elementAt(objects.size() - (int) in.nval - 1);

    in.nextToken(); 
    inlet  = (int) in.nval;

    return new FtsConnection(from, outlet, to, inlet);
  }

  /**
   * Read object description arguments.
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void readObjectArguments(Vector args, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    in.nextToken();

    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
      {
	int n;

	switch (in.ttype)
	  {
	  case FtsDotPatTokenizer.TT_STRING:
	    args.addElement(in.sval);
	    break;

	  case FtsDotPatTokenizer.TT_NUMBER:
	    args.addElement(new Integer(in.nval));
	    break;
	    
	  case FtsDotPatTokenizer.TT_FLOAT:
	    args.addElement(new Float(in.fval));
	    break;
	  }

	in.nextToken();
      }

    in.pushBack();
  }

  /**
   * Parse an object from the a tokenized stream.
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsObject parseObject(FtsContainerObject parent, FtsDotPatTokenizer in, FtsObject lastNObject, String lastNObjectType)
       throws java.io.IOException, FtsDotPatException
  {
    String objclass = in.sval;
    Vector args;
    FtsGraphicDescription graphicDescr;

    args = new Vector();

    // get the graphic information

    graphicDescr = parseGraphic(in, objclass);


    if (objclass.equals("slider"))
      {
	in.nextToken();//skip 'unknown' (may be range)

	return FtsObject.makeFtsObject(parent, "slider", args, graphicDescr);
      }
    else if (objclass.equals("newex"))
      {
	String className;

	in.nextToken();//skip 'unknown'
	in.nextToken();//get the object name

	// Abstraction are handled directly by the makeFtsObject function now.
	
	if (in.ttype == FtsDotPatTokenizer.TT_STRING)
	  {
	    className = in.sval;
	  }
	else if (in.ttype ==  FtsDotPatTokenizer.TT_NUMBER)
	  {
	    className = "int";	// to accept objects with an INT as content
	    in.pushBack();
	  }
	else if (in.ttype ==  FtsDotPatTokenizer.TT_FLOAT)
	  {
	    className = "float";// to accept objects with a FLOAT as content
	    in.pushBack();
	  }
	else
	  className = "";	// just for the compiler :-< 

	readObjectArguments(args, in);

	return FtsObject.makeFtsObject(parent, className, args, graphicDescr); 
      }
    else if (objclass.equals("newobj"))
      {
	// Special handling for patchers, qlist, explode and table

	in.nextToken();//skip 'unknown'
	readObjectArguments(args, in);

	// Check if it is the patcher, otherwise just skip it

	if (args.elementAt(0).equals("patcher") && lastNObjectType.equals("patcher"))
	  {
	    /* add the two ninlet and noutlet arguments to args */

	    ((FtsContainerObject)lastNObject).setGraphicDescription(graphicDescr);

	    if (args.size() > 1)
	      ((FtsContainerObject)lastNObject).getSubPatcher().assignInOutletsAndName((String) args.elementAt(1));
	    else
	      ((FtsContainerObject)lastNObject).getSubPatcher().assignInOutletsAndName("unnamed");

	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("qlist") && lastNObjectType.equals("qlist"))
	  {
	    lastNObject.setGraphicDescription(graphicDescr);

	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("table") && lastNObjectType.equals("table"))
	  {
	    lastNObject.setGraphicDescription(graphicDescr);

	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("explode") && lastNObjectType.equals("explode"))
	  {
	    lastNObject.setGraphicDescription(graphicDescr);

	    return lastNObject;
	  }
	else
	  {
	    System.out.println(args.elementAt(0) + " Not yet Supported in .pat files");

	    return null;
	  }
      }
    else if (objclass.equals("inlet"))
      {
	//skip optional dsp flag 

	in.nextToken();

	if (in.ttype == FtsDotPatTokenizer.TT_EOC)
	  in.pushBack();

	return FtsObject.makeFtsObject(parent, "inlet", args, graphicDescr);
      }
    else if (objclass.equals("outlet"))
      {
	//skip optional dsp flag 

	in.nextToken();

	if (in.ttype == FtsDotPatTokenizer.TT_EOC)
	  in.pushBack();

	return FtsObject.makeFtsObject(parent, "outlet", args, graphicDescr);
      }
    else if (objclass.equals("number"))
      {
	in.nextToken();//skip 'unknown'

	return FtsObject.makeFtsObject(parent, "intbox", args, graphicDescr);
      }
    else if (objclass.equals("flonum"))
      {
	in.nextToken();//skip 'unknown'

	return FtsObject.makeFtsObject(parent, "floatbox", args, graphicDescr);
      }
    else if (objclass.equals("button"))
      {
	return FtsObject.makeFtsObject(parent, "button", args, graphicDescr);
      }
    else if (objclass.equals("toggle"))
      {
	return FtsObject.makeFtsObject(parent, "toggle", args, graphicDescr);
      }
    else if (objclass.equals("message"))
      {
	in.nextToken();//skip 'unknown'
	readObjectArguments(args, in);

	return FtsObject.makeFtsObject(parent, "message", args, graphicDescr);
      }
    else if (objclass.equals("comment"))
      {
	in.nextToken();//skip 'unknown'
	readObjectArguments(args, in);

	return FtsObject.makeFtsObject(parent, "comment", args, graphicDescr);
      }
    else
      throw new FtsDotPatException("unknown object type error");
  }

	    
  /**
   * Parse a fts Graphic description.
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsGraphicDescription parseGraphic(FtsDotPatTokenizer in, String name) throws java.io.IOException, FtsDotPatException
  {

    FtsGraphicDescription g = new FtsGraphicDescription();

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.x = in.nval;
    else
      {
	in.pushBack();
	return g;
      }

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.y = in.nval;
    else
      {
	in.pushBack();
	return g;
      }

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.width = in.nval;
    else
      {
	in.pushBack();
	return g;
      }

    return g;
  }


  /**
   * Parse a window description.
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsWindowDescription parseWindow(FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    FtsWindowDescription ret;
    int x2, y2;
    int x, y, height, width;

    // We don't try to do many checks, for the moment

    in.nextToken();
    x = in.nval;
    
    in.nextToken();
    y = in.nval;

    in.nextToken();
    x2 = in.nval;

    in.nextToken();
    y2 = in.nval;

    width = x2 - x;
    height = y2 - y;

    ret = new FtsWindowDescription(x, y, width, height);

    return ret;
  }
}






