package ircam.jmax.editors.ermes;

import java.awt.*;

/**
 * The graphic circle used to identify the connection-splitting in autorouting
 */
public class ErmesConnCircle{
	int itsX, itsY;
	ErmesConnectionSet itsConnectionSet;
	boolean laidOut = false;
	
	public ErmesConnCircle(int theX, int theY, ErmesConnectionSet theConnectionSet){
		itsX = theX;
		itsY = theY;
		itsConnectionSet = theConnectionSet;
	}
	
	public int GetX() { return itsX;}
	public int GetY() { return itsY;}
	
	public void Paint(Graphics g){
		g.setColor(Color.black);
		g.fillOval(itsX, itsY, 4, 4);
	}

	public Dimension minimumSize() {
        return  preferredSize();
    }

    public Dimension preferredSize() {
        return new Dimension(4,4);
    }
	
	 public void MoveBy(int theDeltaH, int theDeltaV) {
		int j;
		ErmesObjInlet aInlet;
		ErmesObjOutlet aOutlet;
		
		if (theDeltaH == 0 && theDeltaV == 0) return;
		itsX+=theDeltaH; itsY+=theDeltaV;
		itsConnectionSet.GetSketchPad().repaint();	
	}
	
}
