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

package ircam.jmax.editors.sequence;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The Sequence Data Editor; represent the Sequence editor in the Mda system.
 */


public class SequenceDataEditor extends AbstractMaxDataEditor 
{
  private Sequence window = null;
  

  public SequenceDataEditor(SequenceRemoteData data)
  {
    super(data);

    setSequenceWindow(new Sequence(data));

    fireEditorReadyListeners();
  }


  public void setSequenceWindow(Sequence window)
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
    // Should call a method of sequencer, should not 
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





