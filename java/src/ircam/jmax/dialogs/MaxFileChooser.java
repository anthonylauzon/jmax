package ircam.jmax.dialogs;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;
import com.sun.java.swing.filechooser.*; // tmp !!

// Tmp packages
// import com.sun.java.swing.preview.*;
// import com.sun.java.swing.preview.filechooser.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * 
 * A File Dialog that provide the concept
 * of current directory, and a few more things, using the JFileChooser.
 * Use the *static* methods.
 * 
 */

public class MaxFileChooser
{

  static private JFileChooser fd;

  static void makeFileChooser()
  {
    fd = new JFileChooser(MaxApplication.getProperty("user.dir"));

    fd.setFileFilter(Mda.getAllDocumentsFileFilter());

    Enumeration e = Mda.getDocumentFileFilters();
    while (e.hasMoreElements())
      fd.addChoosableFileFilter((FileFilter) e.nextElement());

    fd.addChoosableFileFilter(fd.getAcceptAllFileFilter());
  }

  /** New Loading structure (beginning): global "Open" FileDialog that handle current directory */

  /** CHoose a file for opening, in the current directory, with a given f ilename filter */

  public static File chooseFileToOpen(Frame frame)
  {
    File dir;

    if (fd == null)
      makeFileChooser();

    dir = fd.getCurrentDirectory();
    fd.setDialogTitle("Open"); 

    if (fd.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION)
      return fd.getSelectedFile();
    else
      {
	/* We backup to the old directory in case of cancel;
	   Anyway, the directory will be in the history */

	fd.setCurrentDirectory(dir);

	return null;
      }
  }

  /* CHoose a file to save, having an old File as initial content of the dialog box */

  public static File chooseFileToSave(Frame frame, File oldFile)
  {
    File dir;

    if (fd == null)
      makeFileChooser();

    dir = fd.getCurrentDirectory();
    fd.setDialogTitle("Save As");

    if (oldFile != null)
      fd.setSelectedFile(oldFile);

    if (fd.showDialog(frame, "Save As") == JFileChooser.APPROVE_OPTION)
      return fd.getSelectedFile();
    else
      {
	/* We backup to the old directory in case of cancel;
	   Anyway, the directory will be in the history */

	fd.setCurrentDirectory(dir);

	return null;
      }
  }
}


































