package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class DeleteSelectionAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public DeleteSelectionAction( ErmesSketchWindow editor, int modifier, int key)
  {
    super("Delete Selection", "Delete Selection", modifier, key);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (! editor.isLocked())
      {
	ErmesSelection selection = ErmesSelection.patcherSelection;

	if (selection.ownedBy(editor.itsSketchPad))
	  if (! editor.itsSketchPad.isTextEditingObject())
	    selection.deleteAll();
      }
  }
}
