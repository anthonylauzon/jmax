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

public class MaxFileChooser
{
  private static JFileChooser fd;
  private static JComboBox formatComboBox;

  private static boolean configured = false;

  public static final int SAVE_JMAX_TYPE = 0;
  public static final int SAVE_PAT_TYPE = 1;

  private static int saveType = SAVE_JMAX_TYPE;

  static void makeFileChooser()
  {
    fd = new JFileChooser( MaxApplication.getProperty("user.dir"));
  }

  public static int getSaveType()
  {
    return saveType; 
  }

  static void setSaveType( int type)
  {
    saveType = type;

    File currentFile = fd.getSelectedFile();

    String name;
    if(currentFile!=null)
	name = currentFile.getName();
    else
	name = "untitled.jmax";
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

    if (!configured)
      configure();

    if (getSaveType() == MaxFileChooser.SAVE_JMAX_TYPE)
      formatComboBox.setSelectedIndex( 0);
    else
      formatComboBox.setSelectedIndex( 1);
  }

  interface ComponentMatcher {
    public boolean match( Component c);
  }

  private static Component findInChilds( Component c, ComponentMatcher m)
  {
    if ( m.match( c))
      return c;

    if ( c instanceof Container)
      {
	for( int i = 0; i < ((Container)c).getComponentCount(); i++)
	  {
	    Component child = ((Container)c).getComponent( i);

	    Component r = findInChilds( child, m);

	    if ( r != null)
	      return r;
	  }
      }

    return null;
  }

  static class LabelMatcher implements ComponentMatcher {
    public boolean match( Component c)
      {
	if ( ! (c instanceof JLabel) )
	  return false;

	if ( ! ( ((JLabel)c).getLabelFor() instanceof JComboBox ) )
	  return false;

	JComboBox cb = (JComboBox) ((JLabel)c).getLabelFor();

	return cb.getItemAt( 0) instanceof FileFilter;
      }
  }

  private static void nickNackFileDialog()
  {
    ComponentMatcher fmatcher = new ComponentMatcher() {
	public boolean match( Component c)
	{
	  return c instanceof JComboBox && ((JComboBox)c).getItemAt(0) instanceof FileFilter;
	}
      };

    ComponentMatcher lmatcher = new LabelMatcher();

    formatComboBox = (JComboBox)findInChilds( fd, fmatcher);

    if ( formatComboBox == null)
      {
	System.err.println( "Cannot find file format combo box");
      }

    JLabel label = (JLabel)findInChilds( fd, lmatcher);

    if (label != null && label.getLabelFor().equals( formatComboBox))
      label.setText( "Format:");
    else
      System.err.println( "Cannot find label");

    formatComboBox.setModel( new DefaultComboBoxModel() );

    formatComboBox.addItem( new FileFilter() {
	public boolean accept( File f) { return true; }
	public String getDescription() { return ".jmax"; }
      });

    formatComboBox.addItem( new FileFilter() {
	public boolean accept( File f) { return true; }
	public String getDescription() { return ".pat"; }
      });

    formatComboBox.addActionListener( new ActionListener() {
	public void actionPerformed( ActionEvent e)
	{
	  if ( ((JComboBox)e.getSource()).getSelectedIndex() == 0)
	    setSaveType( SAVE_JMAX_TYPE);
	  else
	    setSaveType( SAVE_PAT_TYPE);
	}
      } );
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

    nickNackFileDialog();
    
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

    return chooseFileToSave( frame, oldFile, title);
  }
}
