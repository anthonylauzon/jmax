//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.patcher;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

import ircam.jmax.editors.patcher.objects.*;

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
    // redefinition code in GraphicObject; if you decide to implement
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
    final FtsConnection c = connection;

    /* Do the delete thru an invoke later, so that the code
       is executed within the event thread and there are no
       races with the paint work; a way to serialize editing on the
       display list.
       */

    SwingUtilities.invokeLater(new Runnable() {
      public void run()
	{
	  GraphicConnection conn;

	  conn = window.itsSketchPad.getDisplayList().getGraphicConnectionFor(c);

	  // conn may be null if the connection has been delete by Ermes
	  // first; a little hack, the whole deleting business should be cleaned up.
    
	  if (conn != null)
	    {
	      conn.redraw();

	      if (conn.isSelected())
		ErmesSelection.patcherSelection.deselect(conn);

	      conn.delete();
	    }
	}
    });
  }

  public void patcherChangedNumberOfInlets(FtsPatcherData data, int nins)
  {
    window.itsSketchPad.redraw(); // ??? Why
  }

  public void patcherChangedNumberOfOutlets(FtsPatcherData data, int nouts)
  {
    window.itsSketchPad.redraw(); // ??? Why
  }

  public void patcherChanged(FtsPatcherData data)
  {
    // not implemented yet; it should redo the sketch content.
  }

  public void patcherHaveMessage(String msg)
  {
    window.showMessage(msg);
  }
}

