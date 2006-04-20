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
import javax.swing.table.*;
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
public class MatPanel extends JPanel implements Editor, MatDataListener, JMaxTableListener
{    
  MatDataModel matData;
  FtsObjectWithEditor ftsObj;
  EditorContainer itsContainer;
  MatTableModel tableModel;
  MatRowIndex rowIndex;
  JPanel topCorner, westPanel;
  int verticalScrollValue = 0;
  
  transient JScrollPane scrollPane; 
  transient JTable table;
  
  public static final int COLUMN_MIN_WIDTH = 60;
  public static final int ROW_HEIGHT = 17;
  
  public MatPanel(EditorContainer container, FtsObjectWithEditor ftsObj, MatDataModel data) 
  {  
    itsContainer = container;

    this.ftsObj = ftsObj;
    matData = data;
    matData.addMatListener(this);

    setBorder(BorderFactory.createEtchedBorder());
    
    tableModel = new MatTableModel(matData);
    
    createTable();
    
    validate();
    
    itsContainer.getFrame().addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
      {
        Dimension d = e.getComponent().getSize();
				updateTableToSize( d.width, d.height);
      }
			public void componentMoved(ComponentEvent e){}
		});
  }
  
  public MatPanel(EditorContainer container, MatDataModel data) 
  {  
    itsContainer = container;
    
    matData = data;
    this.ftsObj = (FtsObjectWithEditor)matData;
    matData.addMatListener(this);
    
    setBorder(BorderFactory.createEtchedBorder());
    
    tableModel = new MatTableModel(matData);
    
    createTable();
    
    validate();
    
    itsContainer.getFrame().addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
      {
        Dimension d = e.getComponent().getSize();
				updateTableToSize( d.width, d.height);
      }
			public void componentMoved(ComponentEvent e){}
		});
  }
  
  public void setContainer(EditorContainer container)
  {
    itsContainer = container;
    itsContainer.getFrame().addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
      {
        Dimension d = e.getComponent().getSize();
				updateTableToSize( d.width, d.height);
      }
			public void componentMoved(ComponentEvent e){}
		});
  }
  
  void createTable()
  {
    setLayout(new BorderLayout());
    table = new JMaxMatTable(tableModel, this);	
    table.setPreferredScrollableViewportSize(matData.getDefaultSize());
    table.setRowHeight(ROW_HEIGHT);
    table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF);
    table.getTableHeader().setReorderingAllowed(false);
    table.setColumnSelectionAllowed(true);  
    
    scrollPane = new JScrollPane(table);
    add(BorderLayout.CENTER, scrollPane);
    
    table.addKeyListener(new KeyAdapter(){
      public void keyPressed(KeyEvent e)
      {
        if(e.getKeyCode() == KeyEvent.VK_ENTER && !table.isEditing())
        {
          if( table.getSelectionModel().isSelectionEmpty())
            matData.requestAppendRow();
          else
            matData.requestInsertRow(table.getSelectionModel().getMinSelectionIndex()+1);
        }
      }
    });
    
    if( matData.haveRowIdCol())
    {
      rowIndex = new MatRowIndex(matData, this);
      rowIndex.setBorder(BorderFactory.createEtchedBorder());
      
      topCorner = new JPanel();
      topCorner.setPreferredSize(new Dimension(38, MatPanel.ROW_HEIGHT));
      topCorner.setMaximumSize(new Dimension(38, MatPanel.ROW_HEIGHT));
      topCorner.setBorder(BorderFactory.createEtchedBorder());
      
      topCorner.addMouseListener(new MouseAdapter(){
        public void mousePressed(MouseEvent e)
        {
          table.selectAll();
        }
      });
      
      westPanel = new JPanel();
      westPanel.setLayout(new BoxLayout(westPanel, BoxLayout.Y_AXIS));
      westPanel.add(topCorner);
      westPanel.add(rowIndex);
      
      add(BorderLayout.WEST, westPanel);
    } 
    
    scrollPane.getVerticalScrollBar().addAdjustmentListener( new AdjustmentListener() {
      public void adjustmentValueChanged( AdjustmentEvent e)
		  {
        if(verticalScrollValue != e.getValue())
        {
          verticalScrollValue = e.getValue();
          updateLastVisibleRow();  
        }
      }
		});
  }
  void updateTableModel()
  {
    tableModel = new MatTableModel(matData);
    table.setModel(tableModel);
    
    for (int i = 1; i < matData.getColumns(); i++) 
      table.getColumnModel().getColumn(i).setMinWidth(COLUMN_MIN_WIDTH);
  }
  
  void updateTableToSize(int width, int height)
  {
    if( matData.getColumns() * COLUMN_MIN_WIDTH < width)
      table.setAutoResizeMode( JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
    else
      table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF);
    
    if( matData.haveRowIdCol())
    {
      rowIndex.updateSize();
      if(matData.getRows() < 100000)
      {
        topCorner.setPreferredSize(new Dimension( MatRowIndex.ROW_WIDTH, MatPanel.ROW_HEIGHT));
        topCorner.setMaximumSize(new Dimension( MatRowIndex.ROW_WIDTH, MatPanel.ROW_HEIGHT));
        westPanel.validate();
        validate();
      }
      else
      {
        topCorner.setPreferredSize(new Dimension(MatRowIndex.ROW_WIDTH+10, MatPanel.ROW_HEIGHT));
        topCorner.setMaximumSize(new Dimension(MatRowIndex.ROW_WIDTH+10, MatPanel.ROW_HEIGHT));
        westPanel.validate();
        validate();
      }
    }
    
    updateLastVisibleRow();
  }
  
  int getVisibleRowsCount()
  {
    return getSize().height/ROW_HEIGHT;
  }
  
  void updateLastVisibleRow()
  {
    if(matData instanceof FtsMatEditorObject)
    {
      int lastRow = (int)((getSize().height + verticalScrollValue)/ROW_HEIGHT) -2 + getVisibleRowsCount();
      if(lastRow < 0) lastRow = 0;
      else if(lastRow > matData.getRows()) lastRow = matData.getRows();
      
      ((FtsMatEditorObject)matData).requestSetLastVisibleRow(lastRow); 
    }
  }
  public MatDataModel getMatModel()
  {
    return matData;
  }
  
  public JTable getTable()
  {
    return table;
  }
  
  public void addTableSelectionListener(ListSelectionListener l)
  {
    table.getSelectionModel().addListSelectionListener(l);
  }
  public void removeTableSelectionListener(ListSelectionListener l)
  {
    table.getSelectionModel().removeListSelectionListener(l);
  }
  //------------------- MaxDataListener interface ---------------
  public void matCleared()
  {
    table.revalidate();
    repaint();
  }
  public void matDataChanged()
  {
    //if(! uploading)
    //{
      /*table.revalidate();*/
      repaint();
    //}
  }
  public void matSizeChanged(int n_rows, int n_cols)
  {
    updateTableModel();
    /*updateTableToSize( itsContainer.getFrame().getSize().width);
    table.revalidate();
    scrollPane.validate();
    repaint();*/
  }
  
  boolean uploading = false;
  public void uploading(boolean uploading)
  {
    this.uploading = uploading;
    if(!uploading)
    {
      table.revalidate();
      repaint();
    }
  }
  public void matNameChanged(String name){}
  
  public void matSelectRow( int row_id)
  {
    table.getSelectionModel().setSelectionInterval(  row_id, row_id);
  }
  
  //------------------- JMaxTableListener interface ---------------
  
  public void deleteSelection()
  {
    //
  }
  
  //------------------- Editor interface ---------------

  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
  public void close(boolean doCancel){    
    ftsObj.closeEditor();
    ftsObj.requestDestroyEditor(); 
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}
}















