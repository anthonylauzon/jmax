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

package ircam.jmax.editors.project;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.*;

// import javax.swing.*;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultCellEditor;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;

// import javax.swing.event.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

// import javax.swing.table.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;

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

    /******** Templates Panel ******************************************/
    fileNameCellEditor = new FileNameCellEditor( new JTextField());

    templateTable = new JTable( templateModel){
	public TableCellEditor getCellEditor(int row, int column)
	{
	  switch( column)
	    {
	    default: 
	    case 0:
	      return super.getCellEditor(row, column);
	    case 1:
	      return fileNameCellEditor;
	    }
	}
      }; ;
    templateTable.setPreferredScrollableViewportSize(new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    templateTable.setRowHeight(17);
    templateTable.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);
    
    templateScrollPane = new JScrollPane( templateTable);
    templateScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    /******** PackagePaths Panel ******************************************/

    packagePathList = new JList( packagePathModel);
    packagePathList.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);
    
    packagePathScrollPane = new JScrollPane( packagePathList);
    packagePathScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));

    /******** Help Panel ******************************************/
    
    helpTable = new JTable(helpModel);
    helpTable.setPreferredScrollableViewportSize(new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    helpTable.setRowHeight(17);
    helpTable.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);
    
    helpScrollPane = new JScrollPane(helpTable);
    helpScrollPane.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));
    
    /******** Config Panel ******************************************/
    
    configPanel = new JPanel();
    configPanel.setLayout( new BoxLayout( configPanel, BoxLayout.Y_AXIS));
    configPanel.setPreferredSize(new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT));
    
    JPanel cPanel = new JPanel();
    cPanel.setLayout( new BoxLayout( cPanel, BoxLayout.Y_AXIS));
    cPanel.setBorder( BorderFactory.createTitledBorder( BorderFactory.createEtchedBorder(), "Configuration File"));    
    
    cField = new JTextField();
    cField.setPreferredSize(new Dimension( DEFAULT_WIDTH, 30));
    cField.setMaximumSize(new Dimension( 2000, 30));
    cField.setEditable(false);
    if((ftsPkg instanceof FtsProject) && ((FtsProject)ftsPkg).getConfig() != null)
      cField.setText( ((FtsProject)ftsPkg).getConfig());
    JPanel cButtons = new JPanel();
    cButtons.setLayout( new BoxLayout( cButtons, BoxLayout.X_AXIS));
    JButton cSetButton = new JButton("set");
    cSetButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  JFileChooser fileChooser = new JFileChooser();
	  fileChooser.setFileFilter( configFilter);
	  fileChooser.setCurrentDirectory( new File( ftsPkg.getDir()));
	  int result = fileChooser.showDialog(null, "Choose");
	  if ( result == JFileChooser.APPROVE_OPTION)
	    {
	      String path = fileChooser.getSelectedFile().getAbsolutePath();		  		
	      if(path!=null)
		((FtsProject)ftsPkg).setConfig( path);
	    }
	}
      });
    cSetButton.setPreferredSize( new Dimension( 80, 30));
    cSetButton.setMinimumSize( new Dimension( 80, 30));
    cSetButton.setMaximumSize( new Dimension( 80, 30));
    JButton cResetButton = new JButton("reset");
    cResetButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ((FtsProject)ftsPkg).setConfig( null);
	}
      });
    
    cResetButton.setPreferredSize( new Dimension( 80, 30));
    cResetButton.setMinimumSize( new Dimension( 80, 30));
    cResetButton.setMaximumSize( new Dimension( 80, 30));
    cButtons.add( cSetButton);
    cButtons.add( cResetButton);
    
    cPanel.add( cField);
    cPanel.add( Box.createRigidArea( new Dimension(0, 5)));
    cPanel.add( cButtons);
    
    configPanel.add( cPanel);
    configPanel.add( Box.createVerticalGlue());
    
    /******** TabbedPane ***********************************************/

    tabbedPane = new JTabbedPane();
    tabbedPane.setBorder( BorderFactory.createEtchedBorder());
    tabbedPane.addTab("Packages", requiresScrollPane);
    
    if (ftsPkg instanceof FtsProject)
      tabbedPane.addTab("Package Path", packagePathScrollPane);
    
    tabbedPane.addTab("Data Path", dataPathScrollPane);
    tabbedPane.addTab("Templates", templateScrollPane);         
    tabbedPane.addTab("Template Path", templPathScrollPane);
    tabbedPane.addTab("Help Patches", helpScrollPane);    

    if (ftsPkg instanceof FtsProject)
      tabbedPane.addTab("Audio/MIDI", configPanel);    

    tabbedPane.setSelectedIndex(0);
    
    add( tabbedPane);

    /************ Buttons **********************************/

    addButton = new JButton("Add");
    addButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Add();
	}
      });
    deleteButton = new JButton("Delete");
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
    
    validate();

    tabbedPane.addChangeListener( new ChangeListener(){
	public void stateChanged( ChangeEvent e){
	  boolean enable = (tabbedPane.getSelectedIndex() != tabbedPane.indexOfTab("Audio/MIDI"));
	  addButton.setEnabled( enable);
	  deleteButton.setEnabled( enable);
	}
      });
  }

  void initDataModels( FtsPackage pkg)
  {
    if(pkg == null) return;
    
    requiresModel = new RequiresTableModel( pkg);
    
    for(Enumeration e = pkg.getRequires(); e.hasMoreElements();)
      requiresModel.addRow(e.nextElement(), Boolean.TRUE);    

    packagePathModel = new DefaultListModel();

    for(Enumeration e = pkg.getPackagePaths(); e.hasMoreElements();)
      packagePathModel.addElement(e.nextElement());    

    templPathModel = new DefaultListModel();

    for(Enumeration e = pkg.getTemplatePaths(); e.hasMoreElements();)
      templPathModel.addElement(e.nextElement());    

    dataPathModel = new DefaultListModel();

    for(Enumeration e = pkg.getDataPaths(); e.hasMoreElements();)
      dataPathModel.addElement(e.nextElement());    
  
    templateModel = new TemplateTableModel( pkg);
    
    FtsPackage.TwoNames tn;
    for(Enumeration e = pkg.getTemplates(); e.hasMoreElements();)
      {
	tn = ( FtsPackage.TwoNames)e.nextElement();
	templateModel.addRow(tn.name, tn.fileName);    
      }

    helpModel = new HelpTableModel( pkg);
    
    for(Enumeration e = pkg.getHelps(); e.hasMoreElements();)
      {
	tn = ( FtsPackage.TwoNames)e.nextElement();
	helpModel.addRow(tn.name, tn.fileName);    
      }
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
    
    packagePathList.setModel( packagePathModel);
    packagePathList.revalidate();
    
    templPathList.setModel( templPathModel);
    templPathList.revalidate();
    
    dataPathList.setModel( dataPathModel);
    dataPathList.revalidate();
    
    templateTable.setModel( templateModel);
    templateTable.revalidate();

    helpTable.setModel( helpModel);
    helpTable.revalidate();

    revalidate();    

    tabbedPane.setSelectedIndex(0);    
    if( ftsPkg instanceof FtsProject)
      {
	tabbedPane.setEnabledAt( 5, true);
	
	if( ((FtsProject)ftsPkg).getConfig() != null)
	  cField.setText( ((FtsProject)ftsPkg).getConfig());
	else
	  cField.setText( "");
      }
    else
      tabbedPane.setEnabledAt( 5, false);
    
    window.pack();
  }

  public void packagePathChanged()
  {
    packagePathModel = new DefaultListModel();

    for(Enumeration e = ftsPkg.getPackagePaths(); e.hasMoreElements();)
      packagePathModel.addElement(e.nextElement());   
    
    packagePathList.setModel( packagePathModel);
    packagePathList.revalidate();
    revalidate();    
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

  public void templateChanged()
  {
    templateModel = new TemplateTableModel( ftsPkg);
    
    FtsPackage.TwoNames tn;
    for(Enumeration e = ftsPkg.getTemplates(); e.hasMoreElements();)
      {
	tn = (FtsPackage.TwoNames)e.nextElement();
	templateModel.addRow(tn.name, tn.fileName);    
      }

    templateTable.setModel( templateModel);
    templateTable.revalidate();
    revalidate();
  }

  public void helpChanged()
  {
    helpModel = new HelpTableModel( ftsPkg);
    
    FtsPackage.TwoNames tn;
    for(Enumeration e = ftsPkg.getHelps(); e.hasMoreElements();)
      {
	tn = (FtsPackage.TwoNames)e.nextElement();
	helpModel.addRow(tn.name, tn.fileName);    
      }

    helpTable.setModel( helpModel);
    helpTable.revalidate();
    revalidate();
  }

    public void setErrorAt(int index)
    {
	requiresModel.setErrorAt(index);
    }

  void chooseAndAddPath( DefaultListModel model, String message, int index)
  {
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    fileChooser.setCurrentDirectory( new File( ftsPkg.getDir()));
    
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
      if( selected == packagePathScrollPane)
        chooseAndAddPath( packagePathModel, "package_path", packagePathList.getSelectedIndex());
    else
      if( selected == templPathScrollPane)
	chooseAndAddPath( templPathModel, "template_path", templPathList.getSelectedIndex());
      else
	if( selected == dataPathScrollPane)
	  chooseAndAddPath( dataPathModel, "data_path", dataPathList.getSelectedIndex());
	else
	  if( selected == templateScrollPane)
	    templateModel.addRow( templateTable.getSelectedRow());
	  else
	    helpModel.addRow( helpTable.getSelectedRow());
  }

  void Delete()
  {
    int index, size;
    Component selected = tabbedPane.getSelectedComponent();
    if( selected == requiresScrollPane)
      requiresModel.removeRow( requiresTable.getSelectedRow());
    else
      if( selected == packagePathScrollPane)
        {
            index = packagePathList.getSelectedIndex();
            if( index == -1)
	      {
		size = packagePathList.getModel().getSize(); 
		index = size-1;
	      }

	    if(index >= 0)
	      {
		packagePathModel.removeElementAt( index);
		ftsPkg.set( "package_path", packagePathModel.elements());
	      }
        }
	else
          if( selected == templPathScrollPane)
            {
                index = templPathList.getSelectedIndex();
                if( index == -1)
                {
                    size = templPathList.getModel().getSize(); 
                    index = size-1;
                }

                if(index >= 0)
                {
                    templPathModel.removeElementAt( index);	  
                    ftsPkg.set( "template_path", templPathModel.elements());
                }      
            }
        else
            if( selected == dataPathScrollPane)
            {
                index = dataPathList.getSelectedIndex();
                if( index == -1)
                {
                    size = dataPathList.getModel().getSize(); 
                    index = size-1;
                }

                if(index >= 0)
                {
                    dataPathModel.removeElementAt( index);
                    ftsPkg.set( "data_path", dataPathModel.elements());
                }
            }
            else
                if( selected == templateScrollPane)
                    templateModel.removeRow( templateTable.getSelectedRow());
                else
                    helpModel.removeRow( helpTable.getSelectedRow());
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
	// only row, col==1  with error are editable 
 	return ((col == 0) || (getValueAt(row, col) == Boolean.FALSE));
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
	    if ( data[row][0] != null)
		ftsPackage.set( "require", getRequires());
	}
    }

      public void setErrorAt(int row)
      {
	  if (row > size)
	      return;
	  data[row][1] = Boolean.FALSE;
	  fireTableCellUpdated(row, 1);
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

  /*********************************************************
   ***   Table model for the Help JTable             ***
   *********************************************************/
  class HelpTableModel extends AbstractTableModel 
  {
    HelpTableModel( FtsPackage pkg)
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
      return String.class;
    }

    public boolean isCellEditable(int row, int col)
    {
      return true;
    }

    public String getColumnName(int col)
    {
      if(col == 0)
	return "class name";
      else 
	return "file name";
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
	  ftsPackage.set( "help", getHelps());
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

      fireTableCellUpdated(row, col);
    
      if( ftsPackage != null)
	{
	  if(( data[row][0] != null) && ( data[row][1] != null))
	    ftsPackage.set( "help", getHelps());
	}
    }

    public String[] getHelps() 
    { 
      String values[] = new String[ size*2]; 
      int j = 0;
      for(int i = 0; i < size; i++)
	{
	  values[j] = (String)data[i][0];
	  values[j+1] = (String)data[i][1];
	  j+=2;
	}
      return values;
    }

    int size = 0;
    int rows = 0;
    Object data[][];
    FtsPackage ftsPackage;
  }

  /*********************************************************
   ***   Table model for the Template JTable             ***
   *********************************************************/
  class TemplateTableModel extends AbstractTableModel 
  {
    TemplateTableModel( FtsPackage pkg)
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
      return String.class;
    }

    public boolean isCellEditable(int row, int col)
    {
      return true;
    }

    public String getColumnName(int col)
    {
      if(col == 0)
	return "template name";
      else 
	return "file name";
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
	      if((data[rowId][0] != null) && (data[rowId][1] != null))
		ftsPackage.requestRemoveTemplate( (String)data[rowId][0], (String)data[rowId][1]);

	      for(int i = rowId; i < size; i++)
		{
		  data[i][0] = data[i+1][0];
		  data[i][1] = data[i+1][1];
		}
	    }
	  data[size][0] = null;
	  data[size][1] = null;

	  fireTableDataChanged();
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

      fireTableCellUpdated(row, col);
    
      if( ftsPackage != null)
	{
	  if(( data[row][0] != null) && ( data[row][1] != null))
	    ftsPackage.requestAddTemplate( (String)data[row][0], (String)data[row][1], row);
	}
    }

    public String[] getTemplates() 
    { 
      String values[] = new String[ size*2]; 
      int j = 0;
      for(int i = 0; i < size; i++)
	{
	  values[j] = (String)data[i][0];
	  values[j+1] = (String)data[i][1];
	  j+=2;
	}
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

  public void configChanged( String config)
  {
    if( config != null)
      cField.setText( config);
    else
      cField.setText( "");
  }
  /************* interface Editor ************************/
  public EditorContainer getEditorContainer()
  {
    return (EditorContainer)window;
  }

  public void close(boolean doCancel)
  {
    boolean toClose = true;
    if( ftsPkg.isDirty())
      {
	String message = ( ftsPkg instanceof FtsProject) ? 
	  "Project File is not saved.\nDo you want to save it now?" :
	  "Package File " + ftsPkg.getName() + " is not saved.\nDo you want to save it now?";
	String title =  ( ftsPkg instanceof FtsProject) ? "Project Not Saved" : "Package Not Saved";

	Object[] options = { "Save", "Don't save", "Cancel" };
	int result = JOptionPane.showOptionDialog( window, message, title, 
						   JOptionPane.YES_NO_CANCEL_OPTION,
						   JOptionPane.QUESTION_MESSAGE,
						   null, options, options[0]);
	

	if( result == JOptionPane.CANCEL_OPTION)
	  return;

	if( result == JOptionPane.YES_OPTION)
	  save();	
      }
    window.setVisible(false);
  }

  public void save()
  {
    ProjectEditor.saveWindows();
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
  private JTable requiresTable, templateTable, helpTable;
  private JButton addButton, deleteButton;
  private JList packagePathList, templPathList, dataPathList;
  private JScrollPane requiresScrollPane, packagePathScrollPane, templPathScrollPane, dataPathScrollPane, helpScrollPane, templateScrollPane;
  private RequiresTableModel requiresModel;
  private HelpTableModel helpModel;
  private TemplateTableModel templateModel;
  private DefaultListModel packagePathModel, templPathModel, dataPathModel;
  private Window window;
  private FtsPackage ftsPkg;
  private JFileChooser fileChooser = new JFileChooser(); 
  private final int DEFAULT_WIDTH = 550;
  private final int DEFAULT_HEIGHT = 280;

  private DefaultCellEditor fileNameCellEditor;

  class FileNameCellEditor extends DefaultCellEditor {
    String name = null;

    public FileNameCellEditor(JTextField f) {
      super( f);
      f.setEditable( false);
      f.setFont( (Font)UIManager.get("Table.font"));
      f.addMouseListener(new MouseAdapter() {
	  public void mousePressed( MouseEvent e) {
	    JFileChooser fileChooser = new JFileChooser();
	    fileChooser.setCurrentDirectory( new File( ftsPkg.getDir()));
	    int result = fileChooser.showDialog(null, "Choose");
	    if ( result == JFileChooser.APPROVE_OPTION)
	      {
		String path = fileChooser.getSelectedFile().getAbsolutePath();		  		
		if(path!=null)
		  {
		    name = path;
		    ((JTextField)editorComponent).setText( path);
		  }
		fireEditingStopped();
	      }
	  }
	});

      editorComponent = f;
      setClickCountToStart(2);
    }

    protected void fireEditingStopped() {
      super.fireEditingStopped();
    }

    public Object getCellEditorValue() {
      return name;
    }

    public Component getTableCellEditorComponent(JTable table, 
						 Object value,
						 boolean isSelected,
						 int row,
						 int column) {
      ((JTextField)editorComponent).setText( (String)value);
      name = (String)value;
      return editorComponent;
    }
  }
  
  JTextField cField;
  JPanel configPanel;
  private static javax.swing.filechooser.FileFilter configFilter;
  static
  {
    configFilter = new javax.swing.filechooser.FileFilter(){	
	public boolean accept( File f) {
	  if (f.isDirectory())
	    return true;
	  
	  String name = f.getAbsolutePath();
	  if (name != null)
	    if (name.endsWith(".jcfg"))
	      return true;
	    else
	       return false;
	  return false;
	}
	public String getDescription() {
	  return "jMax Audio/MIDI Config";
	}
      };
  }
}




