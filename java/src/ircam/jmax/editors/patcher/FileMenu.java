package ircam.jmax.editors.patcher;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

/** Implement the patcher editor File Menu */

class FileMenu extends JMenu
{
  ErmesSketchWindow editor;

  FileMenu(ErmesSketchWindow window)
  {
    super("File");

    JMenuItem item;

    editor = window;

    item = new JMenuItem("New",  KeyEvent.VK_N);
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     {
			       Cursor temp = getCursor();

			       try
				 {
				   setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
				   Fts.getPatcherDocumentType().newDocument().edit();
				   setCursor(temp);
				 }
			       catch (MaxDocumentException ex)
				 {
				   setCursor(temp);
				   new ErrorDialog(editor, ex.toString());
				 }
			     }});

    item = new JMenuItem("Open...",  KeyEvent.VK_O);
    add(item);

    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     { Open();}});

    addSeparator();

    item = new JMenuItem("Save", KeyEvent.VK_S);
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     { editor.Save();}});

    item = new JMenuItem("Save As ...");
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     { editor.SaveAs();}});

    item = new JMenuItem("Save To ...");
    add(item);
    item.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { editor.SaveTo();}});

    item = new JMenuItem("Close", KeyEvent.VK_W);
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     { editor.Close(true);}});

    addSeparator();

    item = new JMenuItem("Print...", KeyEvent.VK_P);
    add(item);
    item.addActionListener(new ActionListener()
			   { public  void actionPerformed(ActionEvent e)
			     { editor.Print();}});

    item = new JMenuItem("Statistics...");
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     {new StatisticsDialog(editor);}});

    item = new JMenuItem("Quit", KeyEvent.VK_Q);
    add(item);
    item.addActionListener(new ActionListener()
			   {public  void actionPerformed(ActionEvent e)
			     { MaxApplication.Quit();}});
  }

  // The Open is implemented here because really independent from the Menu

  public void Open()
  {
    File file = MaxFileChooser.chooseFileToOpen(editor);

    if (file != null)
      {
	Cursor temp = getCursor();

	try
	  {
	    MaxDocument document;

	    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	    document = Mda.loadDocument(file);
	
	    try
	      {
		if (document.getDocumentType().isEditable())
		  document.edit();
	      }
	    catch (MaxDocumentException e)
	      {
		// Ignore MaxDocumentException exception in running the editor
		// May be an hack, may be is ok; move this stuff to an action
		// handler !!
	      }

	    setCursor(temp);
	  }
	catch (MaxDocumentException e)
	  {
	    setCursor(temp);
	    new ErrorDialog(editor, e.toString());
	  }
      }
  }
}
