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

package ircam.jmax.editors.project;

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

public class ConfigPackagePanel extends JPanel implements Editor
{
  public ConfigPackagePanel(Window win, FtsPackage pkg)  
  {
    window = win;
    ftsPkg = pkg;

    ftsPkg.setPackageListener( this);

    initDataModels( ftsPkg);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    setBorder( BorderFactory.createEtchedBorder());

    /******** Requires Panel ******************************************/

    requiresTable = new JTable(requiresModel);
    requiresTable.setPreferredScrollableViewportSize(new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    requiresTable.setRowHeight(17);
    requiresTable.getColumnModel().getColumn(1).setPreferredWidth( 50);
    requiresTable.getColumnModel().getColumn(1).setMaxWidth( 50);  
    requiresTable.getColumnModel().getColumn(1).setResizable( false);  
    requiresTable.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);

    requiresScrollPane = new JScrollPane(requiresTable);
    requiresScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    /******** TemplatePaths Panel ******************************************/

    templPathList = new JList( templPathModel);
    templPathList.setSelectionMode(  ListSelectionModel.SINGLE_SELECTION);
    
    templPathScrollPane = new JScrollPane( templPathList);
    templPathScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    /******** DataPaths Panel ******************************************/

    dataPathList = new JList( dataPathModel);
    dataPathList.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);

    dataPathScrollPane = new JScrollPane( dataPathList);
    dataPathScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    /******** TabbedPane ***********************************************/

    tabbedPane = new JTabbedPane();
    tabbedPane.setBorder( BorderFactory.createEtchedBorder());
    tabbedPane.addTab("Packages", requiresScrollPane);
    tabbedPane.addTab("Template Path", templPathScrollPane);
    tabbedPane.addTab("Data Path", dataPathScrollPane);    
    tabbedPane.setSelectedIndex(0);

    add( tabbedPane);

    /************ Buttons **********************************/
    JButton addButton = new JButton("Add");
    addButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Add();
	}
      });
    JButton deleteButton = new JButton("Delete");
    deleteButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Delete();
	}
      });
    
    JPanel buttons = new JPanel();
    buttons.setLayout(new BoxLayout( buttons, BoxLayout.X_AXIS));    
    buttons.add( addButton);
    buttons.add( deleteButton);
    buttons.add( Box.createHorizontalGlue());
    
    add( tabbedPane);
    add( buttons);
  }

  void initDataModels( FtsPackage pkg)
  {
    if(pkg == null) return;
    
    requiresModel = new RequiresTableModel( pkg);
    
    for(Enumeration e = pkg.getRequires(); e.hasMoreElements();)
      requiresModel.addRow(e.nextElement(), Boolean.TRUE);    

    templPathModel = new DefaultListModel();

    for(Enumeration e = pkg.getTemplatePaths(); e.hasMoreElements();)
      templPathModel.addElement(e.nextElement());    

    dataPathModel = new DefaultListModel();

    for(Enumeration e = pkg.getDataPaths(); e.hasMoreElements();)
      dataPathModel.addElement(e.nextElement());    
  }

  void setPackage( FtsPackage pkg)
  {
    if(ftsPkg == pkg) return;

    ftsPkg = pkg;
    
    ftsPkg.setPackageListener( this);
  }

  public void update()
  {    
    initDataModels( ftsPkg);
    requiresTable.setModel( requiresModel);
    requiresTable.getColumnModel().getColumn(1).setPreferredWidth( 50);
    requiresTable.getColumnModel().getColumn(1).setMaxWidth( 50);  
    requiresTable.getColumnModel().getColumn(1).setResizable( false);
    requiresTable.revalidate();

    templPathList.setModel( templPathModel);
    templPathList.revalidate();

    dataPathList.setModel( dataPathModel);
    dataPathList.revalidate();
    revalidate();    

    window.pack();
  }

  public void dataPathChanged()
  {
    dataPathModel = new DefaultListModel();

    for(Enumeration e = ftsPkg.getDataPaths(); e.hasMoreElements();)
      dataPathModel.addElement(e.nextElement());   

    dataPathList.setModel( dataPathModel);
    dataPathList.revalidate();
    revalidate();    
  }

  public void templatePathChanged()
  {
    templPathModel = new DefaultListModel();

    for(Enumeration e = ftsPkg.getTemplatePaths(); e.hasMoreElements();)
      templPathModel.addElement(e.nextElement());   

    templPathList.setModel( templPathModel);
    templPathList.revalidate();
    revalidate();    
  }

  void chooseAndAddPath( DefaultListModel model, String message, int index)
  {
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    fileChooser.setSelectedFile( null);
    
    int result = fileChooser.showDialog(null, "Select Path");
    if ( result == JFileChooser.APPROVE_OPTION)
      {
	String path = fileChooser.getSelectedFile().getAbsolutePath();		  		
	if(path!=null)
	  {
	    if(index == -1)
	      model.addElement(path);
	    else
	      model.add(index+1, path);
	  
	    ftsPkg.set( message, model.elements());
	  }
      }
  }

  void Add()
  {
    Component selected = tabbedPane.getSelectedComponent();
    if( selected == requiresScrollPane)
      requiresModel.addRow( requiresTable.getSelectedRow());
    else
      if( selected == templPathScrollPane)
	  chooseAndAddPath( templPathModel, "template_path", templPathList.getSelectedIndex());
      else
	chooseAndAddPath( dataPathModel, "data_path", dataPathList.getSelectedIndex());
  }

  void Delete()
  {
    int index;
    Component selected = tabbedPane.getSelectedComponent();
    if( selected == requiresScrollPane)
      requiresModel.removeRow( requiresTable.getSelectedRow());
    else
      if( selected == templPathScrollPane)
	{
	  index = templPathList.getSelectedIndex();
	  if(index != -1)
	    {
	      templPathModel.removeElementAt( index);	  
	      ftsPkg.set( "template_path", templPathModel.elements());
	    }      
	}
      else
	{
	  index = dataPathList.getSelectedIndex();
	  if(index != -1)
	    {
	      dataPathModel.removeElementAt( index);
	      ftsPkg.set( "data_path", dataPathModel.elements());
	    }
	}
  }

  /*********************************************************
   ***   Table model for the Requires JTable             ***
   *********************************************************/
  class RequiresTableModel extends AbstractTableModel 
  {
    RequiresTableModel( FtsPackage pkg)
    {
      super();
      ftsPackage = pkg;
    }

    public int getColumnCount() 
    { 
      return 2;
    }
  
    public Class getColumnClass(int col)
    {
      if(col == 0)
	return String.class;
      else
	return Boolean.class;
    }

    public boolean isCellEditable(int row, int col)
    {
      return (col == 0);
    }

    public String getColumnName(int col)
    {
      if(col == 0)
	return "package";
      else 
	return "used";
    }

    public int getRowCount() { 
      return size; 
    }

    public void addRow(int index)
    {
      size++;    
      if((size > rows) || (index != -1))
	{
	  Object[][] temp = new Object[size+5][2];
	  if(index == -1)
	    for(int i = 0; i < size-1; i++)
	      {
		temp[i][0] = data[i][0];
		temp[i][1] = data[i][1];
	      }
	  else
	    {
	      for(int i = 0; i < index+1; i++)
	      {
		temp[i][0] = data[i][0];
		temp[i][1] = data[i][1];
	      }

	      temp[index+1][0] = null;
	      temp[index+1][1] = null;

	      for(int j = index+2; j < size; j++)
	      {
		temp[j][0] = data[j-1][0];
		temp[j][1] = data[j-1][1];
	      }
	    }
	  data = temp;
	  rows = size+5;
	}
      fireTableDataChanged();
    }

    public void addRow(Object v1, Object v2)
    {
      addRow(-1);
      data[size-1][0] = v1;
      data[size-1][1] = v2;
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
		}
	    }
	  data[size][0] = null;
	  data[size][1] = null;

	  fireTableDataChanged();
	  ftsPackage.set( "require", getRequires());
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

      data[row][col] = value;
      data[row][1] = Boolean.TRUE;

      fireTableCellUpdated(row, col);
    
      if( ftsPackage != null)
	{
	  if(( col==0) && ( data[row][0] != null))
	    ftsPackage.set( "require", getRequires());
	}
    }

    public String[] getRequires() 
    { 
      String values[] = new String[size]; 
      for(int i=0; i<size; i++)
	values[i] = (String)data[i][0];
      return values;
    }

    int size = 0;
    int rows = 0;
    Object data[][];
    FtsPackage ftsPackage;
  }

  public void updateDone()
  {
    update();
    window.setVisible( true);
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

  public void save()
  {
    ftsPkg.save( null);
  }

  public void saveAs()
  {
    fileChooser.setSelectedFile( null);
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int result = fileChooser.showSaveDialog( window);

    if ( result == JFileChooser.APPROVE_OPTION)
      {
	String fileName = fileChooser.getSelectedFile().getAbsolutePath();		  		
	if( fileName != null)
	  ftsPkg.save( fileName);
      }
  }
  public void print()
  {
  } 
  /********************************/
  private JTabbedPane tabbedPane;
  private JTable requiresTable;
  private JList templPathList, dataPathList;
  private JScrollPane requiresScrollPane, templPathScrollPane, dataPathScrollPane;
  private RequiresTableModel requiresModel;
  private DefaultListModel templPathModel, dataPathModel;
  private Window window;
  private FtsPackage ftsPkg;
  private JFileChooser fileChooser = new JFileChooser(); 
  private final int DEFAULT_WIDTH = 450;
  private final int DEFAULT_HEIGHT = 280;
}




