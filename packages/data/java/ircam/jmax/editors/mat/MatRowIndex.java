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

package ircam.jmax.editors.mat;

import ircam.jmax.toolkit.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
/**
* A graphic JPanel that represents a ruler containing time indications */
public class MatRowIndex extends JPanel 
{
	
  public MatRowIndex(MatDataModel data, MatPanel panel)
  {
    super();
    setOpaque(false);
    setDoubleBuffered(false);
    this.data = data;
    this.matPanel = panel;
    //setBorder(BorderFactory.createEtchedBorder());
    rowIndexFont = matPanel.table.getTableHeader().getFont();
      
    setFont( rowIndexFont);
    fm = getFontMetrics( rowIndexFont);

    addMouseListener(new MouseListener(){
			public void mousePressed(MouseEvent e){}
			public void mouseClicked(MouseEvent e){}
			public void mouseReleased(MouseEvent e){}
			public void mouseEntered(MouseEvent e){}
			public void mouseExited(MouseEvent e){}
		});
    
    matPanel.scrollPane.getVerticalScrollBar().addAdjustmentListener( new AdjustmentListener(){
      public void adjustmentValueChanged(AdjustmentEvent e)
      {
        yTransp = e.getValue();
        repaint();
      }
    });
}

public void paintComponent(Graphics g)
{
  Dimension d = getSize();
  int rows = data.getRows();
  g.setColor( MatPanel.rowsIdColor);
  g.fillRect(0, 0, d.width, d.height);
  /*g.setColor( MatPanel.matGridColor);
  g.draw3DRect( 1, 1, d.width-1, MatPanel.ROW_HEIGHT-1, true);*/
  
  int rectY = /*2**/MatPanel.ROW_HEIGHT - yTransp;
  String idxString;
  int strW = 0;
  for(int i = 0; i<rows; i++)
  {
    g.setColor( MatPanel.matGridColor);
    g.drawLine(0, rectY, d.width, rectY);
    idxString = ""+i;
    strW = fm.stringWidth(idxString);
    g.setColor(Color.black);
    g.drawString(idxString, (d.width-strW)/2, rectY-4);
    rectY += MatPanel.ROW_HEIGHT;
  }
}

protected void processMouseEvent(MouseEvent e)
{
  super.processMouseEvent(e);
}

public Dimension getPreferredSize()
{ return rulerDimension; }


Dimension rulerDimension = new Dimension(ROW_WIDTH, ROW_HEIGHT);
FontMetrics fm;
MatDataModel data;
MatPanel matPanel;
public final static int ROW_WIDTH = 30; 
public final static int ROW_HEIGHT = MatWindow.DEFAULT_HEIGHT; 
Font rowIndexFont;
int yTransp = 0;
}    




