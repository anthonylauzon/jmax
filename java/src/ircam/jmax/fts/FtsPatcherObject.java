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

    obj.objectName = "root";
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
    super(parent, "patcher", description);

    Vector args;

    args = new Vector();
    
    FtsParse.parseObjectArguments(description, args);

    if (args.size() >= 1)
      setObjectName(args.elementAt(0).toString());
    else
      setObjectName("unnamed");

    if (args.size() >= 2)
      setNumberOfInlets(Integer.parseInt(args.elementAt(1).toString()));
    else
      setNumberOfInlets(0);

    if (args.size() >= 3)
      setNumberOfOutlets(Integer.parseInt(args.elementAt(2).toString()));
    else
      setNumberOfOutlets(0);

    updateDescription();

    FtsServer.getServer().newPatcherObject(parent, this,
						   getObjectName(),
						   ninlets,
						   noutlets);
    if (parent.isOpen())
      updated = true;
  }

  /**
   * Create a FtsPatcherObject object: real constructor;
   * a patcher name nins and nouts are actually taken from
   * properties, and are not arguments !
   */

  public FtsPatcherObject(FtsContainerObject parent)
  {
    super(parent, "patcher", "unnamed 0 0");

    setObjectName("unnamed");
    setNumberOfInlets(0);
    setNumberOfOutlets(0);
    updateDescription();

    FtsServer.getServer().newPatcherObject(parent, this,
						   objectName,
						   ninlets,
						   noutlets);
    if (parent.isOpen())
      updated = true;
  }


  /** Special method to redefine a patcher without looosing its content 
   */

  public void redefinePatcher(String description)
  {
    Vector args;

    args = new Vector();
    
    FtsParse.parseObjectArguments(description, args);

    if (args.size() >= 1)
      setObjectName((String) args.elementAt(0));
    else
      setObjectName("unnamed");

    if (args.size() >= 2)
      setNumberOfInlets(Integer.parseInt(args.elementAt(1).toString()));
    else
      setNumberOfInlets(0);

    if (args.size() >= 3)
      setNumberOfOutlets(Integer.parseInt(args.elementAt(2).toString()));
    else
      setNumberOfOutlets(0);

    updateFtsObject();
  }


  /** Overwrite the Update FTS object;
   * a pure patcher, for now, change its description every time
   * the number of inlets/outlets is changed; for the moment is done
   * directly, because there is no description property (and we don't
   * want it for the moment); also, this behaviour should disappear
   * when the patcher will have its own representation.
   */

  public void updateFtsObject()
  {
    super.updateFtsObject();

    updateDescription();
  }

  /** update the description;
    temporary code, waiting for the real thing (the green box)
   */

  void updateDescription()
  {
    description = objectName + " " + ninlets + " " + noutlets;
  }

  /** Save the object to a TCL stream. 
   * We use object id to index local variables,
   * because tcl variables are always hash table, not sequential
   * arrays; so, it is not worth to spend time counting objects.
   */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "patcher <properties> { <body> }

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
	
	if ((! (obj instanceof FtsDeclarationObject)) &&  obj.isPersistent())
	  {
	    writer.print("set obj(" + obj.getObjId() + ")" + " [");

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










