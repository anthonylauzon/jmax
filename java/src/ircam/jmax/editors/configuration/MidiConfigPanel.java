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

package ircam.jmax.editors.configuration;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class MidiConfigPanel extends JPanel implements Editor
{
  public MidiConfigPanel(Window win, FtsMidiManager mm)  
  {
    window = win;
    midiMan = mm;

    midiMan.setFtsActionListener( new FtsActionListener(){
	public void ftsActionDone()
	{
	  update();
	  window.setVisible( true);
	}
      });

    initDataModel();

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    setBorder( BorderFactory.createEtchedBorder());

    /*********** Table  ******************************************/
    initCellEditors();

    midiTable = new JTable( midiModel){
	public TableCellEditor getCellEditor(int row, int column){
	  switch( column)
	    {
	    case 0:
	      return super.getCellEditor(row, column);
	    case 1:
	      return inputCellEditor;
	    case 2:
	      return outputCellEditor;
	    default: 
	      return super.getCellEditor(row, column);
	    }
	}
      }; 

    midiTable.setPreferredScrollableViewportSize( new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    midiTable.setRowHeight(17);
    midiTable.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);
    midiTable.setPreferredSize( new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));

    scrollPane = new JScrollPane( midiTable);
    scrollPane.setPreferredSize( new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    add( scrollPane);
  }

  void initDataModel()
  {
    midiModel = new MidiTableModel( midiMan);
    
    FtsMidiManager.MidiLabel label;
    for( Enumeration e = midiMan.getLabels(); e.hasMoreElements();)
      {
	label = (FtsMidiManager.MidiLabel) e.nextElement();
	midiModel.addRow( label.label, label.input, label.output);    
      }
  }

  void initCellEditors()
  {
    JComboBox sourceCombo = new JComboBox( midiMan.getSources());
    sourceCombo.setBackground( Color.white);
    sourceCombo.setFont( (Font)UIManager.get("Table.font"));
    inputCellEditor = new DefaultCellEditor( sourceCombo);

    JComboBox destCombo = new JComboBox( midiMan.getDestinations());
    destCombo.setBackground( Color.white);
    destCombo.setFont( (Font)UIManager.get("Table.font"));
    outputCellEditor = new DefaultCellEditor( destCombo);
  }

  void update()
  {    
    //?????????????????????
    initDataModel();
    midiTable.setModel( midiModel);
    midiTable.revalidate();
    revalidate();    
    window.pack();
  }

  void Add()
  {
    midiModel.addRow( midiTable.getSelectedRow());
  }

  void Delete()
  {
    midiModel.removeRow( midiTable.getSelectedRow());
  }

  /*********************************************************
   ***   Table model for the Requires JTable             ***
   *********************************************************/
  class MidiTableModel extends AbstractTableModel 
  {
    MidiTableModel( FtsMidiManager mm)
    {
      super();
      midiMan = mm;
    }

    public int getColumnCount() 
    { 
      return 3;
    }
  
    public Class getColumnClass(int col)
    {
      return String.class;
    }

    public boolean isCellEditable(int row, int col)
    {
      return true;
    }

    public String getColumnName(int col)
    {
      switch(col)
	{
	case 0: 
	  return "label";
	case 1: 
	  return "input";
	case 2: 
	  return "output";
	default:
	  return "";
	}
    }

    public int getRowCount() { 
      return size; 
    }

    public void addRow(int index)
    {
      size++;    
      if((size > rows) || (index != -1))
	{
	  Object[][] temp = new Object[size+5][3];
	  if(index == -1)
	    for(int i = 0; i < size-1; i++)
	      {
		temp[i][0] = data[i][0];
		temp[i][1] = data[i][1];
		temp[i][2] = data[i][2];
	      }
	  else
	    {
	      for(int i = 0; i < index+1; i++)
	      {
		temp[i][0] = data[i][0];
		temp[i][1] = data[i][1];
		temp[i][2] = data[i][2];
	      }

	      temp[index+1][0] = null;
	      temp[index+1][1] = null;
	      temp[index+1][2] = null;

	      for(int j = index+2; j < size; j++)
	      {
		temp[j][0] = data[j-1][0];
		temp[j][1] = data[j-1][1];
		temp[j][2] = data[j-1][2];
	      }
	    }
	  data = temp;
	  rows = size+5;
	}
      fireTableDataChanged();
    }

    public void addRow(Object v1, Object v2, Object v3)
    {
      addRow(-1);
      data[size-1][0] = v1;
      data[size-1][1] = v2;
      data[size-1][2] = v3;
    }

    public void removeRow(int rowId)
    {
      if(size > 0)
	{
	  size--;    
	  if(rowId >= 0)
	    {
	      for(int i = rowId; i < size; i++)
		{
		  data[i][0] = data[i+1][0];
		  data[i][1] = data[i+1][1];
		  data[i][2] = data[i+1][2];
		}
	    }
	  data[size][0] = null;
	  data[size][1] = null;
	  data[size][2] = null;

	  fireTableDataChanged();
	  midiMan.requestRemoveLabel( rowId);
	}
    }

    public Object getValueAt(int row, int col) 
    { 
      if(row > size) return null;
      else
	return data[row][col];
    }

    public void setValueAt(Object value, int row, int col) 
    {
      if(row > size) return;

      Object oldValue = data[row][col];
      data[row][col] = value;

      fireTableCellUpdated(row, col);
    
      if( midiMan != null)
	{
	  if( col==0)
	    {
	      if( oldValue == null)
		midiMan.requestInsertLabel( row, (String)value);	      
	      else
		{
		  midiMan.requestRemoveLabel( row);	
		  midiMan.requestInsertLabel( row, (String)value);	
		  if(data[row][1] != null)
		    midiMan.requestSetInput( row, (String) data[row][1]);	
		  if(data[row][2] != null)
		    midiMan.requestSetOutput( row, (String)data[row][2]);	
		}
	    }	  
	  else
	    if(col == 1)
	      midiMan.requestSetInput( row, (String)value);
	    else if(col == 2)
	      midiMan.requestSetOutput( row, (String)value);
	}
    }

    int size = 0;
    int rows = 0;
    Object data[][];
    FtsMidiManager midiMan;
  }

  /************* interface Editor ************************/
  public EditorContainer getEditorContainer()
  {
    return (EditorContainer)window;
  }

  public void close(boolean doCancel)
  {
    window.setVisible(false);
  }

  public void save(){}
  public void saveAs(){}
  public void print(){}
 
  /********************************/
  private JTabbedPane tabbedPane;
  private JTable midiTable;
  private DefaultCellEditor inputCellEditor;
  private DefaultCellEditor outputCellEditor;

  private JScrollPane scrollPane;
  private MidiTableModel midiModel;
  private Window window;
  private FtsMidiManager midiMan;
  private final int DEFAULT_WIDTH = 450;
  private final int DEFAULT_HEIGHT = 280;
}




