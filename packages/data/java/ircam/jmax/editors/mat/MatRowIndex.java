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

public class MatRowIndex extends PopupToolbarPanel 
{
	
  public MatRowIndex(MatDataModel data, MatPanel panel)
  {
    super();
    setOpaque(false);
    setDoubleBuffered(false);
    this.data = data;
    this.matPanel = panel;
    rowIndexFont = matPanel.table.getTableHeader().getFont();
      
    setFont( rowIndexFont);
    fm = getFontMetrics( rowIndexFont);

    setPreferredSize(fiveNumDimension);
    
    addMouseListener(new MouseAdapter(){
			public void mousePressed(MouseEvent e)
      {
        int rowId = getRowIndex(e.getY());         
        if((e.getModifiers() & InputEvent.SHIFT_MASK) == 0)
        {
          matPanel.table.getSelectionModel().clearSelection();
          matPanel.table.addRowSelectionInterval(rowId, rowId);
        }
        else
        {
          if(matPanel.table.getSelectionModel().isSelectedIndex(rowId))
            matPanel.table.removeRowSelectionInterval(rowId, rowId);
          else
            matPanel.table.addRowSelectionInterval(rowId, rowId);
        }        
      }
		});
    
    matPanel.scrollPane.getVerticalScrollBar().addAdjustmentListener( new AdjustmentListener(){
      public void adjustmentValueChanged(AdjustmentEvent e)
      {
        yTransp = e.getValue();
        repaint();
      }
    });
    popup = new RowIndexPopupMenu(this);
}

void updateSize()
{
  if(data.getRows() < 100000)
  {
    setPreferredSize(fiveNumDimension);
    setSize(fiveNumDimension.width, fiveNumDimension.height);
    validate();
  }
  else
  {
    setPreferredSize(sixNumDimension);
    setSize(sixNumDimension.width, sixNumDimension.height);
    validate(); 
  }
}

int getRowIndex(int y)
{
  int id = (y+yTransp)/MatPanel.ROW_HEIGHT;
  int rows = data.getRows();
  if( id > rows-1) id =  rows-1;
  return id;
}

public JPopupMenu getMenu()
{
	popup.update();
	return popup;
}

public void paintComponent(Graphics g)
{
  Dimension d = getSize();
  int rows = data.getRows();
  g.setColor( MatPanel.rowsIdColor);
  g.fillRect(0, 0, d.width, d.height);
  
  String idxString;
  int strW = 0;
  int first = yTransp/MatPanel.ROW_HEIGHT;
  if(first<0) first = 0;
  int last = first + d.height/MatPanel.ROW_HEIGHT + 1;
  if(last > rows) last = rows;  
  
  int rectY = (first+1)*MatPanel.ROW_HEIGHT - yTransp;
  for(int i = first; i < last; i++)
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

Dimension fiveNumDimension = new Dimension(ROW_WIDTH, ROW_HEIGHT);
Dimension sixNumDimension = new Dimension(ROW_WIDTH+10, ROW_HEIGHT);
FontMetrics fm;
MatDataModel data;
MatPanel matPanel;
RowIndexPopupMenu popup;
public final static int ROW_WIDTH = 38; 
public final static int ROW_HEIGHT = MatWindow.DEFAULT_HEIGHT; 
Font rowIndexFont;
int yTransp = 0;
}    




