package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ResizeAction extends AbstractAction
{
  int dx;
  int dy;
  
  public ResizeAction(int dx, int dy)
  {
    super("Resize");

    this.dx = dx;
    this.dy = dy;
  }

  public  void actionPerformed(ActionEvent e)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) e.getSource();

    if (ErmesSelection.patcherSelection.ownedBy(sketch))
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(GraphicObject object)
	    {
	      object.redraw();
	      object.redrawConnections();
	      object.setWidth(object.getWidth() + dx);
	      object.setHeight(object.getHeight() + dy);
	      object.redraw();
	      object.redrawConnections();
	    }});

	sketch.fixSize();
      }
  }
}
