package ircam.jmax.editors.patcher.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;

public class NewAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    JFrame frame = editor;
    Cursor temp = frame.getCursor();

    try
      {
	frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	Fts.getPatcherDocumentType().newDocument(editor.getFts()).edit();
	frame.setCursor(temp);
      }
    catch (MaxDocumentException ex)
      {
	frame.setCursor(temp);
	new ErrorDialog(frame, ex.toString());
      }
  }
}
