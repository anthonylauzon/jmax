package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

// The graphic inlet used for the 'out' objects (ErmesObjOut.java)
// it draws itself as an arrow.
class ErmesObjOutInlet extends ErmesObjInlet {

  public ErmesObjOutInlet(int theInletNum, ErmesObject theOwner, int x_coord, int y_coord) 
  {
    super( theInletNum, theOwner, x_coord, y_coord);
  }

  public void Paint_specific( Graphics g) 
  {
    if ( !itsOwner.itsSketchPad.itsGraphicsOn) 
      return;

    g.setColor( Color.black);

    if (selected)
      {
	g.drawOval( itsX+1, itsY, getPreferredSize().width-2,getPreferredSize().height-4);
	g.drawLine( itsX+3, itsY+4, itsX+3, itsY+8);
      }
    else 
      {
	if(connected)
	  g.fillRect(itsX+1, itsY+7, getPreferredSize().width-2, 2);
	else
	  {	// not selected, not connected
	    g.drawLine( itsX+1, itsY+getPreferredSize().height/2, itsX + getPreferredSize().width/2, itsY + 1);
	    g.drawLine( itsX + getPreferredSize().width/2, itsY + 1, itsX+getPreferredSize().width - 1, itsY+getPreferredSize().height/2);
	    g.drawLine( itsX+3, itsY+7, itsX+3, itsY+8);
	  }
      }
  }
}
