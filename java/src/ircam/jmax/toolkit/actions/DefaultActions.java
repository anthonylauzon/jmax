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

package ircam.jmax.toolkit.actions;

import java.awt.event.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.project.*;
import ircam.jmax.editors.configuration.*;

/** This class define a set of static variables 
 *  containing all the standard actions used for the
 *  patcher editor; please notes that actions objects are
 *  shared between all the editor instances.
 */

public class DefaultActions
{
  public static EditorAction newAction        = new NewAction();
  public static EditorAction openAction       = new OpenAction();
  public static EditorAction saveAction       = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	container.getEditor().save();
      }
    };
  public static EditorAction saveAsAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	container.getEditor().saveAs();
      }
    };
  public static EditorAction closeAction      = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	container.getEditor().close(true);
	System.gc();
      }
    };
  public static EditorAction printAction      = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	container.getEditor().print();
      }
    };
  public static EditorAction statisticsAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	new StatisticsDialog(container.getFrame());
      }
    };
  public static EditorAction dspAction        = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	 FtsDspControl control = JMaxApplication.getDspControl();
	 control.requestSetDspOn(!control.getDspOn());
      }
    };
  public static EditorAction quitAction       = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.Quit();
      }
    };

  /*********** project ***********************************/
  public static EditorAction newProjectAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.newProject( container.getFrame());
      }
    };
  public static EditorAction openProjectAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.openProject( container.getFrame());
      }
    };
  public static EditorAction editCurrentProjectAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getProject().requestOpenEditor();
      }
    };
  public static EditorAction saveAsDefaultProjectAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getProject().saveAsDefault();
      }
    };
  public static EditorAction newPackageAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.newPackage( container.getFrame());
      }
    };
  public static EditorAction openPackageAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.openPackage( container.getFrame());
      }
    };
  public static EditorAction newConfigurationAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ConfigurationEditor.newConfiguration();
      }
    };
  public static EditorAction openConfigurationAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ConfigurationEditor.open( container.getFrame());
      }
    };
  public static EditorAction editCurrentConfigurationAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getConfig().requestOpenEditor();
      }
    };
  public static EditorAction saveAsDefaultConfigurationAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	System.err.println( "Not yet implemented");
      }
    };
}












