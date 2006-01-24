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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
/**
* A graphic JPanel that represents a ruler containing time indications */
public class TableRuler extends JPanel 
{	
  public TableRuler( TableGraphicContext gc)
  {
    super();
    setLayout(null);
    setOpaque(false);
    setDoubleBuffered(false);
    setFont(Tabler.displayFont);
    fm = getFontMetrics( Tabler.displayFont);
    this.tgc = gc;
    
    addMouseMotionListener(new MouseMotionListener(){
			public void mouseDragged(MouseEvent e)
		  {		    
				if(!SwingUtilities.isLeftMouseButton(e)) return;
        TableAdapter a = tgc.getAdapter();
				float xZoom = a.getXZoom();
        int delta = e.getX()-previousX;		
				previousX = e.getX();		
			
        dddx+=delta;
        if(dddx>35)
        {
          if (xZoom>=0.9)
            a.setXZoom(Math.round(xZoom)+1);
          else
            a.setXZoom(xZoom*(1/(1-xZoom)));
             
          dddx=0;
        }
        else if(dddx<-35)
        {
          if (xZoom>1.9) 
            a.setXZoom(Math.round(xZoom)-1);
          else
            a.setXZoom(xZoom*(1/(1+xZoom)));
          dddx=0;
        }	
      } 
			public void mouseMoved(MouseEvent e){} 
		});
    addMouseListener(new MouseListener(){
			public void mousePressed(MouseEvent e)
		  {
				if(!SwingUtilities.isLeftMouseButton(e)) return;
				previousX = e.getX();
        dddx = 0;
			}
			public void mouseClicked(MouseEvent e){}
			public void mouseReleased(MouseEvent e){}
			public void mouseEntered(MouseEvent e){}
			public void mouseExited(MouseEvent e){}
		});
    tgc.getAdapter().addXZoomListener(new ZoomListener() {
			public void zoomChanged(float zoom, float oldZoom)
      {
        repaint();
      }
    });
    tgc.getAdapter().addTranspositionListener( new TranspositionListener() {
			public void transpositionChanged(int newValue)
		  {
				repaint();
			}
		});
  }

  public void paintComponent(Graphics g)
  {
    int xPosition;
    int snappedIndex;
    String indexString;
    Dimension d = getSize();
    Rectangle r = tgc.getGraphicDestination().getBounds();
    Rectangle clip = g.getClipBounds();
    
    int index = tgc.getAdapter().getInvX( r.x);
    int visibleSize = tgc.getAdapter().getInvX( r.x+r.width);
    int tableSize = tgc.getFtsObject().getSize();
    int firstVisible = tgc.getFirstVisibleIndex();
    int visibleScope = tgc.getVisibleHorizontalScope();
    int indexStep = findBestIndexStep(visibleSize);
    int logicalIndex = firstVisible;

    int stringLenght = fm.stringWidth(""+(logicalIndex+indexStep));
    int delta = tgc.getAdapter().getX(logicalIndex+indexStep)-tgc.getAdapter().getX(logicalIndex);
    int k, stringWidth;
    if(stringLenght>delta-10) k = 2;
    else k=1;
		
    g.setColor(violetColor);
    for (int i=logicalIndex+indexStep; i<logicalIndex+visibleScope; i+=indexStep*k) 
    {
      snappedIndex = (i/indexStep)*indexStep;
      xPosition = tgc.getAdapter().getX(snappedIndex);
      indexString = ""+snappedIndex;
      stringWidth = fm.stringWidth(indexString);
      g.drawString(indexString, xPosition-stringWidth/2, RULER_HEIGHT-3);		  
    }
  }

  public static int findBestIndexStep(int indexScope) 
  {
    int pow = 1;    
    while (indexScope/pow>0) 
      pow *= 10;
    
    pow = pow/10;
    if (indexScope/pow < 5) pow = pow/5;
    if (pow == 0) return 1;
    return pow;
  }
  
  protected void processMouseEvent(MouseEvent e)
  {
    if (e.getClickCount()>1) 
    {
      if( e.isShiftDown())
        tgc.getAdapter().zoomToWindow();
      else	
        tgc.getAdapter().setDefaultZooms();
    }
    else
      super.processMouseEvent(e);
  }

  public Dimension getPreferredSize()
  { return rulerDimension; }

  public Dimension getMinimumSize()
  { return rulerDimension; }

  Dimension rulerDimension = new Dimension(TablePanel.PANEL_WIDTH, RULER_HEIGHT);
  static public Color violetColor = new Color(102, 102, 153);
  FontMetrics fm;
  TableGraphicContext tgc;
  int previousX;
  public final static int RULER_HEIGHT = 15; 
  int dddx = 0;
}    




