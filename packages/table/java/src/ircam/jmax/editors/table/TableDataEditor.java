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
package ircam.jmax.editors.table;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The Table Data Editor; represent the Table editor in the Mda system.
 */

public class TableDataEditor extends AbstractMaxDataEditor
{
  private Tabler window = null;
  
  public TableDataEditor(FtsIntegerVector data)
  {
    super(data);

    setTableWindow(new Tabler(data));
    fireEditorReadyListeners();
  }

  public void setTableWindow(Tabler window)
  {
    this.window = window;
  }

  /**
   * This method is called when an edit is asked
   * for the data the editor is already editing; the semantic
   * is editor dependent.
   */

  public void reEdit()
  {
    // Should call a method of tabler, should not 
    // do the work here.

    if (! window.isVisible())
      window.setVisible(true);

    window.itsData.forceUpdate();
    window.toFront();
  }

  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  public void quitEdit()
  {
    if (window != null)
      {
	window.setVisible(false);
	window.dispose();
      }
  }
}








