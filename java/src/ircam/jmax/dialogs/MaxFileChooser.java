package ircam.jmax.dialogs;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;
import com.sun.java.swing.preview.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * 
 * A File Dialog that provide the concept
 * of current directory, and a few more things, using the JFileChooser.
 * Use the *static* methods.
 * 
 */

public class MaxFileChooser {

  static private String currentOpenDirectory = null; // temp !!

  /** New Loading structure (beginning): global "Open" FileDialog that handle current directory */


  /** CHoose a file for opening, in the current directory, with a given f ilename filter */

  public static File chooseFileToOpen(Frame frame, String title)
  {
    JFileChooser fd;

    if (currentOpenDirectory == null)
      currentOpenDirectory = MaxApplication.getProperty("user.dir");

    fd = new JFileChooser(currentOpenDirectory);

    fd.setDialogTitle("Save"); 

    if (fd.showDialog(frame, "Open") == 0)
      {
	File file;

	file = fd.getSelectedFile();
	
	if (file != null)
	  currentOpenDirectory = file.getParent();

	return file;
      }
    else
      return null;
  }

  /* CHoose a file to save */

  public static File chooseFileToSave(Frame frame, String title)
  {
    return chooseFileToSave(frame, title, null);
  }

  /* CHoose a file to save, having an old File as initial content of the dialog box */

  public static File chooseFileToSave(Frame frame, String title, File oldFile)
  {
    JFileChooser fd;

    if (currentOpenDirectory == null)
      currentOpenDirectory = MaxApplication.getProperty("user.dir");

    fd = new JFileChooser(currentOpenDirectory);

    fd.setDialogTitle("Save"); 

    if (fd.showDialog(frame, "Save") == 0)
      {
	File file;
	
	file = fd.getSelectedFile();    

	if ((oldFile == null) && (file != null))
	  currentOpenDirectory = file.getParent();

	return file;
      }
    else
      return null;
  }
}


































