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
        boolean shift = e.isShiftDown();
        TableAdapter a = tgc.getAdapter();
				float xZoom = a.getXZoom();
        float newZoom = xZoom;
        int delta = e.getX()-previousX;		
				previousX = e.getX();		
        /*int visScope = tgc.getVisibleHorizontalScope();
        if(visScope > 10000) 
        {
          System.err.println("xZoom "+xZoom+" percent "+((float)10000/visScope)+" risultato "+xZoom*((float)10000/visScope));
          if(xZoom > 0.0)
            xZoom = (float)(xZoom*((double)1000/visScope));       
        }*/
        dddx+=delta;
        if(dddx>35)
        {
          if (xZoom>=0.9)
          {
            newZoom = Math.round(xZoom)+1;
          }
          else
          {
            newZoom = xZoom*(1/(1-xZoom));
            if(shift) newZoom*=(float)2.0;
          }  
          a.setXZoom(newZoom);
          dddx=0;
        }
        else if(dddx<-35)
        {
          if (xZoom>1.9) 
          {
            newZoom = Math.round(xZoom)-1;
          }
          else
          {
            newZoom = xZoom*(1/(1+xZoom));
           if(shift) newZoom*=(float)0.5;
          }
          a.setXZoom(newZoom);
          dddx=0;
        }
        
        /*
         if(dddx>35)
         {
           if (xZoom>=0.9)
             a.setXZoom(Math.round(xZoom * (1 + dddx / 35)));
           else
             a.setXZoom(1 / Math.round((1 / xZoom) * (1 + dddx / 35)));
           
           dddx=0;
         }
         else if(dddx<-35)
         {
           if (xZoom>=0.9)
             a.setXZoom(Math.round(xZoom * (1 - dddx / 35)));
           else
             a.setXZoom(1 / Math.round((1 / xZoom) * (1 - dddx / 35)));
           
           dddx=0;
         }	         
         */
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
    tgc.getDataModel().addListener( new TableDataListener(){
      public void valueChanged(int index1, int index2, boolean fromScroll){}
      public void pixelsChanged(int index1, int index2){}
      public void tableSetted(){repaint();}
      public void tableCleared(){}
      public void sizeChanged(int size, int oldSize){repaint();}
      public void tableUpdated(){repaint();}
      public void tableRange(float min_val, float max_val){}
      public void tableReference(int nRowsRef, int nColsRef, String typeRef, int indexRef, int onsetRef, int sizeRef){}
    });
  }

  public void paintComponent(Graphics g)
  {
    int xPosition;
    String indexString;
    Dimension d = getSize();
    Rectangle r = tgc.getGraphicDestination().getBounds();
    
    int index = tgc.getAdapter().getInvX( r.x);
    int visibleSize = tgc.getAdapter().getInvX( r.x+r.width);
    int tableSize = tgc.getFtsObject().getSize();
    int firstVisible = tgc.getFirstVisibleIndex();
    int visibleScope = tgc.getVisibleHorizontalScope();
    int indexStep = findBestIndexStep(visibleSize);
    int logicalIndex = firstVisible;
    int snappedIndex = 0; 
    
    
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
    /*draw always last index */
    if(visibleSize >= tableSize && snappedIndex != tableSize)
    {
      xPosition = tgc.getAdapter().getX(tableSize);
      indexString = ""+tableSize;
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




