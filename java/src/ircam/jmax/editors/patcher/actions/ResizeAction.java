package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ResizeAction extends PatcherAction
{
  ErmesSketchWindow editor;
  int dx;
  int dy;
  
  public ResizeAction( ErmesSketchWindow editor, int dx, int dy, int modifier, int key)
  {
    super("Resize", "Resize the selection", modifier, key);
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
	      object.resizeBy(dx, dy);
	      object.redraw();
	    }});

	editor.itsSketchPad.fixSize();
      }
  }
}
