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

package ircam.jmax.toolkit;

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
import ircam.fts.client.*;

  /**
   * The graphic component containing the tracks of a Sequence.
   */
public class JMaxMatTable extends JTable 
{    
  FtsObjectCellEditor ftsObjEditor;
  FtsObjectCellRenderer ftsObjRenderer;
  
  public static final Color matGridColor = new Color(220, 220, 220);
  public static final Color rowsIdColor = new Color(245, 245, 245);
  public static final int COLUMN_MIN_WIDTH = 60;
  
  boolean shiftPressed = false;
  boolean ctrlPressed = false;
  
  public JMaxMatTable(TableModel model)
  {
    super(model);
    
    ftsObjEditor = new FtsObjectCellEditor();
    ftsObjRenderer = new FtsObjectCellRenderer();
    
    setGridColor( matGridColor);
		setShowGrid(true);
    setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
  }
  
  public TableCellEditor getCellEditor(int row,int col)
  {
    if( getModel().getValueAt(row, col) instanceof FtsObject)
      return ftsObjEditor;
    else
      return super.getCellEditor(row, col);
  }
  
  public TableCellRenderer getCellRenderer(int row,int col)
  {
    if( getModel().getValueAt(row, col) instanceof FtsObject)
      return ftsObjRenderer;
    else
      return super.getCellRenderer(row, col);
  }
  
  protected void processKeyEvent(KeyEvent e)
  {
    if(isEditing() || !(e.getKeyText(e.getKeyCode()).equals("Command") && e.getKeyChar() == KeyEvent.CHAR_UNDEFINED))
      super.processKeyEvent(e);
  }
  
  /************************     FtsObject Table CellEditor ***********************************/
  public class FtsObjectCellEditor extends AbstractCellEditor implements TableCellEditor, ActionListener 
  {
    JButton button;
    protected static final String EDIT = "edit";
    FtsObject currentObject = null;
    
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
        if(currentObject instanceof FtsObjectWithEditor)
          ((FtsObjectWithEditor)currentObject).requestOpenEditor();
  
        fireEditingStopped();
      }
    }
  
    public Object getCellEditorValue() {
      return currentObject;
    }
      
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) 
    {
      currentObject = (FtsObject)value;
      return button;
    }
  }
  
  public class FtsObjectCellRenderer extends JLabel implements TableCellRenderer 
  {  
    public FtsObjectCellRenderer() 
    {
      setOpaque( true); 
      setBorder( BorderFactory.createEtchedBorder());
      setBackground( JMaxMatTable.rowsIdColor);
      setHorizontalTextPosition( SwingConstants.CENTER);
    }
    
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus,int row, int column) 
    {      
      if(value instanceof FtsGraphicObject)
      {
        String name = ((FtsGraphicObject)value).getVariableName();
        String description = ((FtsGraphicObject)value).getDescription();
      
        if(name != null && !name.equals(""))
          setText(description + " [" + name + "]");
        else
          setText(description + " [#" + ((FtsGraphicObject)value).getObjectID() + "]");
      }
      else 
      {
        String description = ((FtsObject)value).getDescription();
        
        if(description.charAt(0) != '{')
          setText(description + " [#" + ((FtsObject)value).getID() + "]");
        else
          setText(description);
      }
      
      setHorizontalTextPosition( SwingConstants.CENTER);
      return this;
    }
  }    
}















