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

    if( midiMan != null)
      midiMan.setListener( this);

    initDataModel();

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    setBorder( BorderFactory.createEtchedBorder());

    /*********** Table  ******************************************/
    initCellEditors();
    defaultLabelFont = tableFont.deriveFont( Font.BOLD);

    midiTable = new JTable( midiModel){
	public TableCellEditor getCellEditor(int row, int column)
	{
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

    scrollPane = new JScrollPane( midiTable);
    scrollPane.setPreferredSize( new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    scrollPane.addMouseListener( new MouseAdapter(){
      public void mousePressed( MouseEvent e){
        midiTable.clearSelection();
        if( midiTable.getCellEditor() != null)
          midiTable.getCellEditor().cancelCellEditing();
        midiTable.transferFocus();
      }
    });

    
    add( scrollPane);

    if( midiMan == null)
      midiTable.setEnabled( false);
  }

  void initDataModel()
  {
    midiModel = new MidiTableModel( midiMan);
    
    if( midiMan == null) return;

    FtsMidiManager.MidiLabel label;
    for( Enumeration e = midiMan.getLabels(); e.hasMoreElements();)
      {
      label = (FtsMidiManager.MidiLabel) e.nextElement();
      midiModel.addRow( label.label, label.input, label.output);    
      }
  }

  void initCellEditors()
  {
    if( midiMan == null) return;

    JComboBox sourceCombo = new JComboBox( midiMan.getSources());
    sourceCombo.setBackground( Color.white);
    sourceCombo.setFont( tableFont);
    inputCellEditor = new DefaultCellEditor( sourceCombo);

    JComboBox destCombo = new JComboBox( midiMan.getDestinations());
    destCombo.setBackground( Color.white);
    destCombo.setFont( tableFont);
    outputCellEditor = new DefaultCellEditor( destCombo);
  }

  void update()
  {    
    initDataModel();
    midiTable.setModel( midiModel);
    midiTable.revalidate();
    revalidate();    
    window.pack();
  }

  public void sourcesChanged()
  {  
    initCellEditors();
    midiTable.revalidate();
    revalidate(); 
  }

  public void destinationsChanged()
  {
    initCellEditors();
    midiTable.revalidate();
    revalidate(); 
  }

  public void labelChanged( int id, String name, String in, String out)
  {
    midiModel.setRowValues( id, name, in, out);
    midiTable.revalidate();
    revalidate();
  }
  
  void Add()
  {
    int sel = midiTable.getSelectedRow();
    midiModel.addRow( sel);
    if( sel==-1) sel = midiTable.getRowCount()-2;
    midiTable.getSelectionModel().setSelectionInterval( sel+1, sel+1);
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
      if((row == 0) && ( col == 0)) return false;
      else return true;
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
      Object[][] temp;
      
      if( size > rows)
      {
        temp = new Object[size+5][3];
        rows = size+5;
      }
      else
        temp = new Object[size][3];

      if(index == -1)
        {
          for(int i = 0; i < size-1; i++)
          {
            temp[i][0] = data[i][0];
            temp[i][1] = data[i][1];
            temp[i][2] = data[i][2];
          }
          temp[size-1][0] = "unnamed";
          temp[size-1][1] = null;
          temp[size-1][2] = null;
          index = size-2;
        }
      else
        {
          for(int i = 0; i < index+1; i++)
          {
            temp[i][0] = data[i][0];
            temp[i][1] = data[i][1];
            temp[i][2] = data[i][2];
          }

          temp[index+1][0] = "unnamed";
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

      if( midiMan != null)
        midiMan.requestInsertLabel( index+1, "unnamed");

      fireTableDataChanged();      
    }

    public void addRow(Object v1, Object v2, Object v3)
    {
      size++;
      Object[][] temp;

      if( size > rows)
        {
          temp = new Object[size+5][3];
          rows = size+5;
        }
      else
        temp = new Object[size][3];

      for(int i = 0; i < size-1; i++)
        {
          temp[i][0] = data[i][0];
          temp[i][1] = data[i][1];
          temp[i][2] = data[i][2];
        }
      
      temp[size-1][0] = v1;
      temp[size-1][1] = v2;
      temp[size-1][2] = v3;
      
      data = temp;
    }

    public void removeRow(int rowId)
    {
      if(( rowId == 0) || (( rowId == -1)&&( size == 1))) return;
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
          else rowId = size;
          
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

    public void setRowValues(int row, Object v1, Object v2, Object v3)
    {
      if( v1 != null)
      {
        data[row][0] = v1;
        fireTableCellUpdated(row, 0);
      }
      if( v2 != null)
      {
        data[row][1] = v2;
        fireTableCellUpdated(row, 1);
      }
      if( v3 != null)
      {
        data[row][2] = v3;
        fireTableCellUpdated(row, 2);
      }
    }
    
    public void setValueAt(Object value, int row, int col) 
    {
      if(row > size) return;

      Object oldValue = data[row][col];

      if((((String)oldValue) != null) && ( ((String)oldValue).equals( value)))
	return;

      data[row][col] = value;

      fireTableCellUpdated(row, col);
    
      if( midiMan != null)
	{
	  if( col==0)
	    {
              if( ((String)value).equals("")) value = "unnamed";
            
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

  private Font tableFont = (Font)UIManager.get("Table.font");
  private Font defaultLabelFont;
}




