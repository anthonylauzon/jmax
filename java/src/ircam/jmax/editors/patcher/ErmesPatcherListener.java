package ircam.jmax.editors.patcher;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class implement the listener for the patcher data.
 * It is an adapter that call the proper function in 
 * Ermes
 */

class ErmesPatcherListener implements FtsPatcherListener
{
  ErmesSketchWindow window;

  ErmesPatcherListener(ErmesSketchWindow window)
  {
    this.window = window;
  }

  public void objectAdded(FtsPatcherData data, FtsObject object)
  {
    // We handle only the case of object added thru 
    // pasting;
    // in reality, this should be the only way to add objects
    // to the sketch.


    if (window.isPasting())
      window.addPastedObject(object);
  }

  public void objectRemoved(FtsPatcherData data, FtsObject object)
  {
    // In general, objects are never really removed under the
    // FTS initiative, so you do not needed to implement
    // this method.
    // You should know that an object is removed and then re-added
    // each time is redefined; this is currently handled in the 
    // redefinition code in ErmesObject; if you decide to implement
    // this method, you must take care of this.
  }

  public void connectionAdded(FtsPatcherData data, FtsConnection connection)
  {
    // We handle only the case of connection added thru 
    // pasting;
    // in reality, this should be the only way to add connections 
    // to the sketch.

    if (window.isPasting())
      window.addPastedConnection(connection);
  }

  public void connectionRemoved(FtsPatcherData data, FtsConnection connection)
  {
    window.DeleteGraphicConnection(connection);
  }

  public void patcherChangedNumberOfInlets(FtsPatcherData data, int nins)
  {
    window.itsSketchPad.RedefineInChoice();
    window.itsSketchPad.redraw(); // ??? Why
  }

  public void patcherChangedNumberOfOutlets(FtsPatcherData data, int nouts)
  {
    window.itsSketchPad.RedefineOutChoice();
    window.itsSketchPad.redraw(); // ??? Why
  }

  public void patcherChanged(FtsPatcherData data)
  {
    // not implemented yet; it should redo the sketch content.
  }
}

