package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Class implementing the proxy of an FTS table object.
 */

public class FtsTableObject extends FtsObject implements FtsIntegerVectorObject, FtsDataObject
{
  /**
   * TableMessageHandler interpret the dedicated messages 
   * coming from FTS to the table object
   */

  class TableMessageHandler implements FtsMessageHandler
  {
    public void handleMessage(FtsMessage msg)
    {
      if (vector != null)
	vector.updateFromMessage(msg);
    }
  }

  MaxData data = null;
  FtsIntegerVector vector = null;
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
    
    FtsParse.parseObjectArguments(description, args);

    if (args.size() >= 3)
      vectorSize = Integer.parseInt(args.elementAt(2).toString());

    installMessageHandler(new TableMessageHandler());
  }

  /** Send the whole vector content to fts */

  public void saveVectorToFts()
  {
    this.vector.changed();
    FtsServer.getServer().syncToFts();
  }

  /** Load the whole vector content from fts */

  public void loadVectorFromFts()
  {
    this.vector.forceUpdate();
    FtsServer.getServer().syncToFts();
  }

  /** Bind this obejct to a vector; this means
    handle from now on all the bidirectional updates
    and similar things by redirecting them to the
    vector
    */

  public void bindVector(FtsIntegerVector vector)
  {
    this.vector = vector;
    vector.setObject(this);
    vector.setSize(vectorSize);
  }

  /** unbind it */

  public void unbindVector(FtsIntegerVector vector)
  {
    // We may have a pending save/update
    FtsServer.getServer().syncToFts();

    vector.setObject(null);
  }

  /** Tell MDA that this FTS object support a integerVector data object */

  public MaxDataType getObjectDataType()
  {
    return MaxDataType.getTypeByName("integerVector");
  }

  public MaxData getData()
  {
    return data;
  }

  public void setData(MaxData data)
  {
    this.data = data;
  }

  // Save the object *and* the table; it must be
  // sure to have the complete table, so it ask it 
  // to fts; this is orribly slow, of course
  // TO be done 

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






