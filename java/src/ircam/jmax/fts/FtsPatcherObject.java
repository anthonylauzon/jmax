package ircam.jmax.fts;

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


  public FtsPatcherObject(FtsContainerObject parent, String description, int objId)
  {
    super(parent, "patcher", description, objId);
  }


  /**
   * Special method to redefine a patcher without looosing its content 
   */

  public void redefinePatcher(String description)
  {
    Fts.getServer().redefinePatcherObject(this, description);
    Fts.getServer().syncToFts();

    this.description = description;

    setDirty();
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
	    writer.print("set obj(" + obj.getObjectId() + ")" + " [");

	    obj.saveAsTcl(writer);

	    writer.println("]");

	    // If the object have data, save the data

 	    if ((! (obj instanceof FtsContainerObject)) && (obj instanceof FtsObjectWithData))
 	      {
 		FtsDataObject data;

		data = (FtsDataObject) ((FtsObjectWithData) obj).getData();
		    
		writer.print("setData $obj(" + obj.getObjectId() + ")" + " [");
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










