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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.toolkit.*;

/**
* The panel containing the JTable representation of an Explode.
 * The editing of a generic entry is handled by a DefaultCellEditor object.
 * See the setUpIntegerEditor method in this class for details. */
class SequenceTablePanel extends JPanel implements ListSelectionListener, JMaxTableListener {
	
  SequenceTablePanel(TrackTableModel model, SequenceGraphicContext gc, SequenceSelection selection)
  {
    this.tmodel = model;
    this.gc = gc;
    this.trackObj = (FtsTrackObject)tmodel.getTrackDataModel();
    this.selection = selection;

    table = new JMaxMatTable(tmodel, this);
		
    /************/
    if(trackObj.getType() == AmbitusValue.info || trackObj.getType() == MarkerValue.info)
		{
			JComboBox combo = new JComboBox( trackObj.getEventTypes());
			combo.setBackground(Color.white);
			typeEditor = new ComboCellEditor( combo);		
			table.getColumnModel().getColumn(1).setCellEditor(typeEditor);
			combo.setFont(table.getFont());
		}
		/************/
    int width = COL_WIDTH * tmodel.getColumnCount();
    if(width > DEF_TABLE_WIDTH) width = DEF_TABLE_WIDTH;
    table.setPreferredScrollableViewportSize(new Dimension(width, 200));
    table.setRowHeight(17);
		
    scrollPane = new JScrollPane(table);
		
    setLayout(new BorderLayout());
    add(BorderLayout.CENTER, scrollPane);
		
    table.setSelectionModel( selection);
    selection.addListSelectionListener(this);
		
    trackObj.addHighlightListener(new HighlightListener() {
			public void highlight(Enumeration hhElements, double time)
		  {
				TrackEvent evt;
				int index;	      
				Rectangle rect = null;
				for (Enumeration e = hhElements; e.hasMoreElements();) 
				{
					evt = (TrackEvent) e.nextElement();			  
					index = trackObj.indexOf(evt);
					
					if(rect!=null)
						rect = rect.union(table.getCellRect(index, 0, true));
					else
						rect = table.getCellRect(index, 0, true);
				}
				if(rect != null)
					table.scrollRectToVisible(rect);
		}
		});
    
    // make this panel a listener of the Sequence data base: changing
    // of the content will result in the right repaint()
    trackObj.addListener( new TrackDataListener() {
			public void objectChanged(Object spec, int index, String propName, Object propValue){
				repaint();
		  }
			public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){
				repaint();
			}
			public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}
			public void objectAdded(Object whichObject, int index)
		  {
				if( !uploading)
          table.revalidate();
      }
			public void objectsAdded(int maxTime)
		  {
        table.revalidate();
      }
			public void objectDeleted(Object whichObject, int oldIndex)
		  {
				table.revalidate();
			}
			public void trackCleared()
		  {
        SwingUtilities.invokeLater(new Runnable() {
          public void run()
          { 
            table.revalidate();
          }
        });
      }
			boolean uploading = false;
			public void startTrackUpload( TrackDataModel track, int size)
			{
				uploading  = true;
			}
			public void endTrackUpload( TrackDataModel track)
			{
				uploading = false;
				table.revalidate();
			}
			public void startPaste(){}
			public void endPaste(){}
      public void startUndoRedo(){}
      public void endUndoRedo(){}
    });
		
		table.getColumnModel().addColumnModelListener(new TableColumnModelListener(){
			public void columnAdded(TableColumnModelEvent e){};
			public void columnRemoved(TableColumnModelEvent e){};
			public void columnMoved(TableColumnModelEvent e)
			{
				if(!restoring && e.getToIndex() != e.getFromIndex())
				{
					Vector names = getTableColumnNames();
					trackObj.editorObject.setTableColumnOrder(names.size(), names.elements() );
				}
			};
			public void columnMarginChanged(ChangeEvent e){};
			public void columnSelectionChanged(ListSelectionEvent e){};
		});
		
    addMouseListener( new MouseAdapter() {
			public void mouseEntered(MouseEvent e)
      {
        table.requestFocus();
      }
			public void mousePressed(MouseEvent e)
      {
        table.requestFocus();
      }
		});
    
		if( trackObj.editorObject!=null)
			restoreColumnNames();
}

Vector getTableColumnNames()
{
	Vector names = new Vector();
	for(int i = 0; i< table.getColumnCount(); i++)
		names.add(table.getColumnName(i));

	return names;
}

TableColumn getColumnByName(JTable table, String name)
{
  for(int i=0; i<table.getColumnCount(); i++)
    if( name.equals( table.getColumnName(i)))
      return table.getColumnModel().getColumn(i);
  return null;
}

void restoreColumnNames()
{
	Enumeration names = trackObj.editorObject.getTableColumns();
	if(names == null) return;
	else
	{
		int i = 0;
		String name;
		TableColumn col;
		int idx = -1;
		restoring = true;
		for(Enumeration e = names; e.hasMoreElements();)
		{
			name = (String)e.nextElement();
			col = getColumnByName( table, name);
			if(col != null)
			{
				for(int j = 0; j < table.getColumnCount(); j++)
				{
					if(name.equals(table.getColumnName(j)))
					{
						idx = j;
						break;
					}
				}
				if(i != idx && idx!=-1)
				{
					table.getColumnModel().moveColumn(idx, i);
					table.validate();
				}
				i++;
			}
    }	
		restoring  = false;
		table.revalidate();
		validate();
	}
}
/*
 listSelectionListener interface
 */
Rectangle lastRect;
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
	  
  lastRect = rect;
  SwingUtilities.invokeLater(new Runnable() {
    public void run()
    { 
      table.scrollRectToVisible(lastRect);      
    }
  });
}
/*
 CellEditor for "type" parameter
 */
public class ComboCellEditor extends DefaultCellEditor
{
	JComboBox combo;
	public ComboCellEditor(JComboBox combo)
	{
		super(combo);
		this.combo = combo;
	}
	public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column)
	{
		combo.setSelectedItem(table.getValueAt(row, column));
		return combo;
	}
	public Object getCellEditorValue() {
		return combo.getSelectedItem();
	}
}

/***
 * JMaxTableListener interface
 */
public void deleteSelection()
{
   selection.deleteAll();
}

public int getVerticalTransposition()
{
  return 0;
}

transient TrackTableModel tmodel;
transient FtsTrackObject trackObj;
transient SequenceGraphicContext gc;
SequenceSelection selection;
transient JScrollPane scrollPane; 
transient JTable table;
ComboCellEditor typeEditor;
boolean restoring = false;
static final int COL_WIDTH = 100;
static final int DEF_TABLE_WIDTH = 700;
}


