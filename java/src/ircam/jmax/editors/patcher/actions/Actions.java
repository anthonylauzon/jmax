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

package ircam.jmax.editors.patcher.actions;

import java.awt.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
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

  public static EditorAction moveTenLeftAction  = new MoveAction(-10,   0, "move ten left");
  public static EditorAction moveTenRightAction = new MoveAction( 10,   0, "move ten right");
  public static EditorAction moveTenUpAction    = new MoveAction(  0, -10, "move ten up");
  public static EditorAction moveTenDownAction  = new MoveAction(  0,  10, "move ten down");

  public static EditorAction moveLeftAction  = new MoveAction(-1,  0, "move left");
  public static EditorAction moveRightAction = new MoveAction( 1,  0, "move right");
  public static EditorAction moveUpAction    = new MoveAction( 0, -1, "move up");
  public static EditorAction moveDownAction  = new MoveAction( 0,  1, "move down");

  public static EditorAction resizeTenLeftAction  = new ResizeAction(-10,   0, "resize ten left");
  public static EditorAction resizeTenRightAction = new ResizeAction( 10,   0, "resize ten right");
  public static EditorAction resizeTenUpAction    = new ResizeAction(  0, -10, "resize ten up");
  public static EditorAction resizeTenDownAction  = new ResizeAction(  0,  10, "resize ten down");

  public static EditorAction resizeLeftAction  = new ResizeAction(-1, 0, "resize left");
  public static EditorAction resizeRightAction = new ResizeAction( 1, 0, "resize right");
  public static EditorAction resizeUpAction    = new ResizeAction( 0,-1, "resize up");
  public static EditorAction resizeDownAction  = new ResizeAction( 0, 1, "resize down");

  public static EditorAction resizeToMaxWidthAction  = new ResizeToMaxWidthAction();
  public static EditorAction resizeToMaxHeightAction = new ResizeToMaxHeightAction();
  public static EditorAction resizeToMinWidthAction  = new ResizeToMinWidthAction();
  public static EditorAction resizeToMinHeightAction = new ResizeToMinHeightAction();

  public static EditorAction deleteSelectionAction = new DeleteSelectionAction();
  public static EditorAction showErrorAction       = new ShowErrorAction();

  public static EditorAction alignTopAction    = new AlignTopAction();
  public static EditorAction alignLeftAction   = new AlignLeftAction();
  public static EditorAction alignBottomAction = new AlignBottomAction();
  public static EditorAction alignRightAction  = new AlignRightAction();

  public static EditorAction bringToFrontAction = new BringToFrontAction();
  public static EditorAction sendToBackAction  = new SendToBackAction();

  public static EditorAction fontSmallerAction     = new FontSmallerAction();
  public static EditorAction fontBiggerAction      = new FontBiggerAction();
  public static EditorAction fontSizesAction       = new FontSizesAction();
  public static EditorAction jmaxFontSizesAction   = new JMaxFontSizesAction();
  public static EditorAction fontStylesAction      = new FontStylesAction();
  public static EditorAction fontAction            = new FontAction();
  public static EditorAction jmaxFontAction        = new JMaxFontAction();
  public static EditorAction setAutomaticFitAction = new SetAutomaticFitAction();
  public static EditorAction fitToTextAction       = new FitToTextAction();
  //public static EditorAction fontBaseSizesAction   = new FontBaseSizesAction();

  public static EditorAction cutAction       = new CutAction();
  public static EditorAction copyAction      = new CopyAction();
  public static EditorAction pasteAction     = new PasteAction();
  public static EditorAction duplicateAction = new DuplicateAction();
  public static EditorAction selectAllAction = new SelectAllAction();
  public static EditorAction findAction      = new FindAction();
  public static EditorAction inspectAction   = new InspectAction();
  public static EditorAction lockAction      = new LockAction();

  public static EditorAction saveToAction   = new SaveToAction();
  public static EditorAction saveTemplateAction = new SaveToAction();

  public static EditorAction scalePatcherAction = new ScalePatcherAction();

  public static EditorAction openHelpPatchAction = new OpenHelpPatchAction();
  public static EditorAction showHtmlAction = new ShowHTMLAction();

  //Object popup actions
  public static EditorAction bringToFrontObjectAction = new BringToFrontObjectAction();
  public static EditorAction sendToBackObjectAction = new SendToBackObjectAction();
  public static EditorAction removeConnectionsAction = new RemoveInOutletConnection();
  public static EditorAction fitToTextPopUpAction = new FitToTextPopUpAction();
  public static EditorAction fontBiggerPopUpAction = new FontBiggerPopUpAction();
  public static EditorAction fontSmallerPopUpAction = new FontSmallerPopUpAction();
  public static EditorAction fontSizesPopUpAction = new FontSizesPopUpAction();
  public static EditorAction jmaxFontSizesPopUpAction = new JMaxFontSizesPopUpAction();
  public static EditorAction fontStylesPopUpAction = new FontStylesPopUpAction();
  public static EditorAction fontPopUpAction = new FontPopUpAction();
  public static EditorAction jmaxFontPopUpAction = new JMaxFontPopUpAction();
  public static EditorAction inspectObjectAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	ObjectPopUp.getPopUpTarget().inspect();
      }
    };
  public static EditorAction undoAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((ErmesSketchPad)container.getEditor()).undo();
      }
    };
  public static EditorAction redoAction = new EditorAction(){
      public void doAction(EditorContainer container)
      {
	((ErmesSketchPad)container.getEditor()).redo();
      }
    };
}













