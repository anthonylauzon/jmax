package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class AlignTopAction extends SubMenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    if (editor == null)
      System.err.println("NUll editor in Aligntop");

    if (ErmesSelection.patcherSelection == null)
      System.err.println("Huga huga !!!");

    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.alignTop();
  }
}
