package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class DeleteSelectionAction extends AbstractAction
{
  public DeleteSelectionAction()
  {
    super("Delete Selection");
  }

  public  void actionPerformed(ActionEvent e)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) e.getSource();

    if (! sketch.isLocked())
      {
	ErmesSelection selection = ErmesSelection.patcherSelection;

	if (selection.ownedBy(sketch))
	  if (! sketch.isTextEditingObject())
	    {
	      selection.redraw();
	      selection.deleteAll();
	    }
      }
  }
}
