
package ircam.jmax.editors.bpf;

import ircam.jmax.toolkit.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.editors.bpf.renderers.*;
 /**
  * A graphic JPanel that represents a ruler containing time indications */
public class BpfRuler extends JPanel {

    public BpfRuler(Geometry geom, BpfPanel panel)
    {
	super();

	setOpaque(false);
	setDoubleBuffered(false);
	setFont(BpfPanel.rulerFont);
	fm = getFontMetrics(BpfPanel.rulerFont);
	this.geometry = geom;
	bpfPanel = panel;
	utilityBpfAdapter = new BpfAdapter(geometry, null);
	addMouseMotionListener(new MouseMotionListener(){
	    public void mouseDragged(MouseEvent e)
	    {		    
		if(!SwingUtilities.isLeftMouseButton(e)) return;
		int delta = e.getX()-previousX;		
		geometry.incrXZoom(delta);
		previousX = e.getX();		
	    } 
	    public void mouseMoved(MouseEvent e){} 
	});
	addMouseListener(new MouseListener(){
	    public void mousePressed(MouseEvent e)
	    {
		if(!SwingUtilities.isLeftMouseButton(e)) return;
		previousX = e.getX();
	    }
	    public void mouseClicked(MouseEvent e){}
	    public void mouseReleased(MouseEvent e){}
	    public void mouseEntered(MouseEvent e){}
	    public void mouseExited(MouseEvent e){}
	});
	geometry.addTranspositionListener( new TranspositionListener() {
	    public void transpositionChanged(int newValue)
		{
		    repaint();
		}
	});
    }

    public void paint(Graphics g)
    {
	int xPosition;
	int snappedTime;
	String timeString;
	Dimension d = getSize();
	Rectangle clip = g.getClipBounds();

	int logicalTime = -geometry.getXTransposition();
	int windowTime = bpfPanel.getMaximumVisibleTime();	    
	
	int timeStep = BpfBackground.findBestTimeStep(windowTime-logicalTime);
	
	//controll if the time string is too long (in this case draw one string on two)
	int stringLenght = fm.stringWidth(""+(windowTime));
	int delta = utilityBpfAdapter.getX(windowTime)-utilityBpfAdapter.getX(windowTime-timeStep);
	int k, stringWidth;
	if(stringLenght>delta-10) k = 2;
	else k=1;
		
	g.setColor(BpfPanel.violetColor);

	for (int i=logicalTime+timeStep; i<windowTime; i+=timeStep*k) 
	    {
		snappedTime = (i/timeStep)*timeStep;
		xPosition = utilityBpfAdapter.getX(snappedTime);
		g.drawLine(xPosition, d.height-4, xPosition, d.height);
			
		if(unity==MILLISECONDS_UNITY)		    
		    timeString = ""+snappedTime;
		else
		    timeString = ""+(float)(snappedTime/(float)1000.0);
			
		stringWidth = fm.stringWidth(timeString);
		g.drawString(timeString, xPosition-stringWidth/2, /*15*/12);		  
	    }
    }

    protected void processMouseEvent(MouseEvent e)
    {
	if (e.getClickCount()>1) 
	    {
		geometry.setXZoom(DEFAULT_XZOOM);
	    }
	else
	    super.processMouseEvent(e);
    }

    public Dimension getPreferredSize()
    { return rulerDimension; }
    
    public Dimension getMinimumSize()
    { return rulerDimension; }

    //--- Ruler fields
    Dimension rulerDimension = new Dimension(200, RULER_HEIGHT);
    FontMetrics fm;
    int unity = MILLISECONDS_UNITY;
    BpfAdapter utilityBpfAdapter;
    Geometry geometry;
    BpfPanel bpfPanel;
    int previousX;
    public final static int DEFAULT_XZOOM      = 20; 
    public final static int MILLISECONDS_UNITY = 0; 
    public final static int SECONDS_UNITY      = 1; 
    public final static int SAMPLES_UNITY      = 2; 

    public final static int RULER_HEIGHT = 20; 
}    




