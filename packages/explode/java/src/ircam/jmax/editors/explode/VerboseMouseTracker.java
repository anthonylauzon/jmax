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

import ircam.jmax.toolkit.*;
import java.awt.event.*;

/** A simple extension of the mouse tracker that writes its position in
 * the status bar... */
public class VerboseMouseTracker extends MouseTracker {

  public VerboseMouseTracker(PositionListener l)
  {
    super(l);
  }

  /**
   * The only redefined method */
  public void mouseMoved(MouseEvent e) 
  {  
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    egc.getStatusBar().post(egc.toolbar.getTool(), ""+
			   (egc.getAdapter().getInvX(e.getX()))+
			   ", "+
			   (egc.getAdapter().getInvY(e.getY())));
    ScoreBackground.pressKey(egc.getAdapter().getInvY(e.getY()), egc);
  }
}
