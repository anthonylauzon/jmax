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
  // no constructor

  private FtsDotPatParser()
  {
  }

  /**
   * Create a new patcher from a .pat file.
   * The patcher is always a top level patcher.
   * 
   * @param server the server where the patcher is loaded.
   * @param inputFile the file to read.
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */
    
  static public FtsObject importPatcher(FtsServer server, File inputFile) throws java.io.IOException, FtsDotPatException
  {
    FtsDotPatTokenizer in = null; 
    FtsPatcher p;

    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject obj;
    Vector oargs = new Vector();

    oargs.addElement("unnamed");
    oargs.addElement(new Integer(0));
    oargs.addElement(new Integer(0));

    obj = FtsObject.makeFtsObject(server.getRootObject(), "patcher", oargs);

    try
      {
	in  = new FtsDotPatTokenizer(new FileInputStream(inputFile));
      }
    catch (FileNotFoundException e)
      {
	System.err.println("file not found" + inputFile.getName()); // Should raise an exception
      }
    catch (IOException e)
      {
	System.err.println("io error" + inputFile.getName()); // Should raise an exception
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
  
  static void readFromFtsDotPatTokenizer(FtsObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    // skip the header from the file,

    in.nextToken();
	
    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("max"))
      throw new FtsDotPatException("file not in .pat format (header error)");

    in.nextToken(); 

    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("v2"))
      throw new FtsDotPatException("file not in .pat format (header error)");

    in.nextToken(); // Skip the ';'
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

  static void parsePatcher(FtsObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
  {
    FtsObject lastPatcher = null;

    try
      {
	// We don't try to do many checks, for the moment

	in.nextToken(); // Skip the 'vpatcher'

	parent.setWindowDescription(parseWindow(in)); 

	// skip the optional processor number

	in.nextToken();

	if (in.ttype !=  FtsDotPatTokenizer.TT_EOC)
	  in.nextToken();	//skip ';'

	while (in.ttype != FtsDotPatTokenizer.TT_EOF)
	  {
	    in.nextToken(); 
	    if (in.ttype != FtsDotPatTokenizer.TT_STRING)
	      throw new FtsDotPatException("format not supported (" + (char) in.ttype + ")");

	    if (in.sval.equals("#N"))
	      {
		FtsObject pt;

		// Here it will need more analysys to handle
		// qlist, explode and tables.

		Vector args = new Vector();

		args.addElement("unnamed");
		args.addElement(new Integer(0));
		args.addElement(new Integer(0));
		pt = FtsObject.makeFtsObject(parent, "patcher", args);

		parsePatcher(pt, in);

		lastPatcher = pt;
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

			(parent.getWindowDescription()).open = true;
		      }
		    
		    return ;
		  }
		else
		  {
		    // Otherwise, we are parsing an object

		    parseObject(parent, in, lastPatcher); // the object add itself to the patcher
		  }

		in.nextToken();//skip ';'
	      }
	    else
	      {
		System.err.println("format not supported (#" + (char) in.ttype + ")");

		// skip until the next ';'

		while ((in.ttype != FtsDotPatTokenizer.TT_EOC) && (in.ttype != FtsDotPatTokenizer.TT_EOF))
		  in.nextToken();//skip ';'
	      }
	  }
      }
    catch (java.io.IOException e)
      {
	System.err.println("parse error !!!");  // Should raise an exception
      }

  }

  /**
   * Load an abstraction, substituing the arguments.
   * 
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */

  static public FtsObject importAbstraction(FtsObject parent, File inputFile, Vector env) throws java.io.IOException, FtsDotPatException
  {
    FtsDotPatTokenizer in = null; 

    // open the file
    
    try
      {
	in  = new FtsDotPatTokenizer(new FileInputStream(inputFile), env);
      }
    catch (FileNotFoundException e)
      {
	System.err.println("file not found" + inputFile.getName()); // Should raise an exception
      }
    catch (IOException e)
      {
	System.err.println("io error" + inputFile.getName()); // Should raise an exception
      }

    readFromFtsDotPatTokenizer(parent, in);

    return parent;
  }


  /**
   * Parse a connection. 
   *
   * @exception ircam.jmax.fts.FtsDotPatException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsConnection parseConnection(FtsObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsDotPatException
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

  static FtsObject parseObject(FtsObject parent, FtsDotPatTokenizer in, FtsObject lastPatcher)
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

	return FtsObject.makeFtsObject(parent, "gint", args, graphicDescr);
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
	// for now, only patchers are supported.

	in.nextToken();//skip 'unknown'
	readObjectArguments(args, in);

	// Check if it is the patcher, otherwise just skip it

	if (args.elementAt(0).equals("patcher"))
	  {
	    FtsObject obj;

	    /* add the two ninlet and noutlet arguments to args */

	    obj = lastPatcher;
	    
	    obj.setGraphicDescription(graphicDescr);

	    if (args.size() > 1)
	      obj.getSubPatcher().assignInOutletsAndName((String) args.elementAt(1));
	    else
	      obj.getSubPatcher().assignInOutletsAndName("unnamed");

	    return obj;
	  }
	else
	  {
	    System.err.println(args.elementAt(0) + " Not yet Supported in .pat files");

	    return null;
	  }
      }
    else if (objclass.equals("inlet"))
      {
	//skip optional dsp flag 

	FtsObject obj;

	in.nextToken();

	if (in.ttype == FtsDotPatTokenizer.TT_EOC)
	  in.pushBack();

	obj = FtsObject.makeFtsObject(parent, "inlet", args, graphicDescr);

	return obj;
      }
    else if (objclass.equals("outlet"))
      {
	//skip optional dsp flag 

	FtsObject obj;

	in.nextToken();

	if (in.ttype == FtsDotPatTokenizer.TT_EOC)
	  in.pushBack();

	obj = FtsObject.makeFtsObject(parent, "outlet", args, graphicDescr);

	return obj;
      }
    else if (objclass.equals("number"))
      {
	in.nextToken();//skip 'unknown'

	return FtsObject.makeFtsObject(parent, "gint", args, graphicDescr);
      }
    else if (objclass.equals("flonum"))
      {
	in.nextToken();//skip 'unknown'

	return FtsObject.makeFtsObject(parent, "gfloat", args, graphicDescr);
      }
    else if (objclass.equals("button"))
      {
	return FtsObject.makeFtsObject(parent, "button", args, graphicDescr);
      }
    else if (objclass.equals("toggle"))
      {
	return FtsObject.makeFtsObject(parent, "vtoggle", args, graphicDescr);
      }
    else if (objclass.equals("message"))
      {
	in.nextToken();//skip 'unknown'
	readObjectArguments(args, in);

	return FtsObject.makeFtsObject(parent, "vmessage", args, graphicDescr);
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

    g.name = name;

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






