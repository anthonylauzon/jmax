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

  FtsTableObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);

    FtsServer.getServer().newObject(parent, this, description);

    //Then, look for the size in the argument

    Vector args;

    args = new Vector();
    
    FtsParse.parseObjectArguments(description, args);

    if ((args.size() >= 3) && (args.elementAt(0) instanceof Integer))
      vectorSize = ((Integer) args.elementAt(0)).intValue();

    if (parent.isOpen())
      FtsServer.getServer().syncToFts();
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
    vector.setObject(null);
    FtsServer.getServer().syncToFts();
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






