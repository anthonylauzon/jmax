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

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "system statistics" dialog.
 */

public class NewProjectDialog extends JDialog 
{
  public NewProjectDialog( Frame dw, int type) 
  {
    super(dw, (type == PROJECT_TYPE) ? "New Project" : "New Package", true);

    parent = (Frame)dw;
    this.type = type;

    JPanel titlePanel = new JPanel();    
    titlePanel.setLayout(new BoxLayout(titlePanel, BoxLayout.X_AXIS));
    JLabel iconLabel = new JLabel( JMaxIcons.jmaxIcon);
    JLabel titleLabel = new JLabel();
    titleLabel.setFont( titleLabel.getFont().deriveFont( (float)36));
    titleLabel.setText((type == PROJECT_TYPE) ? "New Project" : "New Package");
    titlePanel.add( iconLabel);
    titlePanel.add( Box.createRigidArea( new Dimension(20, 0)));
    titlePanel.add( titleLabel);
    titlePanel.add( Box.createHorizontalGlue());

    /****************  Project Name ***************************************/
    JPanel namePanel = new JPanel();    
    namePanel.setLayout(new BoxLayout(namePanel, BoxLayout.X_AXIS));
    namePanel.setBorder( BorderFactory.createTitledBorder( BorderFactory.createEmptyBorder(), 
							   (type == PROJECT_TYPE) ? "Project Name" : "Package Name"));
    nameField = new JTextField();
    nameField.addActionListener( new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  enableCreateButton();
	}
      });
    nameField.setPreferredSize( new Dimension( 400, 25));
    nameField.setMaximumSize( new Dimension( 2000, 25));
    namePanel.add( nameField);
    
    /****************************** Project Location  ********************/
    JPanel pathPanel = new JPanel();
    pathPanel.setLayout(new BoxLayout(pathPanel, BoxLayout.X_AXIS));
    pathPanel.setBorder( BorderFactory.createTitledBorder( BorderFactory.createEmptyBorder(), "Location"));
    pathField = new JTextField()
      {
	public void setText(String text)
	{
	  super.setText( text);
	  enableCreateButton();
	}
      };
    
    pathField.setPreferredSize( new Dimension( 400, 25));
    pathField.setMaximumSize( new Dimension( 2000, 25));
    JButton pathButton = new JButton("Choose...");
    pathButton.addActionListener( new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  JFileChooser fileChooser = new JFileChooser();
	  fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	  fileChooser.setSelectedFile( null);
	  
	  int result = fileChooser.showDialog(null, "Choose");
	  if ( result == JFileChooser.APPROVE_OPTION)
	    {
	      String path = fileChooser.getSelectedFile().getAbsolutePath();		  		
	      if(path!=null)
		pathField.setText( path);
	    }
	}
      });
    pathPanel.add( pathField);
    pathPanel.add( pathButton);

    /******************** CheckBox "Init as Copy of Current" ****************/ 
    JPanel checkPanel = new JPanel();
    checkPanel.setLayout(new BoxLayout( checkPanel, BoxLayout.X_AXIS));    
    copyCheckBox = new JCheckBox("Copy current project");
    copyCheckBox.setSelected( true);
    checkPanel.add( Box.createRigidArea( new Dimension( 5, 0)));
    checkPanel.add( copyCheckBox);
    checkPanel.add( Box.createHorizontalGlue());

    JPanel borderedPanel = new JPanel();
    borderedPanel.setLayout(new BoxLayout(borderedPanel, BoxLayout.Y_AXIS));
    borderedPanel.setBorder( BorderFactory.createEtchedBorder());
    borderedPanel.add( namePanel);
    borderedPanel.add( pathPanel);
    borderedPanel.add( checkPanel);
    
    /********** buttons ************************/
    JPanel buttonsPanel = new JPanel();
    buttonsPanel.setLayout(new BoxLayout(buttonsPanel, BoxLayout.X_AXIS));
    createButton = new JButton("Create");
    createButton.addActionListener( new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  //create the project
	  returnValue = CREATE_OPTION;
	  setVisible(false);
	}
      });
    createButton.setEnabled( false);
    JButton cancelButton = new JButton("Cancel");
    cancelButton.addActionListener( new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{	  
	  returnValue = CANCEL_OPTION;
	  setVisible(false);
	}
      });
    buttonsPanel.add(cancelButton);
    buttonsPanel.add(Box.createHorizontalGlue());
    buttonsPanel.add(createButton);

    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
    mainPanel.add( titlePanel);
    mainPanel.add( Box.createRigidArea( new Dimension(0, 20)));
    mainPanel.add( borderedPanel);
    mainPanel.add( Box.createVerticalGlue());
    mainPanel.add( Box.createRigidArea( new Dimension(0, 10)));
    mainPanel.add( buttonsPanel);

    getContentPane().add( mainPanel);
    getContentPane().validate();
    pack();  

    setResizable( false);
    setLocationRelativeTo( parent);
    setVisible( true);
  }

  void enableCreateButton()
  {
    String name = nameField.getText();
    String path = pathField.getText();
    if( name != null) name.trim();
    if( path != null) path.trim();

    createButton.setEnabled( (name!=null)&&(path!=null)&&(!name.equals("")&&(!path.equals(""))));
  }

  public static int showDialog(Frame frame, int type)
  {
    instance = new NewProjectDialog( frame, type);
    System.gc();
    return instance.returnValue;
  }
  public static String getResultName()
  {
    String name = instance.nameField.getText();
    
    if( instance.type == PROJECT_TYPE)
      { 
	if( !name.endsWith(".jprj")) name = name.concat(".jprj");
      }      
    else
      if( !name.endsWith(".jpkg")) name = name.concat(".jpkg");
    
    return name;
  }
  public static String getResultLocation()
  {
    String location = instance.pathField.getText();
     if( !location.endsWith("/"))
      location = location.concat("/");
     return location;
  }

  public static String getPackageLocation()
  {
    String location = instance.pathField.getText();
    String pkgName = instance.nameField.getText();
    if( pkgName.endsWith(".jpkg"))
      {
	int idx = pkgName.indexOf(".jpkg"); 
	pkgName = pkgName.substring( 0, idx-1);
      }

    if( !location.endsWith("/"))
      location = location.concat("/");
    if( !location.endsWith(pkgName+"/"))
      location = location.concat(pkgName+"/");

    File dir = new File( location);
    if( !dir.exists())
      dir.mkdir();
    
    return location;
  }

  public static boolean copyCurrentProject()
  {
    return instance.copyCheckBox.isSelected();
  }

  private static NewProjectDialog instance;

  Frame parent;
  JTextField nameField, pathField;
  JButton createButton;
  JCheckBox copyCheckBox;

  public static final int CANCEL_OPTION = 0;
  public static final int CREATE_OPTION = 1;
  int returnValue = CANCEL_OPTION; 
  public static final int PROJECT_TYPE = 0;
  public static final int PACKAGE_TYPE = 1;
  private int type = PROJECT_TYPE;
}










