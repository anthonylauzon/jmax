package ircam.jmax.editors.patcher.interactions;

/** This class define a set of static variables 
 *  containing all the standard interactions used for the
 * patcher editor; please notes that interactions objects are
 * shared between all the editor instances.
 */

public class Interactions
{
  // High level dispatchers interactions

  public static Interaction runModeInteraction  = new RunModeInteraction();
  public static Interaction editModeInteraction = new EditModeInteraction();
  public static Interaction addModeInteraction  = new AddModeInteraction();

  // Mixed mode interactions

  public static Interaction helpInteraction    = new HelpInteraction();

  // Run Mode interactions 

  public static Interaction runCtrlInteraction = new RunCtrlInteraction();
  public static Interaction doubleClickEditInteraction = new DoubleClickEditInteraction();
 
  // Edit mode interactions

  public static Interaction dragSelectInteraction = new DragSelectInteraction();
  public static Interaction dragToggleSelectInteraction = new DragToggleSelectInteraction();
  public static Interaction editCtrlInteraction    = new EditCtrlInteraction();
  public static Interaction hResizeInteraction     = new HResizeInteraction();
  public static Interaction vResizeInteraction     = new VResizeInteraction();
  public static Interaction moveEditInteraction    = new MoveEditInteraction();
  public static Interaction moveInteraction        = new MoveInteraction();
  public static Interaction dragConnectInteraction = new DragConnectInteraction();
  public static Interaction dragReverseConnectInteraction = new DragReverseConnectInteraction();
  public static Interaction followInteraction      = new FollowInteraction();
  public static Interaction connectionSelectInteraction       = new ConnectionSelectInteraction();
  public static Interaction connectionToggleSelectInteraction = new ConnectionToggleSelectInteraction();
  public static Interaction popUpInteraction = new PopUpInteraction();
  public static Interaction backgroundPopUpInteraction = new BackgroundPopUpInteraction();
}


