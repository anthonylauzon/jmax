//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
  ErmesSketchPad sketch;

  ErmesPatcherListener(ErmesSketchPad sketch)
  {
    this.sketch = sketch;
  }

  public void objectAdded(FtsPatcherData data, FtsObject object)
  {
    // We handle only the case of object added thru 
    // pasting;
    // in reality, this should be the only way to add objects
    // to the sketch.
    if(PatcherClipboardManager.clipboardManager.isPasting())
      PatcherClipboardManager.clipboardManager.addPastedObject(object);
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

    if(PatcherClipboardManager.clipboardManager.isPasting())
      PatcherClipboardManager.clipboardManager.addPastedConnection(connection);
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

	  conn = sketch.getDisplayList().getGraphicConnectionFor(c);

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
    sketch.redraw(); // ??? Why
  }

  public void patcherChangedNumberOfOutlets(FtsPatcherData data, int nouts)
  {
    sketch.redraw(); // ??? Why
  }

  public void patcherChanged(FtsPatcherData data)
  {
    // not implemented yet; it should redo the sketch content.
  }

  public void patcherHaveMessage(String msg)
  {
    sketch.showMessage(msg);
  }
}



