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
	public void Paint(Graphics g){
	  if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
		debug_count += 1;
		if (debug_count % ErmesSketchPad.DEBUG_COUNT == 0) {
		  /* System.out.println("uscito in ErmesObjOutlet.paint"); */
		}
		g.setColor(Color.black);		
		if(selected){
			g.drawOval(itsX+1, itsY+4, preferredSize().width-2,preferredSize().height-4);
			g.drawLine(itsX+3, itsY, itsX+3, itsY+4);
		}
		else 
			if (connected) g.fillRect(itsX+1, itsY, preferredSize().width-2, 2);
			else{	// not selected, not connected
				//g.fillOval(itsX+1, itsY+1, preferredSize().width-2,preferredSize().height-4);
				g.drawLine(	itsX+1, itsY+preferredSize().height/2, 
							itsX + preferredSize().width/2, itsY+preferredSize().height - 1);
				g.drawLine(	itsX + preferredSize().width/2, itsY+preferredSize().height - 1,
							itsX+preferredSize().width - 1, itsY+preferredSize().height/2);
				g.drawLine(itsX+3, itsY, itsX+3, itsY+1);
			}
	}
		
}
	
