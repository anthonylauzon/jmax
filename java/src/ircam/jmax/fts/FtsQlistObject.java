package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Class implementing the proxy of an FTS qlist object.
 */

public class FtsQlistObject extends FtsObject implements FtsAtomListObject, FtsDataObject
{
  /**
   * QlistMessageHandler interpret the dedicated messages 
   * coming from FTS to the qlist object
   */

  class QlistMessageHandler implements FtsMessageHandler
  {
    public void handleMessage(FtsMessage msg)
    {
      if (list != null)
	list.updateFromMessage(msg);
    }
  }

  MaxData data = null;
  FtsAtomList list = null;

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  FtsQlistObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);

    FtsServer.getServer().newObject(parent, this, description);
    
    installMessageHandler(new QlistMessageHandler());

    if (parent.isOpen())
      FtsServer.getServer().syncToFts();
  }


  FtsQlistObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);

    installMessageHandler(new QlistMessageHandler());
  }

  /** Send the whole list content to fts */

  public void saveAtomListToFts()
  {
    this.list.changed();
    FtsServer.getServer().syncToFts();
  }

  /** Load the whole list content from fts */

  public void loadAtomListFromFts()
  {
    this.list.forceUpdate();
    FtsServer.getServer().syncToFts();
  }

  /** Bind this obejct to a list; this means
    handle from now on all the bidirectional updates
    and similar things by redirecting them to the
    list
    */

  public void bindList(FtsAtomList list)
  {
    this.list = list;
    list.setObject(this);
  }

  /** unbind it */

  public void unbindList(FtsAtomList list)
  {
    // We may have a pending save/update

    FtsServer.getServer().syncToFts();
    list.setObject(null);
  }

  /** Tell MDA that this FTS object support a AtomList data object */

  public MaxDataType getObjectDataType()
  {
    return MaxDataType.getTypeByName("atomList");
  }

  public MaxData getData()
  {
    return data;
  }

  public void setData(MaxData data)
  {
    this.data = data;
  }

  // Save the object *and* the qlist; it must be
  // sure to have the complete qlist, so it ask it 
  // to fts; this is orribly slow, of course
  // TO be done 

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






