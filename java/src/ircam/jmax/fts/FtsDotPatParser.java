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
  /** auxiliary class: store the graphic information for
   *  an object 
   */

  static class FtsGraphicDescription
  {
    int x = 0;
    int y = 0;
    int width = 0;
    int range = 0;
    int fontSize = 12;

    /** Called only if it make sense */

    void setRange(FtsDotPatTokenizer in) throws java.io.IOException
    {
      int n;

      in.nextToken();
      n = in.getNVal();

      range = n;
    }

    void setFontIndex(FtsDotPatTokenizer in) throws java.io.IOException
    {
      int n;

      in.nextToken();
      n = in.getNVal();

      switch (n)
	{
	case 0:
	  fontSize = 9;
	  break;
	case 1:
	  fontSize = 10;
	  break;
	case 2:
	  fontSize = 12;
	  break;
	case 3:
	  fontSize = 14;
	  break;
	case 4:
	  fontSize = 16;
	  break;
	case 5:
	  fontSize = 18;
	  break;
	case 6:
	  fontSize = 20;
	  break;
	case 7:
	  fontSize = 24;
	  break;
	}
    }

    void setTextGraphicProperties(FtsObject obj)
    {
      obj.setX(x);
      obj.setY(y);
      obj.setWidth(width);

      if (fontSize != 12)
	obj.put("fs", fontSize);
    }

    void setSliderGraphicProperties(FtsObject obj)
    {
      obj.setX(x);
      obj.setY(y);
      obj.setWidth(width);
      obj.setHeight(range + 20);
      obj.put("minValue", 0);
      obj.put("maxValue", range);
    }

    void setSquareGraphicProperties(FtsObject obj)
    {
      obj.setX(x);
      obj.setY(y);
      obj.setWidth(width);
      obj.setHeight(width);
    }
  }

  /**
   * Create a new patcher from a .pat file.
   * The patcher is always a top level patcher.
   * 
   * @param server the server where the patcher is loaded.
   * @param inputFile the file to read.
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */
    
  static public FtsContainerObject importPatcher(FtsServer server, File inputFile) throws java.io.IOException, FtsException
  {
    FtsDotPatTokenizer in = null; 

    // Build a new FtsObject, a patcher 0 in 0 out

    FtsContainerObject obj;

    obj = new FtsPatcherObject(server.getRootObject());

    try
      {
	in  = new FtsDotPatTokenizer(new BufferedInputStream(new FileInputStream(inputFile), 1024));
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

    obj.setObjectName("unnamed");
    obj.assignInOutlets();
    obj.put("autorouting", "off"); // compatibility with old stuff
    obj.loaded();	// activate the post-load init, like loadbangs

    return obj;
  }

  
  /**
   * Method implementing the actual reading and parsing.
   *
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */
  
  static void readFromFtsDotPatTokenizer(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsException
  {
    // skip the header from the file,

    in.nextToken();
	
    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("max"))
      syntaxError("file not in .pat format (header error)");

    in.nextToken(); 

    if ((in.ttype != FtsDotPatTokenizer.TT_STRING) && ! in.sval.equals("v2"))
      syntaxError("file not in .pat format (header error)");

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
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void parsePatcher(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsException
  {
    FtsObject lastNObject = null;
    String lastNObjectType = null;

    try
      {
	// We don't try to do many checks, for the moment

	in.nextToken(); // Skip the 'vpatcher'

	parseWindowProperties(parent, in); 

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
	      syntaxError("System Error parsing .pat file (" + (char) in.ttype + ")");

	    if (in.sval.equals("#N"))
	      {

		in.nextToken();

		if (in.sval.equals("vpatcher"))
		  {
		    in.pushBack();

		    lastNObject = new FtsPatcherObject(parent);
		    lastNObject.put("autorouting", "off"); // compatibility with old stuff
		    parsePatcher((FtsContainerObject) lastNObject, in);

		    lastNObjectType = "patcher";
		  }
		else if (in.sval.equals("qlist"))
		  {
		    lastNObject = FtsObject.makeFtsObject(parent, "qlist");
		    lastNObjectType = "qlist";

		    // skip the rest of the command: #N qlist argument are ignored

		    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
		      in.nextToken();
		  }
		else if (in.sval.equals("vtable"))
		  {
		    Vector vargs = new Vector();
		    StringBuffer description = new StringBuffer();

		    // Get the size and the name,
		    // ignore everything else (data
		    // that regard the editor)

		    readObjectArguments(vargs, in);

		    // get the name

		    description.append("table ");

		    if (vargs.size() >= 8)
		      {
			description.append(vargs.elementAt(7));
			description.append(" ");
		      }
		    else
		      description.append("table ");

		    // get the size

		    description.append(vargs.elementAt(0));

		    // Make the table

		    lastNObject = FtsObject.makeFtsObject(parent, description.toString());
		    lastNObjectType = "table";

		    // skip the ';'

		    in.nextToken();
		  }
		else if (in.sval.equals("explode"))
		  {
		    lastNObject = FtsObject.makeFtsObject(parent, "explode");
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
		  syntaxError("file not in .pat format (syntax error)");

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
		  syntaxError("Syntax error in a #T (table content)");		  
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
		  syntaxError("Syntax error: #X in a .pat file, after something different from qlist or table");
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
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found in the file,
   * or the file contains an unimplemented construct.
   */

  static public FtsObject importAbstraction(FtsContainerObject parent, File inputFile, Vector env) throws java.io.IOException, FtsException
  {
    FtsDotPatTokenizer in = null; 

    // open the file
    
    try
      {
	in  = new FtsDotPatTokenizer(new BufferedInputStream(new FileInputStream(inputFile), 1024), env);

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
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void parseConnection(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsException
  {
    int fromIdx;
    int toIdx;
    int inlet;
    int outlet;
    FtsObject from;
    FtsObject to;

    Vector objects;

    objects = parent.getObjects(); 

    in.nextToken(); 
    fromIdx = (int) in.getNVal();


    in.nextToken(); 
    outlet = (int) in.getNVal();

    in.nextToken();
    toIdx = (int) in.getNVal();

    in.nextToken(); 
    inlet  = (int) in.getNVal();

    //    System.err.println("In " + parent + " adding connection " + fromIdx + " " + outlet + " " + toIdx + " " + inlet);

    // for (int i = 0; i < objects.size(); i++)
    //      System.err.println("Object " + (objects.size() - i - 1) + " : " + objects.elementAt(i));

    from = (FtsObject) objects.elementAt(objects.size() - fromIdx - 1);
    to = (FtsObject) objects.elementAt(objects.size() - toIdx - 1);

    new FtsConnection(from, outlet, to, inlet);
  }

  /**
   * Read object description arguments in a vect.
   *
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void readObjectArguments(Vector args, FtsDotPatTokenizer in) throws java.io.IOException, FtsException
  {
    in.nextToken();

    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
      {

	switch (in.ttype)
	  {
	  case FtsDotPatTokenizer.TT_STRING:
	    args.addElement(in.sval);
	    break;

	  case FtsDotPatTokenizer.TT_NUMBER:
	    args.addElement(new Integer(in.getNVal()));
	    break;
	    
	  case FtsDotPatTokenizer.TT_FLOAT:
	    args.addElement(new Float(in.getFVal()));
	    break;
	  }

	in.nextToken();
      }

    in.pushBack();
  }


  /**
   * Read object description arguments in a String buffer, adding spaces between them,
   * and a new line in case of a ';'.
   *
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void readObjectArguments(StringBuffer args, FtsDotPatTokenizer in)
       throws java.io.IOException, FtsException
  {
    boolean addBlank = false ;
    in.nextToken();

    while (in.ttype != FtsDotPatTokenizer.TT_EOC)
      {
	int n;

	if (addBlank)
	  args.append(" ");
	else
	  addBlank = true;

	args.append(in.sval);

	if (in.sval.equals(";"))
	  {
	    args.append("\n");
	    addBlank = false;
	  }

	in.nextToken();
      }

    in.pushBack();
  }

  /**
   * Parse an object from the a tokenized stream.
   *
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsObject parseObject(FtsContainerObject parent, FtsDotPatTokenizer in, FtsObject lastNObject, String lastNObjectType)
       throws java.io.IOException, FtsException
  {
    FtsObject obj;
    String objclass = in.sval;

    FtsGraphicDescription graphicDescr;

    // get the graphic information

    graphicDescr = parseGraphic(in, objclass);

    if (objclass.equals("slider"))
      {
	graphicDescr.setRange(in);

	obj = FtsObject.makeFtsObject(parent, "slider");
	graphicDescr.setSliderGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("newex"))
      {
	StringBuffer description = new StringBuffer();

	graphicDescr.setFontIndex(in);
	in.nextToken();//get the object name

	// Abstraction are handled directly by the makeFtsObject function now.
	
	if (in.ttype == FtsDotPatTokenizer.TT_STRING)
	  {
	    description.append(in.sval);
	    description.append(" ");
	  }
	else if (in.ttype ==  FtsDotPatTokenizer.TT_NUMBER)
	  {
	    description.append("int ");
	    in.pushBack();
	  }
	else if (in.ttype ==  FtsDotPatTokenizer.TT_FLOAT)
	  {
	    description.append("float ");
	    in.pushBack();
	  }

	readObjectArguments(description, in);

	obj = FtsObject.makeFtsObject(parent, description.toString()); 
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("newobj"))
      {
	// Special handling for patchers, qlist, explode and table

	Vector args = new Vector();

	graphicDescr.setFontIndex(in);
	readObjectArguments(args, in);

	// Check if it is the patcher, otherwise just skip it

	if (args.elementAt(0).equals("patcher") && lastNObjectType.equals("patcher"))
	  {
	    /* add the two ninlet and noutlet arguments to description */

	    graphicDescr.setTextGraphicProperties((FtsContainerObject)lastNObject);

	    if (args.size() > 1)
	      {
		((FtsContainerObject)lastNObject).setObjectName((String) args.elementAt(1)); 
		((FtsContainerObject)lastNObject).assignInOutlets();
	      }
	    else
	      {
		((FtsContainerObject)lastNObject).setObjectName("unnamed"); 
		((FtsContainerObject)lastNObject).assignInOutlets();
	      }

	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("qlist") && lastNObjectType.equals("qlist"))
	  {
	    graphicDescr.setTextGraphicProperties(lastNObject);

	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("table") && lastNObjectType.equals("table"))
	  {
	    graphicDescr.setTextGraphicProperties(lastNObject);
	    return lastNObject;
	  }
	else if (args.elementAt(0).equals("explode") && lastNObjectType.equals("explode"))
	  {
	    graphicDescr.setTextGraphicProperties(lastNObject);

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

	obj = new FtsInletObject(parent);
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("outlet"))
      {
	//skip optional dsp flag 

	in.nextToken();

	if (in.ttype == FtsDotPatTokenizer.TT_EOC)
	  in.pushBack();

	obj = new FtsOutletObject(parent);
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("number"))
      {
	graphicDescr.setFontIndex(in);

	obj = FtsObject.makeFtsObject(parent, "intbox");
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("flonum"))
      {
	graphicDescr.setFontIndex(in);

	obj = FtsObject.makeFtsObject(parent, "floatbox");
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("button"))
      {
	obj = FtsObject.makeFtsObject(parent, "button");
	graphicDescr.setSquareGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("toggle"))
      {
	obj = FtsObject.makeFtsObject(parent, "toggle");
	graphicDescr.setSquareGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("message"))
      {
	StringBuffer description = new StringBuffer();

	graphicDescr.setFontIndex(in);
	readObjectArguments(description, in);

	obj = new FtsMessageObject(parent, description.toString());
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else if (objclass.equals("comment"))
      {
	StringBuffer description = new StringBuffer();

	graphicDescr.setFontIndex(in);
	readObjectArguments(description, in);

	obj = new FtsCommentObject(parent, description.toString());
	graphicDescr.setTextGraphicProperties(obj);
	return obj;
      }
    else
      {
	syntaxError("unknown object type error");
	return null;
      }
  }

	    
  /**
   * Parse a fts Graphic description.
   *
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static FtsGraphicDescription parseGraphic(FtsDotPatTokenizer in, String name) throws java.io.IOException, FtsException
  {

    FtsGraphicDescription g = new FtsGraphicDescription();

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.x = in.getNVal();
    else
      {
	in.pushBack();
	return g;
      }

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.y = in.getNVal();
    else
      {
	in.pushBack();
	return g;
      }

    in.nextToken();

    if (in.ttype == FtsDotPatTokenizer.TT_NUMBER)
      g.width = in.getNVal();
    else
      {
	in.pushBack();
	return g;
      }

    return g;
  }


  /**
   * Parse a window description.
   * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
   * or the file contains an unimplemented construct.
   */

  static void parseWindowProperties(FtsContainerObject parent, FtsDotPatTokenizer in) throws java.io.IOException, FtsException
  {
    int x2, y2;
    int x, y, height, width;

    // We don't try to do many checks, for the moment

    in.nextToken();
    x = in.getNVal();
    
    in.nextToken();
    y = in.getNVal();

    in.nextToken();
    x2 = in.getNVal();

    in.nextToken();
    y2 = in.getNVal();

    width = x2 - x;
    height = y2 - y;

    parent.put("wx", x);
    parent.put("wy", y);
    parent.put("ww", width);
    parent.put("wh", height);
  }

  private static void syntaxError(String description) throws FtsException
    {
      throw new FtsException(new FtsError(FtsError.DOTPAT_ERROR, description));
    }
}




