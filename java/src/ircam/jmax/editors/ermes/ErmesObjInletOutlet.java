package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

/**
 * The graphic outlet associated with an 'in' object (ErmesObjIn.java).
 * It draws itself as an arrow.
 */
class ErmesObjInletOutlet extends ErmesObjOutlet{
	public ErmesObjInletOutlet(int theInletNum,  ErmesObject theOwner, int x_coord, int y_coord) {
		super(theInletNum, theOwner, x_coord, y_coord);
	}

	//--------------------------------------------------------
    //	Paint
    //--------------------------------------------------------
  public void Paint_specific(Graphics g){
    if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
    debug_count += 1;
    g.setColor(Color.black);		
    if(selected){
      g.drawOval(itsX+1, itsY+4, getPreferredSize().width-2,getPreferredSize().height-4);
      g.drawLine(itsX+3, itsY, itsX+3, itsY+4);
    }
    else 
      if (connected) g.fillRect(itsX+1, itsY, getPreferredSize().width-2, 2);
      else{	// not selected, not connected
	//g.fillOval(itsX+1, itsY+1, preferredSize().width-2,preferredSize().height-4);
	g.drawLine(itsX+1, itsY+getPreferredSize().height/2, 
		   itsX + getPreferredSize().width/2, itsY+getPreferredSize().height - 1);
	g.drawLine(itsX + getPreferredSize().width/2, itsY+getPreferredSize().height - 1,
		   itsX+getPreferredSize().width - 1, itsY+getPreferredSize().height/2);
	g.drawLine(itsX+3, itsY, itsX+3, itsY+1);
      }
  }
		
}
	
