package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Class implementing the proxy of an FTS table object.
 */

public class FtsTableObject extends FtsObject implements FtsObjectWithData
{
  /**
   * TableMessageHandler interpret the dedicated messages 
   * coming from FTS to the table object
   */

  class TableMessageHandler implements FtsMessageHandler
  {
    public void handleMessage(FtsMessage msg)
    {
      vector.updateFromMessage(msg);
    }
  }

  FtsIntegerVector vector;
  int vectorSize = 128;

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */


  FtsTableObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);


    //Then, look for the size in the argument

    Vector args;

    args = new Vector();
    
    FtsParse.parseAtoms(description, args);

    if (args.size() >= 3)
      vectorSize = Integer.parseInt(args.elementAt(2).toString());
    else
      {
	try
	  {
	    if (args.size() >= 2) 
	      vectorSize = Integer.parseInt(args.elementAt(1).toString());
	  }
	catch (NumberFormatException e)
	  {
	  }
      }
	
    vector = new FtsIntegerVector(this, vectorSize);

    installMessageHandler(new TableMessageHandler());
  }

  // FtsObjectWithData implementation

  public MaxData getData()
  {
    this.vector.forceUpdate();
    Fts.getServer().syncToFts();

    return vector;
  }

  public void setData(MaxData data) throws FtsException
  {
    // We may have a pending save/update
    // Sync, than discard the vector

    Fts.getServer().syncToFts();
    vector.setObject(null);

    // set the new vector, bind to this object
    // set the vector size  and sent it to FTS

    vector = (FtsIntegerVector) data;
    vector.setObject(this);
    vector.setSize(vectorSize);
    this.vector.changed();
  }


  public void delete()
  {
    Mda.dispose(vector);
    super.delete();
  }

  // Save the object 
  // Data is saved by the surrounding saved, patcher or selection
  // because need more context

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






