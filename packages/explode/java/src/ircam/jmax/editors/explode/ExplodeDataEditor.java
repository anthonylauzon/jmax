//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The Explode Data Editor; represent the Explode editor in the Mda system.
 */


public class ExplodeDataEditor extends AbstractMaxDataEditor 
{
  private Explode window = null;
  

  public ExplodeDataEditor(ExplodeRemoteData data)
  {
    super(data);

    setExplodeWindow(new Explode(data));

    fireEditorReadyListeners();
  }


  public void setExplodeWindow(Explode window)
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
    // Should call a method of exploder, should not 
    // do the work here.

    if (! window.isVisible())
      window.setVisible(true);

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





