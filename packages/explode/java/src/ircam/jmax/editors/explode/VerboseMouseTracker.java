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

    egc.getStatusBar().post(Explode.toolbar.getTool(), ""+
			   (egc.getAdapter().getInvX(e.getX()))+
			   ", "+
			   (egc.getAdapter().getInvY(e.getY())));
    ScoreBackground.pressKey(egc.getAdapter().getInvY(e.getY()), egc);
  }
}
