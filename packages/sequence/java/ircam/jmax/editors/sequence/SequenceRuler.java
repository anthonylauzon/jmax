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

  public SequenceRuler( Geometry geom, ScrollManager scm)
  {
    super();
    setOpaque(false);
    setDoubleBuffered(false);
    setFont(SequencePanel.rulerFont);
    fm = getFontMetrics( SequencePanel.rulerFont);
    this.geometry = geom;
    scrollManager = scm;
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
    Rectangle clip = g.getClipBounds();
    
    int logicalTime = -geometry.getXTransposition();
    int windowTime = scrollManager.getMaximumVisibleTime();	    
    
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
	g.setColor(Color.green);
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
  Rectangle paintRect = new Rectangle(); 
  public void highlight(Enumeration elements, double time)
  {
    //---------
    hh = true;
    
    int hhX = utilityPartitionAdapter.getX(hhTime)+3+TrackContainer.BUTTON_WIDTH;
    int timeX = utilityPartitionAdapter.getX(time)+3+TrackContainer.BUTTON_WIDTH;
    
    if(time >= hhTime)
      paintRect.setBounds(hhX-1, 1, timeX-hhX+3, getSize().height-2);
    else
      paintRect.setBounds(timeX-1, 1, hhX-timeX+3, getSize().height-2);
    
    repaint(paintRect);
    
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
  ScrollManager scrollManager;
  int previousX;
  public final static int DEFAULT_XZOOM      = 20; 
  public final static int MILLISECONDS_UNITY = 0; 
  public final static int SECONDS_UNITY      = 1; 
  public final static int SAMPLES_UNITY      = 2; 

  public final static int RULER_HEIGHT = 20; 
}    




