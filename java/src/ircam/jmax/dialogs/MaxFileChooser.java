//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
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
import ircam.jmax.utils.*;
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

  static final int SAVE_JMAX_TYPE = 0;
  static final int SAVE_PAT_TYPE = 1;
  static private int saveType = SAVE_JMAX_TYPE;

  static boolean wasSaving = false;

  static private Component filtersCombo;
  static private JLabel label;
  static private JComboBox saveTypeCombo;
  static private JTextField textField;
  
  static void makeFileChooser()
  {
    fd = new JFileChooser(MaxApplication.getProperty("user.dir"));
    
    //get filters comboBox
    filtersCombo = ((Container)((Container)fd.getComponent(5)).getComponent(3)).getComponent(3);

    // get label
    label = (JLabel)((Container)((Container)fd.getComponent(5)).getComponent(1)).getComponent(2);
    //get file textfield 
    textField = (JTextField)((Container)((Container)fd.getComponent(5)).getComponent(3)).getComponent(1);

    //create save types comboBox
    String[] types = {".jmax", ".pat"};
    saveTypeCombo = new JComboBox(types);
    saveTypeCombo.addActionListener(new ActionListener() {
	public void actionPerformed(ActionEvent e) {
	    JComboBox cb = (JComboBox)e.getSource();
	    setSaveType((String)cb.getSelectedItem());
	}
    });
  }

 public static int getSaveType()
 {
   return saveType; 
 }

 public static void setSaveType(String type)
 {
   if(type.equals(".pat"))
   {
     saveType = SAVE_PAT_TYPE;
     textField.setText("untitled.pat");
   }   
   else
   {
     saveType = SAVE_JMAX_TYPE;
     textField.setText("untitled.jmax");
   } 
 }

 /* Added the full class name to FileFilter because of clash with java.io.FileFilter in JDK 1.2 */
  private static void configure()
  {
      if (MaxApplication.getProperty("jmaxFastFileBox").equals("false"))
      {
	fd.setFileFilter(Mda.getAllDocumentsFileFilter());

	Enumeration e = Mda.getDocumentFileFilters();
	while (e.hasMoreElements())
	  fd.addChoosableFileFilter((javax.swing.filechooser.FileFilter) e.nextElement());

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

    reInit(true);

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

    reInit(false);

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

  static private void reInit(boolean open)
  {
    if(open)
    {
      if(wasSaving)
      {
	  ((Container)((Container)fd.getComponent(5)).getComponent(3)).remove(saveTypeCombo);
	  ((Container)((Container)fd.getComponent(5)).getComponent(3)).add(filtersCombo, 3);
	  label.setText("Files of type");
	  wasSaving = false;
      }
      textField.setText("");
    }
    else
    {
      if(!wasSaving)
      {
	  ((Container)((Container)fd.getComponent(5)).getComponent(3)).remove(filtersCombo);
	  ((Container)((Container)fd.getComponent(5)).getComponent(3)).add(saveTypeCombo, 3);
	  label.setText("Save as type");
	  wasSaving = true;
      }
      saveTypeCombo.setSelectedIndex(0);
      textField.setText("untitled.jmax");
    }
  }
}


































