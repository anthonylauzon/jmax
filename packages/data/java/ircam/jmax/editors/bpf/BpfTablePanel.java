//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.bpf;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.*;

/**
 * The panel containing the JTable representation of an Explode.
 * The editing of a generic entry is handled by a DefaultCellEditor object.
 * See the setUpIntegerEditor method in this class for details. */
class BpfTablePanel extends JPanel implements ListSelectionListener {

  BpfTablePanel(BpfTableModel model, BpfGraphicContext gc)
  {
    this.tmodel = model;
    this.gc = gc;
    table = new JTable(tmodel);
    table.setPreferredScrollableViewportSize(new Dimension(BpfWindow.DEFAULT_WIDTH, BpfWindow.DEFAULT_HEIGHT-55));
    table.setRowHeight(17);
    table.getColumnModel().getColumn(0).setPreferredWidth(50);
    table.getColumnModel().getColumn(0).setMaxWidth(50);

    scrollPane = new JScrollPane(table);
    scrollPane.setPreferredSize(new Dimension(BpfWindow.DEFAULT_WIDTH, BpfWindow.DEFAULT_HEIGHT/*-25*/));

    setUpEditors();
    
    setLayout(new BorderLayout());
    add(BorderLayout.CENTER, scrollPane);

    table.setSelectionModel(gc.getSelection());
    gc.getSelection().addListSelectionListener(this);

    BpfDataModel bpfModel = tmodel.getDataModel();

    // make this panel a listener of the Sequence data base: changing
    // of the content will result in the right repaint()
    bpfModel.addBpfListener(new BpfDataListener() {

	    public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue)
	    {
		repaint();
	    }
	    public void pointsChanged()
	    {
		repaint();
	    }
	    public void pointAdded(int index)
	    {
		table.revalidate();
	    }
	    public void pointsDeleted(int[] indexs)
	    {
		table.revalidate();
	    }
	    public void cleared() 
	    {
		table.revalidate();
	    }
    });
  }

    /*
      listSelectionListener interface
    */
  public void valueChanged(ListSelectionEvent e)
  {
      Rectangle rect;
      ListSelectionModel selection = table.getSelectionModel();

      if(selection.isSelectionEmpty() || selection.getValueIsAdjusting()) return;

      int minIndex = selection.getMinSelectionIndex();
      int maxIndex = selection.getMaxSelectionIndex();
      
      rect = table.getCellRect(minIndex, 0, true);

      if(minIndex!=maxIndex)
	  rect = rect.union(table.getCellRect(maxIndex, 0, true));

      table.scrollRectToVisible(rect);      
    }

    /*
     * Setup typed editors in table: in jdk1.3 setup only multilines textArea for string values
     * The rest is a bug fix for JTable in jdk 117.
     */
    private void setUpEditors() {
	// Set up the editor for the numeric cells.
	final JTextField numberField = new JTextField();
	numberField.setHorizontalAlignment(JTextField.RIGHT);

	DefaultCellEditor floatEditor = new DefaultCellEditor(numberField) {
		public Object getCellEditorValue() {
		    try { 
			return Float.valueOf(numberField.getText()); // parse double
		    } catch (NumberFormatException exc) {
			Toolkit.getDefaultToolkit().beep();
			System.err.println("Error:  invalid number format!");
			return null;
		    }
		}
	    };

	table.setDefaultEditor(Float.class, floatEditor);
    }
  
  //--- Fields
    public class CellEditorArea extends DefaultCellEditor
    {
	JTextArea area = new JTextArea();
	JScrollPane scroll;
	public CellEditorArea(JTextField field)
	{
	    super(field);
	    scroll = new JScrollPane(area,JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	}
	public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column)
	{
	    area.setText(""+table.getValueAt(row, column));
	    return scroll;
	}
	public Object getCellEditorValue() {
	    return area.getText();
	}
    }

  BpfTableModel tmodel;
  BpfGraphicContext gc;
  JScrollPane scrollPane; 
  JTable table;
}



