package ircam.jmax.editors.patcher.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;

public class OpenAction extends PatcherAction
{
  JFrame         frame;

  public OpenAction(JFrame frame)
  {
    super("Open", "Open a Patcher", Event.CTRL_MASK, KeyEvent.VK_O);
    this.frame = frame;
  }

  public  void actionPerformed(ActionEvent e)
  {
    File file = MaxFileChooser.chooseFileToOpen(frame);

    if (file != null)
      {
	Cursor temp = frame.getCursor();

	try
	  {
	    MaxDocument document;

	    frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	    document = Mda.loadDocument(file);
	
	    try
	      {
		if (document.getDocumentType().isEditable())
		  document.edit();
	      }
	    catch (MaxDocumentException ex)
	      {
		// Ignore MaxDocumentException exception in running the editor
		// May be an hack, may be is ok; move this stuff to an action
		// handler !!
	      }

	    frame.setCursor(temp);
	  }
	catch (MaxDocumentException exc)
	  {
	    frame.setCursor(temp);
	    new ErrorDialog(frame, e.toString());
	  }
      }
  }
}
