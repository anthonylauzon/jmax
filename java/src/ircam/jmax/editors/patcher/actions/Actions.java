//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.patcher.actions;

import java.awt.event.*;
import javax.swing.*;


/** This class define a set of static variables 
 *  containing all the standard actions used for the
 * patcher editor; please notes that actions objects are
 * shared between all the editor instances.
 */

public class Actions
{
  // Keyboard actions

  public static Action moveTenLeftAction  = new MoveAction(-10,   0);
  public static Action moveTenRightAction = new MoveAction( 10,   0);
  public static Action moveTenUpAction    = new MoveAction(  0, -10);
  public static Action moveTenDownAction  = new MoveAction(  0,  10);

  public static Action moveLeftAction  = new MoveAction(-1,  0);
  public static Action moveRightAction = new MoveAction( 1,  0);
  public static Action moveUpAction    = new MoveAction( 0, -1);
  public static Action moveDownAction  = new MoveAction( 0,  1);

  public static Action resizeTenLeftAction  = new ResizeAction(-10,   0);
  public static Action resizeTenRightAction = new ResizeAction( 10,   0);
  public static Action resizeTenUpAction    = new ResizeAction(  0, -10);
  public static Action resizeTenDownAction  = new ResizeAction(  0,  10);

  public static Action resizeLeftAction  = new ResizeAction(-1, 0);
  public static Action resizeRightAction = new ResizeAction( 1, 0);
  public static Action resizeUpAction    = new ResizeAction( 0,-1);
  public static Action resizeDownAction  = new ResizeAction( 0, 1);

  public static Action resizeToMaxWidthAction  = new ResizeToMaxWidthAction();
  public static Action resizeToMaxHeightAction = new ResizeToMaxHeightAction();

  public static Action deleteSelectionAction = new DeleteSelectionAction();
  public static Action showErrorAction       = new ShowErrorAction();

  // Menu actions; they *cannot* be interchanged with the Keyboard actions,
  // at least for now; the big problem is how to get the editor from 
  // the actionPerformed method; the two kind of actions use different methods.
  // Also, currently the Menu actions define their shortcuts .. together with the name.

  public static ActionListener alignTopAction    = new AlignTopAction();
  public static ActionListener alignLeftAction   = new AlignLeftAction();
  public static ActionListener alignBottomAction = new AlignBottomAction();
  public static ActionListener alignRightAction  = new AlignRightAction();

  public static ActionListener cutAction       = new CutAction();
  public static ActionListener copyAction      = new CopyAction();
  public static ActionListener pasteAction     = new PasteAction();
  public static ActionListener duplicateAction = new DuplicateAction();
  public static ActionListener selectAllAction = new SelectAllAction();
  public static ActionListener findAction      = new FindAction();
  public static ActionListener findErrorsAction= new FindErrorsAction();
  public static ActionListener inspectAction   = new InspectAction();
  public static ActionListener lockAction      = new LockAction();

  public static ActionListener newAction      = new NewAction();
  public static ActionListener openAction     = new OpenAction();
  public static ActionListener saveAction     = new SaveAction();
  public static ActionListener saveAsAction   = new SaveAsAction();
  public static ActionListener saveToAction   = new SaveToAction();
  public static ActionListener closeAction    = new CloseAction();
  public static ActionListener printAction    = new PrintAction();
  public static ActionListener statisticsAction = new StatisticsAction();
  public static ActionListener quitAction     = new QuitAction();
}



