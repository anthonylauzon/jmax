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

package ircam.jmax.editors.patcher.actions;

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
  // Keyboard actions

  public static EditorAction moveTenLeftAction  = new MoveAction(-10,   0);
  public static EditorAction moveTenRightAction = new MoveAction( 10,   0);
  public static EditorAction moveTenUpAction    = new MoveAction(  0, -10);
  public static EditorAction moveTenDownAction  = new MoveAction(  0,  10);

  public static EditorAction moveLeftAction  = new MoveAction(-1,  0);
  public static EditorAction moveRightAction = new MoveAction( 1,  0);
  public static EditorAction moveUpAction    = new MoveAction( 0, -1);
  public static EditorAction moveDownAction  = new MoveAction( 0,  1);

  public static EditorAction resizeTenLeftAction  = new ResizeAction(-10,   0);
  public static EditorAction resizeTenRightAction = new ResizeAction( 10,   0);
  public static EditorAction resizeTenUpAction    = new ResizeAction(  0, -10);
  public static EditorAction resizeTenDownAction  = new ResizeAction(  0,  10);

  public static EditorAction resizeLeftAction  = new ResizeAction(-1, 0);
  public static EditorAction resizeRightAction = new ResizeAction( 1, 0);
  public static EditorAction resizeUpAction    = new ResizeAction( 0,-1);
  public static EditorAction resizeDownAction  = new ResizeAction( 0, 1);

  public static EditorAction resizeToMaxWidthAction  = new ResizeToMaxWidthAction();
  public static EditorAction resizeToMaxHeightAction = new ResizeToMaxHeightAction();

  public static EditorAction deleteSelectionAction = new DeleteSelectionAction();
  public static EditorAction showErrorAction       = new ShowErrorAction();

  public static EditorAction alignTopAction    = new AlignTopAction();
  public static EditorAction alignLeftAction   = new AlignLeftAction();
  public static EditorAction alignBottomAction = new AlignBottomAction();
  public static EditorAction alignRightAction  = new AlignRightAction();

  public static EditorAction bringToFrontAction = new BringToFrontAction();
  public static EditorAction sendToBackAction  = new SendToBackAction();

  public static EditorAction cutAction       = new CutAction();
  public static EditorAction copyAction      = new CopyAction();
  public static EditorAction pasteAction     = new PasteAction();
  public static EditorAction duplicateAction = new DuplicateAction();
  public static EditorAction selectAllAction = new SelectAllAction();
  public static EditorAction findAction      = new FindAction();
  public static EditorAction findErrorsAction= new FindErrorsAction();
  public static EditorAction inspectAction   = new InspectAction();
  public static EditorAction lockAction      = new LockAction();

  public static EditorAction saveAction     = new SaveAction();
  public static EditorAction saveAsAction   = new SaveAsAction();
  public static EditorAction saveToAction   = new SaveToAction();
  public static EditorAction printAction    = new PrintAction();
}












