package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class PasteAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public PasteAction( ErmesSketchWindow editor)
  {
    super("Paste", "Paste the clipboard", Event.CTRL_MASK, KeyEvent.VK_V);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Paste();
  }
}
