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
import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

public class ProjectEditor extends JFrame implements EditorContainer
{    
  public static void registerProjectEditor()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
	public String getToolName() { return "Project Editor";}
	public void open() { ProjectEditor.editCurrent();}
      });
  }

  public static ProjectEditor editCurrent()
  {    
    if (projectEditor == null)
      projectEditor = new ProjectEditor();
    else
      projectEditor.update();
    
    projectEditor.setVisible(true);
    
    return projectEditor;
  }

  public static ProjectEditor getInstance()
  {
    return projectEditor;
  }

  protected ProjectEditor()
  {
    super( "Project Editor");

    makeMenuBar();    

    packagePanel = new ConfigPackagePanel( this, JMaxApplication.getProject());
    getContentPane().add( packagePanel);
    
    validate();
    pack();
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

  void update()
  {
    packagePanel.setPackage( JMaxApplication.getProject());
    packagePanel.update();
  } 

  public static void newProject( Frame frame)
  {
    int result = NewProjectDialog.showDialog( frame);
    if(result == NewProjectDialog.CREATE_OPTION)
      {
	FtsProject newProject = null;
	String name = NewProjectDialog.getProjectName();
	String location = NewProjectDialog.getProjectLocation();
	
	try{
	  newProject = new FtsProject();
	}
	catch(IOException e)
	  {
	    System.err.println("Error in FtsProject creation!");
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
    File project = MaxFileChooser.chooseFileToOpen( frame);
    if ( project!= null)
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
  private static ProjectEditor projectEditor = null;
}

