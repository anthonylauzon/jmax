package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import com.sun.java.swing.*;
import com.sun.java.swing.table.*;
import com.sun.java.swing.event.*;

class ExplodeTableModel extends AbstractTableModel {
  public ExplodeTableModel( ExplodeRemoteData explodeRemoteData)
  {
    this.explodeRemoteData = explodeRemoteData;
  }

  public int getColumnCount() 
  {
    return names.length; 
  }

  public int getRowCount() 
  {
    return explodeRemoteData.length();
  }

  public Object getValueAt(int row, int col) 
  {
    return explodeRemoteData.getValue( row, col); 
  }

  public String getColumnName(int column)
  {
    return names[column];
  }

  public boolean isCellEditable( int rowIndex, int columnIndex)
  {
    //return columnIndex >= 1;
    return false;
  }

  private ExplodeRemoteData explodeRemoteData;

  static String names[] = { "Time", "Pitch", "Velocity", "Duration", "Channel"};
}


public class Explode extends MaxEditor implements MaxDataEditor {

  public Explode( MaxData maxData)
  {
    super( Mda.getDocumentTypeByName( "explodeRemoteData"));

    this.maxData = maxData;

    // A method of the super class, that must be called, and is not documented... 
    Init();

    setTitle( "Explode");

    ExplodeRemoteData explodeRemoteData = (ExplodeRemoteData) ((FtsRemoteDataObject)maxData).getRemoteData();

    JTable table = new JTable( new ExplodeTableModel( explodeRemoteData ));

    getContentPane().add( JTable.createScrollPaneForTable( table));

    table.getTableHeader().setReorderingAllowed( false);

    setSize( 300, 300);

    validate();

    setVisible( true);
  }


  // MaxDataEditor interface
  public MaxData getData()
  {
    return maxData;
  }

  // MaxDataEditor interface
  public void reEdit()
  {
    if (!isVisible()) 
      setVisible(true);

    toFront();
  }

  // MaxDataEditor interface
  public void quitEdit()
  {
    Close();
    dispose();
  }

  // MaxDataEditor interface
  public void syncData()
  {
  }

  // MaxDataEditor interface
  public void dataChanged( Object reason)
  {
  }

  // MaxEditor
  public void SetupMenu()
  {
  }

  MaxData maxData;
}  
