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

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.undo.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

  /**
   * The graphic component containing the tracks of a Sequence.
   */
public class MatPanel extends JPanel implements Editor, MatDataListener
{    
  FtsMatObject matData;
  EditorContainer itsContainer;
  MatTableModel tableModel;

  transient JScrollPane scrollPane; 
  transient JTable table;
  public static final Color matGridColor = new Color(220, 220, 220);
  public static final int COLUMN_MIN_WIDTH = 70;
  
  public MatPanel(EditorContainer container, FtsMatObject data) 
  {  
    itsContainer = container;

    matData = data;
    matData.addMatListener(this);

    tableModel = new MatTableModel(matData);
    
    createTable();
    
    validate();
    
    itsContainer.getFrame().addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
      {
				updateTableToSize( e.getComponent().getSize().width);
      }
			public void componentMoved(ComponentEvent e){}
		});
  }
  
  void createTable()
  {
    setLayout(new BorderLayout());
    
    table = new JTable(tableModel);
		table.setGridColor( matGridColor);
		table.setShowGrid(true);			
    table.setPreferredScrollableViewportSize(new Dimension( MatWindow.DEFAULT_WIDTH, MatWindow.DEFAULT_HEIGHT));
    table.setRowHeight(17);
    table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF);
    scrollPane = new JScrollPane(table);
    add(BorderLayout.CENTER, scrollPane);
  }
  
  void updateTableModel()
  {
    tableModel = new MatTableModel(matData);
    table.setModel(tableModel);
    
    for (int i = 0; i < matData.getColumns(); i++) 
      table.getColumnModel().getColumn(i).setMinWidth(COLUMN_MIN_WIDTH);
  }
  
  void updateTableToSize(int width)
  {
    if( matData.getColumns() * COLUMN_MIN_WIDTH < width)
      table.setAutoResizeMode( JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
    else
      table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF);
  }
   //------------------- MaxDataListener interface ---------------
  public void matCleared()
  {
    table.revalidate();
    repaint();
  }
  public void matDataChanged()
  {
    table.revalidate();
    repaint();
  }
  public void matSizeChanged(int n_rows, int n_cols)
  {
    updateTableModel();
    updateTableToSize( itsContainer.getFrame().getSize().width);
    table.revalidate();
    scrollPane.validate();
    repaint();
  }
 
  //------------------- Editor interface ---------------

  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
  public void close(boolean doCancel){
    itsContainer.getFrame().setVisible(false);
    matData.requestDestroyEditor(); 
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}
}















