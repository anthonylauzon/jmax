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
  public static EditorAction saveAction       = new EditorAction("Save...", "save", KeyEvent.VK_S, KeyEvent.VK_S, true){
      public void doAction(EditorContainer container)
      {
	container.getEditor().save();
      }
    };
  public static EditorAction saveAsAction     = new EditorAction("Save as...", "save as", KeyEvent.VK_A, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	container.getEditor().saveAs();
      }
    };
  public static EditorAction closeAction      = new EditorAction("Close", "close", KeyEvent.VK_C, KeyEvent.VK_W, true){
      public void doAction(EditorContainer container)
      {
	container.getEditor().close(true);
	System.gc();
      }
    };
  public static EditorAction printAction      = new EditorAction("Print...", "print", KeyEvent.VK_P, KeyEvent.VK_P, true){
      public void doAction(EditorContainer container)
      {
	container.getEditor().print();
      }
    };
  public static EditorAction statisticsAction = new EditorAction("System info...", "statistics", KeyEvent.VK_UNDEFINED, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	new StatisticsDialog(container.getFrame());
      }
    };
  public static EditorAction dspAction        = new EditorAction("Activate DSP", "dsp", KeyEvent.VK_D, KeyEvent.VK_ENTER, true){
      public void doAction(EditorContainer container)
      {
	 FtsDspControl control = JMaxApplication.getDspControl();
	 control.requestSetDspOn(!control.getDspOn());
	 if(!control.getDspOn())
		{
		   dspAction.putValue(NAME, "Deactivate DSP");
		}
	 else
		{
		   dspAction.putValue(NAME, "Activate DSP");
		}
      }
    };
  public static EditorAction quitAction       = new EditorAction("Quit", "quit", KeyEvent.VK_Q, KeyEvent.VK_Q, true){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.Quit();
      }
    };

  /*********** project ***********************************/
  public static EditorAction newProjectAction     = new EditorAction("New project", null, KeyEvent.VK_N, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.newProject( container.getFrame());
      }
    };
   public static EditorAction openProjectAction     = new EditorAction("Open project...", null, KeyEvent.VK_O, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.openProject( container.getFrame());
      }
    };
  public static EditorAction editCurrentProjectAction    = new EditorAction("Edit current project...", null, KeyEvent.VK_E, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getProject().requestOpenEditor();
      }
    };
  public static EditorAction saveAsDefaultProjectAction    = new EditorAction("Save as default project", null, KeyEvent.VK_A, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getProject().saveAsDefault();
      }
    };
  public static EditorAction newPackageAction     = new EditorAction("New package...", null, KeyEvent.VK_P, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.newPackage( container.getFrame());
      }
    };
  public static EditorAction openPackageAction     = new EditorAction("Open package...", null, KeyEvent.VK_C, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.openPackage( container.getFrame());
      }
    };
  public static EditorAction newConfigurationAction     = new EditorAction("New configuration...", null, KeyEvent.VK_F, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ConfigurationEditor.newConfiguration();
      }
    };
  public static EditorAction openConfigurationAction     = new EditorAction("Open configuration...", null, KeyEvent.VK_G, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	ConfigurationEditor.open( container.getFrame());
      }
    };
  public static EditorAction editCurrentConfigurationAction    = new EditorAction("Edit current configuration...", null, KeyEvent.VK_U, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	JMaxApplication.getConfig().requestOpenEditor();
      }
    };
  public static EditorAction saveAsDefaultConfigurationAction    = new EditorAction("Save as default configuration", null, KeyEvent.VK_L, KeyEvent.VK_UNDEFINED, true){
      public void doAction(EditorContainer container)
      {
	  JMaxApplication.getConfig().saveAsDefault();
      }
    };
}












