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

import javax.swing.*;
import javax.swing.table.*;
//import javax.swing.tree.*;
import java.util.*;

import ircam.fts.client.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

public class ProjectEditor extends JFrame 
{    
  static void registerProjectEditor()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
	public String getToolName() { return "Project Editor";}
	public void open() { ProjectEditor.open();}
      });
  }

  public static ProjectEditor open()
  {
    if (projectEditor == null)
      projectEditor = new ProjectEditor();
    else
      projectEditor.update();
    
    projectEditor.setVisible(true);
    
    return projectEditor;
  }

  
  protected ProjectEditor()
  {
    super( "Project Editor");
    
    String[] names = {"require", "template_path", "abstraction_template", "data_path"};
    TableCellEditor[] editors = { null, 
				  ConfigTablePanel.createPathCellEditor(), 
    ConfigTablePanel.createPathCellEditor(), 
				  ConfigTablePanel.createPathCellEditor()};
    
    initDataModel( JMaxApplication.getProject());
    
    tablePanel = new ConfigTablePanel( this, model, names, editors);
    getContentPane().add( tablePanel);
    validate();
    pack();
  }

  void initDataModel( FtsPackage project)
  {
    if(project == null) return;
    
    model = new ConfigTableModel();
    
    for(Enumeration e = project.getRequires(); e.hasMoreElements();)
      model.addRow("require", e.nextElement());
    
    for(Enumeration e = project.getTemplatePaths(); e.hasMoreElements();)
      model.addRow("template_path", e.nextElement());
    
    for(Enumeration e = project.getAbstractionPaths(); e.hasMoreElements();)
      model.addRow("abstraction_path", e.nextElement());
    
    for(Enumeration e = project.getDataPaths(); e.hasMoreElements();)
      model.addRow("data_path", e.nextElement());    
    
    model.setFtsPackage( project);
  }
  
  void update()
  {
    initDataModel( JMaxApplication.getProject());
    tablePanel.update( model);
  }  
  private ConfigTablePanel tablePanel;
  private ConfigTableModel model;
  private static ProjectEditor projectEditor = null;

  /*protected ProjectEditor()
    {
    super( "Project Editor");
    
    initDataModel( JMaxApplication.getProject());
    
    treePanel = new ConfigTreePanel( this, model, names, editors);
    getContentPane().add( treePanel);
    
    validate();
    pack();
    }

    void initDataModel( FtsPackage project)
    {
    if(project == null) return;
    
    model = new ConfigTreeModel( names);
    
    for(Enumeration e = project.getRequires(); e.hasMoreElements();)
    model.addRow("require", (String)e.nextElement());
    
    for(Enumeration e = project.getTemplatePaths(); e.hasMoreElements();)
    model.addRow("template_path", (String)e.nextElement());
    
    for(Enumeration e = project.getAbstractionPaths(); e.hasMoreElements();)
    model.addRow("abstraction_path", (String)e.nextElement());
    
    for(Enumeration e = project.getDataPaths(); e.hasMoreElements();)
    model.addRow("data_path", (String)e.nextElement());    
    
    model.setFtsPackage( project);
    }

    void update()
    {
    initDataModel( JMaxApplication.getProject());
    treePanel.update( model);
    }

    private ConfigTreePanel treePanel;
    private ConfigTreeModel model;
    private static ProjectEditor projectEditor = null;
    String[] names = {"require", "template_path", "abstraction_template", "data_path"};
    TreeCellEditor[] editors = { null, ConfigTreePanel.createPathCellEditor(), ConfigTreePanel.createPathCellEditor(), 
    ConfigTreePanel.createPathCellEditor()};*/
}

