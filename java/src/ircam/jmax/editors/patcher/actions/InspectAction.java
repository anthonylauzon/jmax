package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class InspectAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public InspectAction( ErmesSketchWindow editor)
  {
    super("Inspect", "Inspect the selection");
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(ErmesObject object)
	    { object.inspect();}});
      }
  }
}
