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

package ircam.jmax.toolkit.actions;

import java.awt.event.*;
import javax.swing.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.project.*;

/** This class define a set of static variables 
 *  containing all the standard actions used for the
 *  patcher editor; please notes that actions objects are
 *  shared between all the editor instances.
 */

public class DefaultActions
{
  public static EditorAction newAction      = new NewAction();
  public static EditorAction openAction     = new OpenAction();
  public static EditorAction closeAction    = new CloseAction();
  public static EditorAction statisticsAction = new StatisticsAction();
  public static EditorAction dspAction     = new DSPAction();
  public static EditorAction quitAction     = new QuitAction();

  /*********** project ***********************************/
  public static EditorAction newProjectAction     = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	//ProjectEditor.new();
      }
    };
  public static EditorAction editProjectAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.open();
      }
    };
  public static EditorAction saveProjectAction    = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.getInstance().save();
      }
    };
  public static EditorAction saveAsProjectAction  = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ProjectEditor.getInstance().saveAs();
      }
    };
}












