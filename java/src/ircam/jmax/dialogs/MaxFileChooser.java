package ircam.jmax.dialogs;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import com.sun.java.swing.*;
import com.sun.java.swing.preview.*;
import com.sun.java.swing.preview.filechooser.*;

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
  static class MaxFileFilter extends FileFilter
  {
    public boolean accept(File f)
    {
      if (f.isDirectory()) 
	return true;
      else
	return Mda.canLoadDocument(f);
    }
    
    public String getDescription()
    {
	return "jMax documents";
    }
  }


  static private String currentOpenDirectory = null; 

  /** New Loading structure (beginning): global "Open" FileDialog that handle current directory */

  /** CHoose a file for opening, in the current directory, with a given f ilename filter */

  public static File chooseFileToOpen(Frame frame, String title)
  {
    JFileChooser fd;

    if (currentOpenDirectory == null)
      currentOpenDirectory = MaxApplication.getProperty("user.dir");

    fd = new JFileChooser(currentOpenDirectory);

    fd.setDialogTitle("Open"); 
    fd.setFileFilter(new MaxFileFilter());
    fd.addChoosableFileFilter(fd.getAcceptAllFileFilter());

    // fd.setFileView(new MyFileView());


    if (fd.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION)
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
    fd.setFileFilter(new MaxFileFilter());
    fd.addChoosableFileFilter(fd.getAcceptAllFileFilter());

    if (fd.showSaveDialog(frame) == JFileChooser.APPROVE_OPTION)
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


































