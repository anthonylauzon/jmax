package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class MoveAction extends PatcherAction
{
  ErmesSketchWindow editor;
  int dx, dy;

  public MoveAction( ErmesSketchWindow editor, int dx, int dy, int modifier, int key)
  {
    super("Move", "Move the selection", modifier, key);
    this.editor = editor;
    this.dx = dx;
    this.dy = dy;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(ErmesObject object)
	    {
	      object.redraw();
	      object.moveBy(dx, dy);
	      object.redraw();
	    }});

	editor.itsSketchPad.fixSize();
      }
  }
}
