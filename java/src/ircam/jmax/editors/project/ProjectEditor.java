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
import javax.swing.table.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.filechooser.*;
import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

public class ProjectEditor extends JFrame implements EditorContainer
{    
  public ProjectEditor(FtsPackage pkg)
  {
    super();

    if( pkg instanceof FtsProject)
      setTitle("Project Editor: current project");
    else
      setTitle("Package Editor: "+pkg.getName());

    makeMenuBar();    

    packagePanel = new ConfigPackagePanel( this, pkg);
    getContentPane().add( packagePanel);
    
    validate();
    pack();

    setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    addWindowListener(new WindowAdapter(){
	public void windowClosing(WindowEvent e)
	{
	  packagePanel.close(false);    
	  MaxWindowManager.getWindowManager().removeWindow( ProjectEditor.this);
	}
      });
  }

  private void makeMenuBar()
  {
    JMenuBar mb = new JMenuBar();

    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 7);
    mb.add( fileMenu); 
    mb.add( new DefaultProjectMenu()); 
    mb.add( new MaxWindowJMenu( "Windows", this));
    mb.add(Box.createHorizontalGlue());
    mb.add( new DefaultHelpMenu());
    
    setJMenuBar( mb);
  }

  public static void newProject( Frame frame)
  {
    int result = NewProjectDialog.showDialog( frame, NewProjectDialog.PROJECT_TYPE);
    if(result == NewProjectDialog.CREATE_OPTION)
      {
	FtsProject newProject = null;
	String name = NewProjectDialog.getResultName();
	String location = NewProjectDialog.getResultLocation();
	
	try{
	  newProject = new FtsProject();
	}
	catch(IOException e)
	  {
	    System.err.println("[ProjectEditor]: Error in FtsProject creation!");
	    e.printStackTrace();
	  }
	newProject.save( location+name);
	newProject.setFileName( location+name);
	newProject.setDir( location);
	JMaxApplication.setCurrentProject( newProject);
	newProject.setAsCurrentProject();
      }
  }

  public static void openProject( Frame frame)
  {
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    fileChooser.setFileFilter( projectFilter);
    int result = fileChooser.showOpenDialog( frame);

    if ( result == JFileChooser.APPROVE_OPTION)
      {
	File project = fileChooser.getSelectedFile();	

	if ( project != null)
	  {
	    try
	      {	
		JMaxApplication.loadProject( project.getAbsolutePath());
	      }
	    catch(IOException e)
	      {
		System.err.println("[ProjectEditor]: I/O error loading project "+project.getAbsolutePath());
	      }
	  }
      }
  }

  public static void newPackage( Frame frame)
  {
    int result = NewProjectDialog.showDialog( frame, NewProjectDialog.PACKAGE_TYPE);
    if(result == NewProjectDialog.CREATE_OPTION)
      {
	FtsPackage newPackage = null;
	String name = NewProjectDialog.getResultName();
	String location = NewProjectDialog.getPackageLocation();
	
	try{
	  newPackage = new FtsPackage();
	}
	catch(IOException e)
	  {
	    System.err.println("[ProjectEditor]: Error in FtsPackage creation!");
	    e.printStackTrace();
	  }
	newPackage.save( location+name);
	newPackage.setFileName( location+name);
	newPackage.setName( name);
	newPackage.setDir( location);
	newPackage.requestOpenEditor();
      }
  }

  public static void editPackage( Frame frame)
  {
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    fileChooser.setFileFilter( packageFilter);
    int result = fileChooser.showOpenDialog( frame);

    if ( result == JFileChooser.APPROVE_OPTION)
      {
	File pkg = fileChooser.getSelectedFile();	

	if ( pkg != null)
	  {
	    String name = pkg.getName();
	     
	    if(name == null) return;

	    if(name.endsWith(".jpkg"))
	       {
		 int idx = name.indexOf(".jpkg"); 
		 name = name.substring( 0, idx);
	       }
	    try
	      {	
		JMaxApplication.loadPackage( name, pkg.getAbsolutePath());
	      }
	    catch(IOException e)
	      {
		System.err.println("[ProjectEditor]: I/O error loading package "+pkg.getAbsolutePath());
	      }
	  }
      }
  }
  /************* interface EditorContainer ************************/
  public Editor getEditor()
  {
    return packagePanel;
  }
  public Frame getFrame()
  {
    return this;
  }
  public Point getContainerLocation()
  {
    return getLocation();
  }
  public Rectangle getViewRectangle()
  {
    return getContentPane().getBounds();
  }
  /**************************************************************/

  private ConfigPackagePanel packagePanel;
  private static JFileChooser fileChooser = new JFileChooser(); 
  private static javax.swing.filechooser.FileFilter projectFilter, packageFilter, configFilter;
  static
  {
     projectFilter = new javax.swing.filechooser.FileFilter(){	
	 public boolean accept( File f) {
	   if (f.isDirectory())
	     return true;
	  
	   String name = f.getAbsolutePath();
	   if (name != null)
	     if (name.endsWith(".jprj"))
	       return true;
	     else
	       return false;
	   return false;
	 }
	 public String getDescription() {
	   return "jMax Projects";
	 }
       };
     packageFilter = new javax.swing.filechooser.FileFilter(){	
	 public boolean accept( File f) {
	   if (f.isDirectory())
	     return true;
	   
	   String name = f.getAbsolutePath();
	   if (name != null)
	     if (name.endsWith(".jpkg"))
	       return true;
	     else
	       return false;
	   return false;
	 }
	 public String getDescription() {
	   return "jMax Packages";
	 }
       };
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

