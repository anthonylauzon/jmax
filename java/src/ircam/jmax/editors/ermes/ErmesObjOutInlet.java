package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

/**
 * The graphic inlet used for the 'out' objects (ErmesObjOut.java)
 * it draws itself as an arrow.
 */
class ErmesObjOutInlet extends ErmesObjInlet{
	public ErmesObjOutInlet(int theInletNum, ErmesObject theOwner, int x_coord, int y_coord) {
		super(theInletNum, theOwner, x_coord, y_coord);
	}

	public void Paint(Graphics g) {
	  if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
		g.setColor(Color.black);
		if (selected){
			g.drawOval(itsX+1, itsY, preferredSize().width-2,preferredSize().height-4);
			g.drawLine(itsX+3, itsY+4, itsX+3, itsY+8);
		}
		else 
			if(connected) g.fillRect(itsX+1, itsY+7, preferredSize().width-2, 2);
			else{	// not selected, not connected
				g.drawLine(	itsX+1, itsY+preferredSize().height/2, 
							itsX + preferredSize().width/2, itsY + 1);
				g.drawLine(	itsX + preferredSize().width/2, itsY + 1,
							itsX+preferredSize().width - 1, itsY+preferredSize().height/2);
				//g.fillOval(itsX+1, itsY+3, preferredSize().width-2,preferredSize().height-4);
				g.drawLine(itsX+3, itsY+7, itsX+3, itsY+8);
			}
	}
}
	
