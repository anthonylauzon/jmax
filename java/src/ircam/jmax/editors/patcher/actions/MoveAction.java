package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class MoveAction extends AbstractAction
{
  int dx, dy;

  public MoveAction(int dx, int dy)
  {
    super("Move");
    this.dx = dx;
    this.dy = dy;
  }

  public  void actionPerformed(ActionEvent e)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) e.getSource();

    if (ErmesSelection.patcherSelection.ownedBy(sketch))
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(ErmesObject object)
	    {
	      object.redraw();
	      object.moveBy(dx, dy);
	      object.redraw();
	    }});

	sketch.fixSize();
      }
  }
}
