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
public class MatPanel extends JPanel implements Editor, MatDataListener
{    
  MatDataModel matData;
  EditorContainer itsContainer;
  MatTableModel tableModel;
  MatRowIndex rowIndex;
  JPanel topCorner, westPanel;
  
  transient JScrollPane scrollPane; 
  transient JTable table;
  FtsObjectCellEditor ftsObjEditor;
  FtsObjectCellRenderer ftsObjRenderer;
  
  public static final Color matGridColor = new Color(220, 220, 220);
  public static final Color rowsIdColor = new Color(245, 245, 245);
  public static final int COLUMN_MIN_WIDTH = 70;
  public static final int ROW_HEIGHT = 17;
  
  public MatPanel(EditorContainer container, MatDataModel data) 
  {  
    itsContainer = container;

    matData = data;
    matData.addMatListener(this);

    setBorder(BorderFactory.createEtchedBorder());
    
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
    
    ftsObjEditor = new FtsObjectCellEditor();
    ftsObjRenderer = new FtsObjectCellRenderer();
    
    table = new MatTable(tableModel);
		table.setGridColor( matGridColor);
		table.setShowGrid(true);			
    table.setPreferredScrollableViewportSize(new Dimension( MatWindow.DEFAULT_WIDTH, MatWindow.DEFAULT_HEIGHT));
    table.setRowHeight(ROW_HEIGHT);
    table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF);
    table.getTableHeader().setReorderingAllowed(false);
        
    scrollPane = new JScrollPane(table);
    add(BorderLayout.CENTER, scrollPane);
    
    table.addKeyListener(new KeyAdapter(){
      public void keyPressed(KeyEvent e)
      {
        if(e.getKeyCode() == KeyEvent.VK_ENTER && !table.isEditing())
          matData.requestAppendRow();
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
  }
  void updateTableModel()
  {
    tableModel = new MatTableModel(matData);
    table.setModel(tableModel);
    
    for (int i = 1; i < matData.getColumns(); i++) 
      table.getColumnModel().getColumn(i).setMinWidth(COLUMN_MIN_WIDTH);
  }
  
  void updateTableToSize(int width)
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
  }
  //------------------- MaxDataListener interface ---------------
  public void matCleared()
  {
    table.revalidate();
    repaint();
  }
  public void matDataChanged()
  {
    if(! uploading)
    {
      table.revalidate();
      repaint();
    }
  }
  public void matSizeChanged(int n_rows, int n_cols)
  {
    updateTableModel();
    updateTableToSize( itsContainer.getFrame().getSize().width);
    table.revalidate();
    scrollPane.validate();
    repaint();
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
  //------------------- Editor interface ---------------

  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
  public void close(boolean doCancel){
    itsContainer.getFrame().setVisible(false);
    ((FtsObjectWithEditor)matData).requestDestroyEditor(); 
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}

  /************************     FtsObject Table CellEditor ***********************************/
  public class FtsObjectCellEditor extends AbstractCellEditor implements TableCellEditor, ActionListener 
  {
    JButton button;
    protected static final String EDIT = "edit";
    FtsGraphicObject currentObject = null;  
    
    public FtsObjectCellEditor() 
    {
      button = new JButton();
      button.setActionCommand(EDIT);
      button.addActionListener(this);
    }
      
    public void actionPerformed(ActionEvent e) 
    {
      if (EDIT.equals(e.getActionCommand())) 
      {          
        ((FtsObjectWithEditor)currentObject).requestOpenEditor();
        fireEditingStopped();
      } 
    }
  
    public Object getCellEditorValue() {
      return currentObject;
    }
      
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) 
    {
      currentObject = (FtsGraphicObject)value;
      return button;
    }
  }
  
  public class FtsObjectCellRenderer extends JLabel implements TableCellRenderer 
  {  
    public FtsObjectCellRenderer() 
    {
      setOpaque(true); 
      setBorder(BorderFactory.createEtchedBorder());
      setBackground(Color.lightGray);
      setHorizontalTextPosition(SwingConstants.CENTER);
    }
    
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus,int row, int column) 
    {      
      String name = ((FtsGraphicObject)value).getVariableName();
      
      if( name != null && !name.equals(""))
        setText(((FtsGraphicObject)value).getClassName()+" "+name);
      else
        setText(((FtsGraphicObject)value).getClassName()+" #"+((FtsGraphicObject)value).getObjectID());
      
      setHorizontalTextPosition(SwingConstants.CENTER);
      return this;
    }
  }  
  
  public class MatTable extends JTable
  {
    public MatTable(TableModel model)
    {
      super(model);
    }
    public TableCellEditor getCellEditor(int row,int col)
    {
      if( getModel().getValueAt(row, col) instanceof FtsGraphicObject)
        return ftsObjEditor;
      else
        return super.getCellEditor(row, col);
    }
    public TableCellRenderer getCellRenderer(int row,int col)
    {
      if( getModel().getValueAt(row, col) instanceof FtsGraphicObject)
        return ftsObjRenderer;
      else
        return super.getCellRenderer(row, col);
    }
  }
}














