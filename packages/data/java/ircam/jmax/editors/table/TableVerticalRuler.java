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

import java.awt.geom.AffineTransform;
/**
* A graphic JPanel that represents a ruler containing time indications */
public class TableVerticalRuler extends JPanel 
{	
  public TableVerticalRuler( TableGraphicContext gc)
  {
    super();
    setLayout(null);
    setOpaque(false);
    setFont(Tabler.displayFont);
    fm = getFontMetrics( Tabler.displayFont);
    this.tgc = gc;
    setSize(rulerDimension);
        
    tgc.getDataModel().addListener( new TableDataListener(){
      public void valueChanged(int index1, int index2, boolean fromScroll){}
      public void pixelsChanged(int index1, int index2){}
      public void tableSetted(){repaint();}
      public void tableCleared(){}
      public void sizeChanged(int size, int oldSize){repaint();}
      public void tableUpdated(){repaint();}
      public void tableRange(float min_val, float max_val){repaint();}
      public void tableReference(int nRowsRef, int nColsRef, String typeRef, int indexRef, int onsetRef, int sizeRef){}
    });
  }
  public void paintComponent(Graphics g)
  {
    Graphics2D g2d = (Graphics2D)g;
    Dimension d = getSize();
    
    g.setColor(getBackground());
    g.fillRect(0, 0, d.width, d.height);
    
    g.setColor(Color.lightGray); 
    g.drawLine(1, 0, d.width-1, 0);
    g.setColor(Color.white);
    g.drawLine(1, 1, d.width-1, 1);
    
    AffineTransform at = new AffineTransform();
    int yPosition, stringWidth;
    String valueString;
    float max = tgc.getVerticalMaxValue();
    float min = tgc.getVerticalMinValue();
    float valueRange = max-min;
    int yzero = tgc.getAdapter().getY(0);
    float valueStep = (float)findBestValueStep(valueRange);
    int graphStep = yzero - tgc.getAdapter().getY(valueStep);
         
    if(graphStep <= 0) return;
    
    g.setColor(violetColor);
    for(int i = yzero-graphStep ; i > 0; i-=graphStep)
      g.drawLine(d.width-1, i, d.width, i);
    
    for(int i = yzero+graphStep ; i < d.height; i+=graphStep)
      g.drawLine(d.width-1, i, d.width, i);
        
    valueString = ""+0.0;
    stringWidth = fm.stringWidth(valueString);
    at.setToRotation(-Math.PI/2.0, d.width-1, yzero+stringWidth/2+1);    
    g2d.setTransform(at);
    g2d.drawString(""+0.0, d.width-1, yzero+stringWidth/2+1);
        
    int k;
    if(fm.stringWidth(""+valueStep) <= graphStep-10) k = 1;
    else 
      if(fm.stringWidth(""+valueStep) <= 2*graphStep-10) k=2;
    else k = 4;
        
    float value = (float)(k*valueStep);
    for(int i = yzero-k*graphStep ; i > 0; i-=k*graphStep)
    {
      valueString = Displayer.numberFormat.format(value);
      stringWidth = fm.stringWidth(valueString);
      at.setToRotation(-Math.PI/2.0, d.width-1, i+stringWidth/2+1);    
      g2d.setTransform(at);
      g2d.drawString(valueString, d.width-1, i+stringWidth/2+1);
      value+= (float)(k*valueStep);      
    }
    value = -(float)(k*valueStep);
    for(int i = yzero+k*graphStep ; i < d.height; i+=k*graphStep)
    {
      valueString = Displayer.numberFormat.format(value);
      stringWidth = fm.stringWidth(valueString);
      at.setToRotation(-Math.PI/2.0, d.width-1, i+stringWidth/2+1);    
      g2d.setTransform(at);
      g2d.drawString(valueString, d.width-1, i+stringWidth/2+1);
      value-= (float)(k*valueStep);
    }
  }

  public static double findBestValueStep(float valueRange) 
  {
    double pow;
    if(valueRange > 4.0)
    {
      pow = 1.0;    
      while ((int)(valueRange/pow)>0) 
        pow *= 10.0;
    
      pow = pow/10.0;
      if (valueRange/pow < 5.0) pow = pow/5.0;
      if ((int)pow == 0.0) pow = 1.0;
    }
    else
      pow = 0.1;
    
    return pow;
  }
  
  /*protected void processMouseEvent(MouseEvent e)
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
  }*/

  public Dimension getPreferredSize()
  { return rulerDimension; }

  public Dimension getMaximumSize()
 { return maxDimension; }

  
  Dimension rulerDimension = new Dimension( RULER_WIDTH, TablePanel.PANEL_HEIGHT);
  Dimension maxDimension = new Dimension( RULER_WIDTH, 10000);
  static public Color violetColor = new Color(102, 102, 153);
  FontMetrics fm;
  TableGraphicContext tgc;
  int previousX;
  public final static int RULER_WIDTH = 15; 
  int dddx = 0;
}    




