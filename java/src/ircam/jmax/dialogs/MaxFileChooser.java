//
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.dialogs;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.filechooser.*; // tmp !!


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
  static private boolean configured = false;

  static void makeFileChooser()
  {
    fd = new JFileChooser(MaxApplication.getProperty("user.dir"));

  }

  private static void configure()
  {
    if (MaxApplication.getProperty("jmaxFastFileBox").equals("false"))
      {
	fd.setFileFilter(Mda.getAllDocumentsFileFilter());

	Enumeration e = Mda.getDocumentFileFilters();
	while (e.hasMoreElements())
	  fd.addChoosableFileFilter((FileFilter) e.nextElement());

	fd.addChoosableFileFilter(fd.getAcceptAllFileFilter());

	fd.setFileView(Mda.getFileView());
      }

    configured = true;
  }

  /** New Loading structure (beginning): global "Open" FileDialog that handle current directory */

  /** CHoose a file for opening, in the current directory, with a given f ilename filter */

  public static File chooseFileToOpen(Frame frame)
  {
    return chooseFileToOpen(frame, "Open");
  }

  public static File chooseFileToOpen(Frame frame, String title)
  {
    File dir;

    if (fd == null)
      makeFileChooser();

    if ( !configured)
      configure();

    dir = fd.getCurrentDirectory();
    fd.setDialogTitle(title); 
    fd.rescanCurrentDirectory();
    if (fd.showDialog(frame, title) == JFileChooser.APPROVE_OPTION)
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
    return chooseFileToSave(frame, oldFile, "Save");
  }

  public static File chooseFileToSave(Frame frame, File oldFile, String title)
  {
    File dir;

    if (fd == null)
      makeFileChooser();

    if ( !configured)
      configure();

    dir = fd.getCurrentDirectory();
    fd.setDialogTitle(title);
    fd.rescanCurrentDirectory();
    if (oldFile != null)
      fd.setSelectedFile(oldFile);

    if (fd.showDialog(frame, title) == JFileChooser.APPROVE_OPTION)
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


































