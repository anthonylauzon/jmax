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

import ircam.jmax.toolkit.*;
import java.awt.event.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.renderers.*;

/** A simple extension of the mouse tracker that writes its position in
 * the status bar... and presses the key in the little keyboard*/
public class MidiMouseTracker extends MouseTracker {

  public MidiMouseTracker(PositionListener l)
  {
    super(l);
  }

  /**
   * The only redefined method */
  public void mouseMoved(MouseEvent e) 
  {  
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;

    if(!(egc.getAdapter() instanceof MonoDimensionalAdapter))//only for midi editor
	{
	    egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), ""+
				    ListPanel.numberFormat.format(egc.getAdapter().getInvX(e.getX()))+
				    ", "+
				    (egc.getAdapter().getInvY(e.getY())));
    
	    //press keys in the pianoroll representation
	    
	    if(((PartitionAdapter)egc.getAdapter()).getViewMode()==MidiTrackEditor.PIANOROLL_VIEW)
		if (egc.getTrack().getTrackDataModel().containsType(AmbitusValue.info))
		    ScoreBackground.pressKey(egc.getAdapter().getInvY(e.getY()), egc);
	}
    else
	egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), ""+
				    ListPanel.numberFormat.format(egc.getAdapter().getInvX(e.getX())));
  }
}




