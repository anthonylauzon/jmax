package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Class implementing the proxy of an FTS qlist object.
 */

public class FtsQlistObject extends FtsObject  implements FtsObjectWithData
{
  /**
   * QlistMessageHandler interpret the dedicated messages 
   * coming from FTS to the qlist object
   */

  class QlistMessageHandler implements FtsMessageHandler
  {
    public void handleMessage(FtsMessage msg)
    {
      list.updateFromMessage(msg);
    }
  }

  FtsAtomList list = new FtsAtomList(this);

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  FtsQlistObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);

    installMessageHandler(new QlistMessageHandler());
  }

  // FtsObjectWithData implementation

  public MaxData getData()
  {
    this.list.forceUpdate();

    // (fd) already done in FtsAtomList::forceUpdate, but twice is better than once...
    Fts.sync();

    return list;
  }

  public void setData(MaxData data) throws FtsException
  {
    // We may have a pending save/update
    // Sync, than discard the list

    Fts.sync();
    list.setObject(null);

    // set the new list, bind to this object
    // and sent it to FTS

    list = (FtsAtomList) data;
    list.setObject(this);
    this.list.changed();
  }

  public void delete()
  {
    Mda.dispose(list);
    super.delete();
  }
}






