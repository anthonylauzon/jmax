package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ResizeToMaxHeightAction extends AbstractAction
{
  public ResizeToMaxHeightAction()
  {
    super("Max Height");
  }

  public  void actionPerformed(ActionEvent e)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) e.getSource();

    if (ErmesSelection.patcherSelection.ownedBy(sketch))
      ErmesSelection.patcherSelection.resizeToMaxHeight();
  }
}
