package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsPatcherObject extends FtsContainerObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsPatcherObject object
   */


  public FtsPatcherObject(FtsContainerObject parent, String name, int ninlets, int noutlets, int objId)
  {
    super(parent, "patcher", "", objId);

    setObjectName(name);
    setNumberOfInlets(ninlets);
    setNumberOfOutlets(noutlets);

    updateDescription();
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

    for (int i = 0; i < getObjects().size(); i++)
      {
	FtsObject obj   =  (FtsObject) getObjects().elementAt(i);
	
	if (obj instanceof FtsDeclarationObject)
	  {
	    obj.saveAsTcl(writer);
	    writer.println();
	  }
      }

    // Then store the objects

    for (int i = 0; i < getObjects().size(); i++)
      {
	FtsObject obj   =  (FtsObject) getObjects().elementAt(i);
	
	if (! (obj instanceof FtsDeclarationObject))
	  {
	    writer.print("set obj(" + obj.getObjId() + ")" + " [");

	    obj.saveAsTcl(writer);

	    writer.println("]");

	    // If the object have data, save the data

 	    if ((! (obj instanceof FtsContainerObject)) && (obj instanceof FtsObjectWithData))
 	      {
 		FtsDataObject data;

		data = (FtsDataObject) ((FtsObjectWithData) obj).getData();
		    
		writer.print("setData $obj(" + obj.getObjId() + ")" + " [");
		data.saveAsTcl(writer);
		writer.println("]");
	      }
	  }
      }

    // Then, store the connections

    for (int i = 0; i < getConnections().size(); i++)
      {
	FtsConnection c   =  (FtsConnection) getConnections().elementAt(i);
	
	c.saveAsTcl(writer);
	writer.println();
      }

    if (writer instanceof IndentedPrintWriter)
      ((IndentedPrintWriter)writer).indentLess();
    
    writer.print("}");
  }
}










