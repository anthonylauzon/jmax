package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsPatcherObject extends FtsContainerObject
{
  /**
   * MOVE TO FtsPatcher
   * Build the root object.
   * The root object is the super patcher of everything;
   * cannot be edited, can include only patchers.
   */

  // Should go to the patcher object

  static FtsContainerObject makeRootObject(FtsServer server)
  {
    FtsPatcherObject obj;

    // Build the arguments

    obj = new FtsPatcherObject();

    obj.className = "patcher";

    obj.name = "root";
    obj.ninlets = 0;
    obj.noutlets = 0;

    obj.parent = null;

    // create it in FTS

    server.newPatcherObject(null, obj, "root", 0, 0);

    return obj;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * The empty constructor.
   * Used only to build the special
   * root object.
   */

  FtsPatcherObject()
  {
    super();
  }

  /**
   * Create a FtsPatcherObject object: temporary constructor, should go away
   * as soon as the graphic patcher object work !
   */

  public FtsPatcherObject(FtsContainerObject parent, String description)
  {
    String name;
    Vector args;

    args = new Vector();
    
    name = FtsParse.parseObject(description, args);

    setName((String) args.elementAt(1));
    setNumberOfInlets(((Integer) args.elementAt(2)).intValue());
    setNumberOfOutlets(((Integer) args.elementAt(3)).intValue());

    MaxApplication.getFtsServer().newPatcherObject(parent, this,
						   name,
						   ninlets,
						   noutlets);
    if (parent.isOpen())
      updated = true;
  }

  /**
   * Create a FtsPatcherObject object.
   */


  public FtsPatcherObject(FtsContainerObject parent, String name, int ninlets, int noutlets)
  {
    super(parent, "patcher", "patcher " + name + " " + ninlets + " " + noutlets);

    setName(name);
    setNumberOfInlets(ninlets);
    setNumberOfOutlets(noutlets);

    MaxApplication.getFtsServer().newPatcherObject(parent, this,
						   name,
						   ninlets,
						   noutlets);
    if (parent.isOpen())
      updated = true;
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /** Save the object to a TCL stream. 
   * We use object id to index local variables,
   * because tcl variables are always hash table, not sequential
   * arrays; so, it is not worth to spend time counting objects.
   */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "patcher <properties> { <body> }

    // Save as "object ..."

    writer.print("patcher ");
    savePropertiesAsTcl(writer);
    writer.println(" {");

    if (writer instanceof IndentedPrintWriter)
      ((IndentedPrintWriter)writer).indentMore();

    // Write the body 
    //
    // First, store the declarations; declaration don't have
    // connections, so we don't store them in variables.

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (obj instanceof FtsDeclarationObject)
	  {
	    obj.saveAsTcl(writer);
	    writer.println();
	  }
      }

    // Then store the objects

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (! (obj instanceof FtsDeclarationObject))
	  {
	    writer.print("set objs(" + obj.getObjId() + ")" + " [");

	    obj.saveAsTcl(writer);

	    writer.println("]");
	  }
      }

    // Then, store the connections

    for (int i = 0; i < connections.size(); i++)
      {
	FtsConnection c   =  (FtsConnection) connections.elementAt(i);
	
	c.saveAsTcl(writer);
	writer.println();
      }

    if (writer instanceof IndentedPrintWriter)
      ((IndentedPrintWriter)writer).indentLess();
    
    writer.print("}");
  }
}






