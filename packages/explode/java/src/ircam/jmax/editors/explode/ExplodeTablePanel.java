
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.*;

/**
 * The panel containing the JTable representation of an Explode */
class ExplodeTablePanel extends JPanel {

  ExplodeTablePanel(ExplodeTableModel model)
  {
    this.tmodel = model;
    table = new JTable(tmodel);
    JScrollPane scrollpane = new JScrollPane(table);

    setUpIntegerEditor();

    setLayout(new BorderLayout());
    add(BorderLayout.CENTER, scrollpane);

    table.setSelectionModel(ExplodeSelection.getSelection());

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
	  table.removeRowSelectionInterval(0, tmodel.getExplodeDataModel().length()-1);
	  repaint(); 
	}

      public void objectDeleted(Object whichObject, int oldIndex)
	{
	  table.removeRowSelectionInterval(oldIndex, oldIndex);
	  repaint(); //test
	}
    });
  }

  private void setUpIntegerEditor() {
    //Set up the editor for the integer cells.
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

