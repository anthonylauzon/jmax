package ircam.jmax.dialogs;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.mda.*;
/**
 * 
 * A File Dialog that provide the concept
 * of current directory, and a few more things.
 * Use the *static* methods.
 * 
Substitute getOpenFileName <<<
 */

public class MaxFileChooser {

  static private String currentOpenDirectory = null; // temp !!

  /** New Loading structure (beginning): global "Open" FileDialog that handle current directory */

  /** CHoose a file for opening, in the current directory */

  public static MaxDataSource chooseFileToOpen(Frame frame, String title)
  {
    FileDialog fd = new FileDialog(frame, title);
    String file;

    fd.setFile("");

    if (currentOpenDirectory != null)
      fd.setDirectory(currentOpenDirectory);

    fd.setMode(FileDialog.LOAD);

    fd.show();

    currentOpenDirectory = fd.getDirectory();
    file = fd.getFile();

    if ((file == null) || file.equals(""))
      return null;
    else
      return MaxDataSource.makeDataSource(new File(currentOpenDirectory, file));
  }

  /* CHoose a file to save */

  public static MaxDataSource chooseFileToSave(Frame frame, String title)
  {
    return chooseFileToSave(frame, title, null);
  }

  /* CHoose a file to save, having an old File as initial content of the dialog box */

  public static MaxDataSource chooseFileToSave(Frame frame, String title, MaxDataSource source)
  {
    FileDialog fd = new FileDialog(frame, title);
    String file;
    String dir;
    String oldDir = null;
    File oldFile = null;

    if ((source != null) && (source instanceof MaxFileDataSource))
      oldFile = ((MaxFileDataSource) source).getFile();

    if (oldFile != null)
      {
	fd.setFile(oldFile.getName());
	oldDir = oldFile.getParent();
	fd.setDirectory(oldDir);
      }
    else
      {
	fd.setFile("");

	if (currentOpenDirectory != null)
	  {
	    oldDir = currentOpenDirectory;
	    fd.setDirectory(oldDir);
	  }
      }

    fd.setMode(FileDialog.SAVE);
    fd.show();

    file = fd.getFile();    
    dir  = fd.getDirectory();

    // Patch for the Motif file box  ???

    if(dir!=null){
      if (dir.equals(".") || dir.equals("./"))
	if (oldDir != null)
	  dir = oldDir;
    }
      
    if ((file == null) || file.equals(""))
      return null;

    if (oldFile == null)
      currentOpenDirectory = dir;

    return MaxDataSource.makeDataSource(new File(dir, file));
  }
}


































