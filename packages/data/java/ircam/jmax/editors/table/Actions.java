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
  public static class CopyAction extends EditorAction
  {
    public CopyAction()
    {
      super("Copy", null, KeyEvent.VK_C, KeyEvent.VK_C, false);
    }

    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Copy();
    }
  }

  public static class CutAction extends EditorAction
  {
    public CutAction()
    {
      super("Cut", null, KeyEvent.VK_C, KeyEvent.VK_X, false);
    }

    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Cut();
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
      ((TablePanel)container.getEditor()).Paste();
    }
  }
 
  public static class InsertAction extends EditorAction
  {
    public InsertAction()
    {
      super("Insert", null, KeyEvent.VK_I, KeyEvent.VK_I, false);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Insert();
    }
  }

  public static class UndoAction extends EditorAction
  {
    public UndoAction()
    {
      super("Undo", null, KeyEvent.VK_U, KeyEvent.VK_Z, false);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Undo();
    }
  }

  public static class RedoAction extends EditorAction
  {
    public RedoAction()
    {
      super("Redo", null, KeyEvent.VK_R, KeyEvent.VK_Y, false);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Redo();
    }
  }

  public static class RefreshAction extends EditorAction
  {
    public RefreshAction()
    {
      super("Refresh", null, KeyEvent.VK_F, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).Refresh();
    }
  }

  public static class PointsViewAction extends EditorAction
  {
    public PointsViewAction()
    {
      super("Points", null, KeyEvent.VK_P, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).setPointsView();
    }
  }

  public static class FilledViewAction extends EditorAction
  {
    public FilledViewAction()
    {
      super("Filled", null, KeyEvent.VK_F, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).setFilledView();
    }
  }  

  public static class LinesViewAction extends EditorAction
  {
    public LinesViewAction()
    {
      super("Lines", null, KeyEvent.VK_L, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).setLinesView();
    }
  }

  public static class BackColorAction extends EditorAction
  {
    public BackColorAction()
    {
      super("Background Color", null, KeyEvent.VK_B, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).changeBackgroundColor();
    }
  }
  
  public static class ForeColorAction extends EditorAction
  {
    public ForeColorAction()
    {
      super("Foreground Color", null, KeyEvent.VK_C, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).changeForegroundColor();
    }
  }

  public static class ZoomToWindowAction extends EditorAction
  {
    public ZoomToWindowAction()
    {
      super("Zoom To Window", null, KeyEvent.VK_Z, KeyEvent.VK_UNDEFINED, true);
    }
    public void doAction(EditorContainer container)
    {
      ((TablePanel)container.getEditor()).getGraphicContext().getAdapter().zoomToWindow();
    }
  }
}
