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
  int vectorSize;

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

    installMessageHandler(new TableMessageHandler());
  }

  /*
   * Size is now a property, in principle set only
   * from the fts object, but it may become bidirectional later.
   */

  protected void builtinPropertyNames(Vector names)
  {
    names.addElement("size");
    super.builtinPropertyNames(names);
  }

  protected boolean builtinPut(String name, Object value)
  {
    if (name.equals("size"))
      {
	vectorSize = ((Integer)value).intValue();
	
	if (vector == null)
	  vector = new FtsIntegerVector(this, vectorSize);
	else
	  vector.setSize(vectorSize);

	return true;
      }
    else
      return super.builtinPut(name, value);
  }

  protected Object builtinGet(String name)
  {
    if (name.equals("size"))
      return new Integer(this.vectorSize);
    else
      return super.builtinGet(name);
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






