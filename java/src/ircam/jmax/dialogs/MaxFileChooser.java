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
import javax.swing.border.*;
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

class RadioFormatAccessory extends JPanel {

  RadioFormatAccessory()
  {
    dotJmaxButton = new JRadioButton( ".jmax");
    dotPatButton = new JRadioButton( ".pat");

    ActionListener listener = new ActionListener() {
	public void actionPerformed( ActionEvent e)
	{
	  String text = ((JRadioButton)(e.getSource())).getText();

	  if ( text.equals( ".pat"))
	    MaxFileChooser.setSaveType( MaxFileChooser.SAVE_PAT_TYPE );
	  else
	    MaxFileChooser.setSaveType( MaxFileChooser.SAVE_JMAX_TYPE );
	}
      };

    dotJmaxButton.addActionListener( listener);
    dotPatButton.addActionListener( listener);

    ButtonGroup bg = new ButtonGroup();

    bg.add( dotJmaxButton);
    bg.add( dotPatButton);

    JPanel panel1 = new JPanel();

    TitledBorder titleBorder = new TitledBorder( LineBorder.createGrayLineBorder(), "Format");
    titleBorder.setTitleJustification( TitledBorder.LEFT);

    panel1.setBorder( titleBorder);

    panel1.setLayout( new GridLayout(0, 1));

    panel1.add( dotJmaxButton);
    panel1.add( dotPatButton);

    add( panel1);
  }

  void setSaveType( int saveType)
  {
    if (saveType == MaxFileChooser.SAVE_PAT_TYPE)
      dotPatButton.setSelected( true);
    else
      dotJmaxButton.setSelected( true);
  }

  private JRadioButton dotJmaxButton;
  private JRadioButton dotPatButton;
}

public class MaxFileChooser
{
  private static JFileChooser fd;
  private static RadioFormatAccessory formatAccessory;

  private static boolean configured = false;

  public static final int SAVE_JMAX_TYPE = 0;
  public static final int SAVE_PAT_TYPE = 1;
  private static int saveType = SAVE_JMAX_TYPE;

  static void makeFileChooser()
  {
    fd = new JFileChooser( MaxApplication.getProperty("user.dir"));
    formatAccessory = new RadioFormatAccessory();
  }

  public static int getSaveType()
  {
    return saveType; 
  }

  static void setSaveType( int type)
  {
    saveType = type;

    File currentFile = fd.getSelectedFile();

    String name = currentFile.getName();
    String suffix = "";

    if (saveType == SAVE_JMAX_TYPE)
      suffix = ".jmax";
    else if (saveType == SAVE_PAT_TYPE)
      suffix = ".pat";

    if ( name.endsWith( ".jmax") || name.endsWith( ".pat"))
      {
	int i = name.lastIndexOf( '.');
	    
	String newName = null;

	try
	  {
	    newName = name.substring( 0, i) + suffix;
	    fd.setSelectedFile( new File( newName));
	  }
	catch ( StringIndexOutOfBoundsException exc)
	  {
	  }
      }

    if (formatAccessory != null)
      formatAccessory.setSaveType( type);
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

    fd.setAccessory( null);

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

  public static File chooseFileToSave(Frame frame, File oldFile, String title, int type)
  {
    if (fd == null)
      makeFileChooser();

    setSaveType( type);

    fd.setAccessory( formatAccessory );

    return chooseFileToSave( frame, oldFile, title);
  }
}
