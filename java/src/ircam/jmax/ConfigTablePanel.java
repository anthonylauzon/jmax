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

package ircam.jmax;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;

import ircam.fts.client.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class ConfigTablePanel extends JPanel {

  public ConfigTablePanel(Window win, ConfigTableModel model, String[] properties, TableCellEditor[] editors)  
  {
    this.window = win;
    
    if( model != null)
      tableModel = model;
    else
      tableModel = new ConfigTableModel();
    
    initEditors(properties, editors);

    table = new JTable(tableModel){
	public TableCellEditor getCellEditor(int row, int column){
	  if( column == 0) return super.getCellEditor(row, column);
	  
	  String name = (String)tableModel.getValueAt(row, 0);
	  if((name != null)&&( editorsTable.containsKey( name)))
	    return (TableCellEditor) editorsTable.get( name);
	  else
	    return super.getCellEditor(row, column);
	}
      }; 

    table.setPreferredScrollableViewportSize(new Dimension(400, 200));
    table.setRowHeight(17);
    table.getColumnModel().getColumn(0).setPreferredWidth( 150);
    table.getColumnModel().getColumn(0).setMaxWidth( 150);    
    table.getColumnModel().getColumn(0).setCellEditor( createComboCellEditor( properties));

    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

    JScrollPane scrollPane = new JScrollPane(table);
    scrollPane.setPreferredSize(new Dimension(400, 200));

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    add( scrollPane);

    JPanel buttons = new JPanel();
    buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
    JButton removeButton = new JButton("remove");
    removeButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  tableModel.removeRow( table.getSelectedRow());
	}
      });
    JButton addButton = new JButton("add");
    addButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  tableModel.addRow();
	}
      });
    JButton applyButton = new JButton("save");
    applyButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  tableModel.save();
	}
      });
    JButton cancelButton = new JButton("cancel");
    cancelButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  window.setVisible(false);
	}
      });

    buttons.add(Box.createRigidArea(new Dimension( 1, 0)));
    buttons.add(addButton);
    buttons.add(removeButton);
    buttons.add(Box.createHorizontalGlue());   
    buttons.add(applyButton);
    buttons.add(cancelButton);
    
    add(buttons);
  }

  void initEditors(String[] names, TableCellEditor[] editors)
  {
    this.names = names;

    if(editors == null) return;

    for(int i = 0 ; i<editors.length; i++)      
      {      
	if(editors[i] != null)
	  editorsTable.put(names[i], editors[i]);
      }
  }

  public void update(ConfigTableModel model)
  {
    tableModel = model;
    table.setModel( tableModel);
    table.revalidate();
    revalidate();
  }

  /************** Predefined TableCellEditors **************/
  public static TableCellEditor createComboCellEditor(String[] items)
  {
    JComboBox combo = new JComboBox( items);
    combo.setBackground( Color.white);
    combo.setFont( (Font)UIManager.get("Table.font"));
    DefaultCellEditor editor = new DefaultCellEditor( combo);
    editor.setClickCountToStart(2); 
    return editor;
  }

  public static TableCellEditor createCheckBoxCellEditor()
  {
    JCheckBox cb = new JCheckBox();
    cb.setBackground( Color.white);
    cb.setFont( (Font)UIManager.get("Table.font"));
    DefaultCellEditor editor = new DefaultCellEditor( cb);
    editor.setClickCountToStart(2); 
    return editor;
  }

  public static TableCellEditor createPathCellEditor()
  {
    return new PathEditor();
  }

  static public class PathEditor extends DefaultCellEditor 
  {        
    String currentPath = null;
    JButton button = new JButton();
    JFileChooser fileChooser = new JFileChooser();
    
    public PathEditor() 
    {
      super(new JCheckBox());
      editorComponent = button;
      
      fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);

      button.setBackground(Color.white);
      button.setFont((Font)UIManager.get("Table.font"));
      button.setHorizontalTextPosition(SwingConstants.LEFT);
      button.setBorderPainted(false);
      button.setMargin(new Insets(0,0,0,0));    
      button.addActionListener(new ActionListener() {
	  public void actionPerformed(ActionEvent e) 
	  {
	    if( currentPath != null)
	      fileChooser.setSelectedFile( new File(currentPath));
	    else
	      fileChooser.setSelectedFile( null);

	    int result = fileChooser.showDialog(null, "Select the Path ");
	    if ( result == JFileChooser.APPROVE_OPTION)
	      {
		String path = fileChooser.getSelectedFile().getAbsolutePath();		  		
		if(path!=null)
		  {
		    currentPath = path;
		    button.setText(path);
		  }		
	      }
	    stopCellEditing();
	  }
	});
      setClickCountToStart(2); 
    }    
    public Object getCellEditorValue() 
    {
      return currentPath;
    }    
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) 
    {      
      if( value != null)
	((JButton)editorComponent).setText( (String)value);
      else
	((JButton)editorComponent).setText( "");
      currentPath = (String)value;

      return editorComponent;
    }
  }

  /********************************/
  private Hashtable editorsTable = new Hashtable();
  private JTable table;
  private Window window;
  private ConfigTableModel tableModel;
  String names[];
}




