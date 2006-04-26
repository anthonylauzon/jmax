//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
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
public class SequenceRuler extends PopupToolbarPanel implements HighlightListener
{	
  public SequenceRuler( Geometry geom, ScrollManager scm)
  {
    super();
    setLayout(null);//%%%%
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
  
    marker = new RulerMarker( this);
    marker.setBounds( -10, -10, 4, RULER_HEIGHT-2);
  }

  public void paintComponent(Graphics g)
  {
    int xPosition;
    int snappedTime;
    String timeString;
    Dimension d = getSize();
    Rectangle clip = g.getClipBounds();
    
    int logicalTime = -geometry.getXTransposition();
    int windowTime = (int) (utilityPartitionAdapter.getInvX(d.width) - utilityPartitionAdapter.getInvX(ScoreBackground.KEYEND))-1;	
    int timeStep = ScoreBackground.findBestTimeStep(windowTime/*-logicalTime*/);
    
    //controll if the time string is too long (in this case draw one string on two)
    int stringLenght = fm.stringWidth(""+(logicalTime+timeStep));
    int delta = utilityPartitionAdapter.getX(logicalTime+timeStep)-utilityPartitionAdapter.getX(logicalTime);
    int k, stringWidth;
    if(stringLenght>delta-10) k = 2;
    else k=1;
		
    g.setColor(SequencePanel.violetColor);
    for (int i=logicalTime+timeStep; i<logicalTime+windowTime; i+=timeStep*k) 
    {
      snappedTime = (i/timeStep)*timeStep;
      xPosition = utilityPartitionAdapter.getX(snappedTime)+getXIndentation();
      
      if(unity==MILLISECONDS_UNITY)		    
        timeString = ""+snappedTime;
      else
        timeString = ""+(float)(snappedTime/(float)1000.0);
        
      stringWidth = fm.stringWidth(timeString);
      g.drawString(timeString, xPosition-stringWidth/2, RULER_HEIGHT-3);		  
    }
  }
  int getXIndentation()
  {
    if( scrollManager instanceof SequencePanel)
      return 3+TrackContainer.BUTTON_WIDTH;
    else
      return 1;
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
  double highTime;
  public void highlight(Enumeration elements, double time)
  {
    highTime = time;
    SwingUtilities.invokeLater(new Runnable() {
	    public void run()
      { 
        if(!hh)
        {
          add( marker);
          marker.setVisible(true);
          //---------
          hh = true;
        }
        marker.setTime(highTime);
      }
    });
  }
  //--- Ruler fields
  boolean hh = false;

  Dimension rulerDimension = new Dimension(SequenceWindow.DEFAULT_WIDTH, RULER_HEIGHT);
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
  
  public final static int RULER_HEIGHT = 15; 
  
  private RulerMarker marker;
  
  class RulerMarker extends JPanel
  {
    SequenceRuler ruler;
    double time;
    RulerMarker(SequenceRuler rul)
    {
      super();
      this.ruler = rul;
      ruler.geometry.addTranspositionListener( new TranspositionListener() {
        public void transpositionChanged(int newValue)
		    {
          int timeX = ruler.utilityPartitionAdapter.getX(time) + ruler.getXIndentation(); 
          setLocation( timeX - 1, 1);
        }
      });
      ruler.geometry.addZoomListener( new ZoomListener() {
        public void zoomChanged(float newZoom, float oldZoom)
		    {
          int timeX = ruler.utilityPartitionAdapter.getX(time) + ruler.getXIndentation(); 
          setLocation( timeX - 1, 1);
        }
      });
      setPreferredSize(new Dimension( 4, RULER_HEIGHT-2));
      setMaximumSize(new Dimension( 4, RULER_HEIGHT-2));
    }
    
    void setTime(double time)
    {      
      this.time = time;
      int timeX = ruler.utilityPartitionAdapter.getX(time) + ruler.getXIndentation(); 
      setLocation( timeX, 1);    
      repaint();
    }
    
    public void paintComponent(Graphics g)
    {
      Dimension d = this.getSize();
      g.setColor( Color.green);
      g.fillRect( 0, 0, d.width-1, d.height-1);
      g.setColor( Color.darkGray);
      g.draw3DRect( 0, 0, d.width-1, d.height-1, true);
    }    
  }
}    




