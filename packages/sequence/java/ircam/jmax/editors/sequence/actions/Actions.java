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

package ircam.jmax.editors.sequence.actions;

import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

/** This class define a set of static variables 
 *  containing all the standard actions used for the
 *  sequence editor; please notes that actions objects are
 *  shared between all the editor instances.
 */

public class Actions
{
  public static EditorAction exportAction    = new ExportAction();
  public static EditorAction importAction    = new ImportAction();

  public static EditorAction cutAction       = new CutAction();
  public static EditorAction copyAction      = new CopyAction();
  public static EditorAction pasteAction     = new PasteAction();
  public static EditorAction duplicateAction = new DuplicateAction();

  public static EditorAction undoAction      = new UndoAction();
  public static EditorAction redoAction      = new RedoAction();

  public static EditorAction removeTrackAction  = new RemoveTrackAction();

    //public static EditorAction settingsAction  = new SettingsAction();
  //******** Merge reintroduction *******************//
  public static EditorAction mergeAction     = new MergeAction();

  public static EditorAction moveMidiTrackToAction = new MoveMidiTrackToAction();
  public static EditorAction moveMonodimensionalTrackToAction = new MoveMonoTrackToAction();
  public static EditorAction moveClosedTrackToAction = new MoveClosedTrackToAction();
}












