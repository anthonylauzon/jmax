
package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.menus.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
 /**
  * A graphic JPanel that represents a ruler containing time indications */
public class SequenceRuler extends PopupToolbarPanel implements HighlightListener{

    public SequenceRuler(Geometry geom, SequencePanel panel)
    {
	super();
	setOpaque(false);
	setDoubleBuffered(false);
	setFont(SequencePanel.rulerFont);
	fm = getFontMetrics(SequencePanel.rulerFont);
	this.geometry = geom;
	sequencePanel = panel;
	utilityPartitionAdapter = new PartitionAdapter(geometry, null);
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
	popup = new RulerPopupMenu(this);
    }

    public void paint(Graphics g)
    {
	int xPosition;
	int snappedTime;
	String timeString;
	Dimension d = getSize();
	Rectangle clip = g.getClipRect();

	int logicalTime = -geometry.getXTransposition();
	int windowTime = sequencePanel.getMaximumVisibleTime();	    
	
	int timeStep = ScoreBackground.findBestTimeStep(windowTime-logicalTime);
	
	//controll if the time string is too long (in this case draw one string on two)
	int stringLenght = fm.stringWidth(""+(logicalTime+timeStep));
	int delta = utilityPartitionAdapter.getX(logicalTime+timeStep)-utilityPartitionAdapter.getX(logicalTime);
	int k, stringWidth;
	if(stringLenght>delta-10) k = 2;
	else k=1;
		
	g.setColor(SequencePanel.violetColor);

	if(hh)//during highlighting
	    {
		for (int i=logicalTime+timeStep; i < windowTime; i+=timeStep*k) 
		    {
			snappedTime = (i/timeStep)*timeStep;
			xPosition = utilityPartitionAdapter.getX(snappedTime)+3+TrackContainer.BUTTON_WIDTH;

			if(unity==MILLISECONDS_UNITY)		    
			    timeString = ""+snappedTime;
			else
			    timeString = ""+(float)(snappedTime/(float)1000.0);		
			stringWidth = fm.stringWidth(timeString);
			
			if((xPosition <= clip.x+clip.width+20)&&(xPosition+stringWidth >= clip.x-20))
			    {				
				g.drawLine(xPosition, d.height-4, xPosition, d.height);				
				g.drawString(timeString, xPosition-stringWidth/2, /*15*/12);		  
			    }
		    }

		int hhX = utilityPartitionAdapter.getX(hhTime)+3+TrackContainer.BUTTON_WIDTH;
		g.setColor(Color.red);
		g.fillRect(hhX-1, 1, 3, d.height-2);

		hh = false;				
	    }
	else
	    {
		for (int i=logicalTime+timeStep; i<windowTime; i+=timeStep*k) 
		    {
			snappedTime = (i/timeStep)*timeStep;
			xPosition = utilityPartitionAdapter.getX(snappedTime)+3+TrackContainer.BUTTON_WIDTH;
			g.drawLine(xPosition, d.height-4, xPosition, d.height);
			
			if(unity==MILLISECONDS_UNITY)		    
			    timeString = ""+snappedTime;
			else
			    timeString = ""+(float)(snappedTime/(float)1000.0);
			
			stringWidth = fm.stringWidth(timeString);
			g.drawString(timeString, xPosition-stringWidth/2, /*15*/12);		  
		    }
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

    public String getUnityName()
    {
	return unityName;
    }
    public void setUnityName(String name)
    {
	if(name.equals(unityName)) return;

	if(name.equals("Milliseconds"))
	    unity = MILLISECONDS_UNITY;
	else if(name.equals("Seconds"))
	    unity = SECONDS_UNITY;
	else if(name.equals("Samples"))
	    unity = SAMPLES_UNITY;

	unityName = name;

	repaint();
    }

    public Dimension getPreferredSize()
    { return rulerDimension; }
    
    public Dimension getMinimumSize()
    { return rulerDimension; }

    public JPopupMenu getMenu()
    {
	popup.update();
	return popup;
    }
    //------- HighlightListener interface
    public void highlight(Enumeration elements, double time)
    {
	//---------
	hh = true;
	int hhX = utilityPartitionAdapter.getX(hhTime)+3+TrackContainer.BUTTON_WIDTH;
	int timeX = utilityPartitionAdapter.getX(time)+3+TrackContainer.BUTTON_WIDTH;
	repaint(new Rectangle(hhX-1, 1, timeX-hhX+3, getSize().height-2));
    
	hhTime = time;
    }
    //--- Ruler fields
    boolean hh = false;
    double hhTime;

    Dimension rulerDimension = new Dimension(200, RULER_HEIGHT);
    FontMetrics fm;
    String unityName = "Milliseconds";
    int unity = MILLISECONDS_UNITY;
    RulerPopupMenu popup;
    PartitionAdapter utilityPartitionAdapter;
    Geometry geometry;
    SequencePanel sequencePanel;
    int previousX;
    public final static int DEFAULT_XZOOM      = 20; 
    public final static int MILLISECONDS_UNITY = 0; 
    public final static int SECONDS_UNITY      = 1; 
    public final static int SAMPLES_UNITY      = 2; 

    public final static int RULER_HEIGHT = 20; 
}    




