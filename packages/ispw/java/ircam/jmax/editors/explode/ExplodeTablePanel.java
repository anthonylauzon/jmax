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

package ircam.jmax.editors.explode;

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
class ExplodeTablePanel extends JPanel {

  ExplodeTablePanel(ExplodeTableModel model, ExplodeSelection s)
  {
    this.tmodel = model;
    table = new JTable(tmodel);
    JScrollPane scrollpane = new JScrollPane(table);

    ExplodeSelection.setCurrent(s);

    setUpIntegerEditor();

    setLayout(new BorderLayout());
    add(BorderLayout.CENTER, scrollpane);

    table.setSelectionModel(s);

    ExplodeDataModel explodeModel = tmodel.getExplodeDataModel();

    // make this panel a listener of the Explode data base: changing
    // of the content will result in the right repaint()
    explodeModel.addListener( new ExplodeDataListener() {

      public void objectChanged(Object whichObject)
	{
	  repaint();
	}
      public void objectMoved(Object whichObject, int oldIndex, int newIndex)
	{
	  objectChanged(whichObject);
	}
      public void objectAdded(Object whichObject, int index)
	{
	  repaint(); 
	}

      public void objectDeleted(Object whichObject, int oldIndex)
	{
	  repaint(); //test
	}
    });
  }

  private void setUpIntegerEditor() {
    // Set up the editor for the integer cells.
    final WholeNumberField integerField = new WholeNumberField(0, 5);
    integerField.setHorizontalAlignment(WholeNumberField.RIGHT);
    
    DefaultCellEditor integerEditor = 
      new DefaultCellEditor(integerField) {
      //Override DefaultCellEditor's getCellEditorValue method
      //to return an Integer, not a String:
      public Object getCellEditorValue() {
	return new Integer(integerField.getValue());
      }
    };
    table.setDefaultEditor(Integer.class, integerEditor);
  }
  

  //--- Fields

  ExplodeTableModel tmodel;
  JTable table;
}

