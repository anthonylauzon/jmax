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
import javax.swing.tree.*;

import ircam.fts.client.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class ConfigTreePanel extends JPanel {

  public ConfigTreePanel(Window win, ConfigTreeModel model, String[] properties, TreeCellEditor[] editors)  
  {
    this.window = win;
    
    if( model != null)
      treeModel = model;
    else
      treeModel = new ConfigTreeModel( properties);
    
    initEditors(properties, editors);

    tree = new JTree(treeModel);
    /*public TreeCellEditor getCellEditor(){
      //if( column == 0) return super.getCellEditor(row, column);
      //prende la selezione e se e' una foglia prende la root della selezione
      //e guarda nella table. Senno' super.getCelleEditor()
      
      //String name = (String)tableModel.getValueAt(row, 0);
      if((name != null)&&( editorsTable.containsKey( name)))
      return (TreeCellEditor) editorsTable.get( name);
      else
      return super.getCellEditor();
      }
      };*/ 
    tree.putClientProperty("JTree.lineStyle", "Angled");
    tree.setRootVisible( false);
    tree.setEditable( true);
    ConfigRenderer renderer = new ConfigRenderer();
    tree.setCellRenderer( renderer);
    tree.setCellEditor( new ConfigEditor( tree, renderer));
    /*tree.setPreferredScrollableViewportSize(new Dimension(400, 200));
      table.setRowHeight(17);
      table.getColumnModel().getColumn(0).setPreferredWidth( 150);
      table.getColumnModel().getColumn(0).setMaxWidth( 150);    
      table.getColumnModel().getColumn(0).setCellEditor( createComboCellEditor( properties));*/

    tree.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION);

    JScrollPane scrollPane = new JScrollPane(tree);
    scrollPane.setPreferredSize(new Dimension(400, 200));

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    add( scrollPane);

    JPanel buttons = new JPanel();
    buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
    JButton removeButton = new JButton("remove");
    removeButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  //treeModel.removeRow( tree.getSelectedRow());
	}
      });
    JButton addButton = new JButton("add");
    addButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  //treeModel.addRow();
	}
      });
    JButton applyButton = new JButton("save");
    applyButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  treeModel.save();
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

  void initEditors(String[] names, TreeCellEditor[] editors)
  {
    this.names = names;

    if(editors == null) return;

    for(int i = 0 ; i<editors.length; i++)      
      {      
	if(editors[i] != null)
	  editorsTable.put(names[i], editors[i]);
      }
  }

  public void update(ConfigTreeModel model)
  {
    treeModel = model;
    tree.setModel( treeModel);
    tree.revalidate();
    revalidate();
  }

  /************** Predefined TreeCellEditors **************/
  public static TreeCellEditor createComboCellEditor(String[] items)
  {
    JComboBox combo = new JComboBox( items);
    combo.setBackground( Color.white);
    combo.setFont( (Font)UIManager.get("Tree.font"));
    DefaultCellEditor editor = new DefaultCellEditor( combo);
    editor.setClickCountToStart(2); 
    return editor;
  }

  public static TreeCellEditor createCheckBoxCellEditor()
  {
    JCheckBox cb = new JCheckBox();
    cb.setBackground( Color.white);
    cb.setFont( (Font)UIManager.get("Tree.font"));
    DefaultCellEditor editor = new DefaultCellEditor( cb);
    editor.setClickCountToStart(2); 
    return editor;
  }

  public static TreeCellEditor createPathCellEditor()
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
      button.setFont((Font)UIManager.get("Tree.font"));
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
    public Component getTreeCellEditorComponent(JTree tree, Object value, boolean isSelected, boolean expanded, boolean leaf, int row) 
    {      
      if( value != null)
	((JButton)editorComponent).setText( (String)value);
      else
	((JButton)editorComponent).setText( "");
      currentPath = (String)value;

      return editorComponent;
    }
  }

  class ConfigRenderer extends DefaultTreeCellRenderer {
    public Component getTreeCellRendererComponent( JTree tree, Object value, boolean sel, boolean expanded,
						  boolean leaf, int row, boolean hasFocus) {
      super.getTreeCellRendererComponent( tree, value, sel, expanded, leaf, row, hasFocus);
      
      if ( leaf)
	if(isRootNode( value))
	  setIcon( JMaxIcons.projectRoot);
	else
	  setIcon( null);

      return this;
    }

    protected boolean isRootNode(Object value) {
      return editorsTable.containsKey( ((DefaultMutableTreeNode)value).getUserObject());
    }
  }

  class ConfigEditor extends DefaultTreeCellEditor {
    
    ConfigEditor(JTree tree, DefaultTreeCellRenderer renderer)
    {
      super( tree, renderer);
    }

    public Component getTreeCellEditorComponent( JTree tree, Object value, boolean sel, boolean expanded,
						 boolean leaf, int row) {
      //super.getTreeCellRendererComponent( tree, value, sel, expanded, leaf, row, hasFocus);      
      /*if ( leaf)
	if(isRootNode( value))
	setIcon( JMaxIcons.projectRoot);
	else
	setIcon( null);*/
      String name = (String)((DefaultMutableTreeNode)value).getUserObject();
      if( editorsTable.containsKey( name))
	realEditor = (TreeCellEditor) editorsTable.get( name);

      return super.getTreeCellEditorComponent( tree, value, sel, expanded, leaf, row);
    }

    protected boolean isRootNode(Object value) {
      return editorsTable.containsKey( ((DefaultMutableTreeNode)value).getUserObject());
    }
  }

  /********************************/
  private Hashtable editorsTable = new Hashtable();
  private JTree tree;
  private Window window;
  private ConfigTreeModel treeModel;
  String names[];
}





