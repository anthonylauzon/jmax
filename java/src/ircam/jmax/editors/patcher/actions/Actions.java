package ircam.jmax.editors.patcher.actions;

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

  public static MenuAction alignTopAction    = new AlignTopAction();
  public static MenuAction alignLeftAction   = new AlignLeftAction();
  public static MenuAction alignBottomAction = new AlignBottomAction();
  public static MenuAction alignRightAction  = new AlignRightAction();

  public static MenuAction cutAction       = new CutAction();
  public static MenuAction copyAction      = new CopyAction();
  public static MenuAction pasteAction     = new PasteAction();
  public static MenuAction duplicateAction = new DuplicateAction();
  public static MenuAction selectAllAction = new SelectAllAction();
  public static MenuAction findAction      = new FindAction();
  public static MenuAction findErrorsAction= new FindErrorsAction();
  public static MenuAction inspectAction   = new InspectAction();
  public static MenuAction lockAction      = new LockAction();

  public static MenuAction newAction      = new NewAction();
  public static MenuAction openAction     = new OpenAction();
  public static MenuAction saveAction     = new SaveAction();
  public static MenuAction saveAsAction   = new SaveAsAction();
  public static MenuAction saveToAction   = new SaveToAction();
  public static MenuAction closeAction    = new CloseAction();
  public static MenuAction printAction    = new PrintAction();
  public static MenuAction statisticsAction = new StatisticsAction();
  public static MenuAction quitAction     = new QuitAction();
}



