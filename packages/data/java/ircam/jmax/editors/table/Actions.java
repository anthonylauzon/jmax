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

package ircam.jmax.editors.table;

import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

/** This class define a set of static variables 
 *  containing all the standard actions used for the
 *  patcher editor; please notes that actions objects are
 *  shared between all the editor instances.
 */

public class Actions
{
  public static EditorAction copyAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Copy();
      }
    };
  public static EditorAction cutAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Cut();
      }
    };
  public static EditorAction pasteAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Paste();
      }
    };
  public static EditorAction insertAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Insert();
      }
    };
  public static EditorAction undoAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Undo();
      }
    };
  public static EditorAction redoAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Redo();
      }
    };
  public static EditorAction refreshAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).Refresh();
      }
    };
  public static EditorAction hollowAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).hollow();
      }
    };
  public static EditorAction solidAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).solid();
      }
    };
  public static EditorAction backColorAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).changeBackgroundColor();
      }
    };
  public static EditorAction foreColorAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((TablePanel)container.getEditor()).changeForegroundColor();
      }
    };
}
