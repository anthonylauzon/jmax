package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ShowErrorAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public ShowErrorAction( ErmesSketchWindow editor)
  {
    super("ShowError", "Show the Error explications", Event.CTRL_MASK, KeyEvent.VK_Z);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (! editor.itsSketchPad.isAnnotating())
      {
	editor.itsSketchPad.setAnnotating();

	if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad) &&
	    ErmesSelection.patcherSelection.hasObjects())
	  {
	    ErmesSelection.patcherSelection.apply(new ObjectAction() {
	      public void processObject(ErmesObject object)
		{ object.showErrorDescription();}});
	  }
	else
	  editor.itsSketchPad.getDisplayList().applyToObjects(new ObjectAction() {
	    public void processObject(ErmesObject object)
	      { object.showErrorDescription();}});
      }
  }
}
