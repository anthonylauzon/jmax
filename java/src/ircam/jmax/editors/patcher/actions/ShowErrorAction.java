package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ShowErrorAction extends AbstractAction
{
  public ShowErrorAction()
  {
    super("ShowError");
  }

  public  void actionPerformed(ActionEvent e)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) e.getSource();

    if (! sketch.isAnnotating())
      {
	sketch.setAnnotating();

	if (ErmesSelection.patcherSelection.ownedBy(sketch) &&
	    ErmesSelection.patcherSelection.hasObjects())
	  {
	    ErmesSelection.patcherSelection.apply(new ObjectAction() {
	      public void processObject(ErmesObject object)
		{ object.showErrorDescription();}});
	  }
	else
	  sketch.getDisplayList().applyToObjects(new ObjectAction() {
	    public void processObject(ErmesObject object)
	      { object.showErrorDescription();}});
      }
  }
}

