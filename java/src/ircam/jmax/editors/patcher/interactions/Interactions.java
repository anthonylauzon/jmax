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
  public static Interaction doubleClickEdit = new DoubleClickEdit();
 
  // Edit mode interactions

  public static Interaction dragSelectInteraction = new DragSelectInteraction();
  public static Interaction dragToggleSelect = new DragToggleSelect();
  public static Interaction editCtrlInteraction    = new EditCtrlInteraction();
  public static Interaction hResizeInteraction     = new HResizeInteraction();
  public static Interaction vResizeInteraction     = new VResizeInteraction();
  public static Interaction moveEditInteraction    = new MoveEditInteraction();
  public static Interaction moveInteraction        = new MoveInteraction();
  public static Interaction dragConnectInteraction = new DragConnectInteraction();
  public static Interaction dragReverseConnect = new DragReverseConnect();
  public static Interaction moveConnectInteraction = new MoveConnectInteraction();
  public static Interaction moveReverseConnect = new MoveReverseConnect();
  public static Interaction followInteraction      = new FollowInteraction();
  public static Interaction connSelectInteraction       = new ConnSelectInteraction();
  public static Interaction connToggleSelectInteraction = new ConnToggleSelect();
  public static Interaction popUpInteraction = new PopUpInteraction();
  public static Interaction runPopUpInteraction = new RunPopUpInteraction();
  public static Interaction bgPopUpInteraction = new BgPopUpInteraction();
}



