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

package ircam.jmax.editors.sequence.actions;

import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;

/** This class define a set of static variables 
 *  containing all the standard actions used for the
 *  sequence editor; please notes that actions objects are
 *  shared between all the editor instances.
 */

public class Actions
{
  public static EditorAction exportAction    = new ExportAction();
  public static EditorAction importAction    = new ImportAction();

  public static class CutAction extends EditorAction
  {
	 public CutAction()
		{
		   super("Cut", null, KeyEvent.VK_C, KeyEvent.VK_X, false);
		}

      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).cut();
      }
  }

   public static class CopyAction extends EditorAction
   {
	  public CopyAction()
		 {
			super("Copy", null, KeyEvent.VK_C, KeyEvent.VK_C, false);
		 }

      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).copy();
      }
   }

   public static class PasteAction extends EditorAction
   {
	  public PasteAction()
		 {
			super("Paste", null, KeyEvent.VK_P, KeyEvent.VK_V, false);
		 }
      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).paste();
      }

   }

   public static class DuplicateAction extends EditorAction
   {
	  public DuplicateAction()
		 {
			super("Duplicate", null, KeyEvent.VK_D, KeyEvent.VK_D, false);
		 }
      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).duplicate();
      }
   }

   public static class UndoAction extends EditorAction
   {
	  public UndoAction()
		 {
			super("Undo", null, KeyEvent.VK_U, KeyEvent.VK_Z, true);
		 }
      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).undo();
      }
   }

   public static class RedoAction extends EditorAction
   {
	  public RedoAction()
		 {
			super("Redo", null, KeyEvent.VK_R, KeyEvent.VK_Y, true);
		 }
      public void doAction(EditorContainer container)
      {
	((SequenceEditor)container.getEditor()).redo();
      }
   }

  public static EditorAction removeTrackAction  = new RemoveTrackAction();

  //******** Merge reintroduction *******************//
  public static EditorAction mergeAction     = new MergeAction();

  public static EditorAction moveMidiTrackToAction = new MoveMidiTrackToAction();
  public static EditorAction moveMonodimensionalTrackToAction = new MoveMonoTrackToAction();
  public static EditorAction moveClosedTrackToAction = new MoveClosedTrackToAction();
  public static EditorAction labelTypesAction = new LabelTypesAction();
}












