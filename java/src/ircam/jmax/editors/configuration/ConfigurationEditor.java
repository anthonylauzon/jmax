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

package ircam.jmax.editors.configuration;

import javax.swing.*;
import javax.swing.table.*;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

public class ConfigurationEditor extends JFrame implements EditorContainer
{    
  public ConfigurationEditor( FtsConfig config)
  {
    super( "Configuration Editor");

    singleInstance = this;

    this.config = config;

    getContentPane().setLayout(new BoxLayout( getContentPane(), BoxLayout.Y_AXIS));    

    makeMenuBar();    
    
    midiPanel  = new MidiConfigPanel( this, config.getMidiConfig());
    audioPanel = new AudioConfigPanel( this, config.getAudioConfig());

    tabbedPane = new JTabbedPane();
    tabbedPane.setBorder( BorderFactory.createEtchedBorder());
    tabbedPane.addTab("MIDI", midiPanel);
    tabbedPane.addTab("Audio", audioPanel);
    tabbedPane.setSelectedIndex(0);
    
    /************ Buttons **********************************/
    JButton addButton = new JButton("Add");
    addButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  add();
	}
      });
    JButton deleteButton = new JButton("Delete");
    deleteButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  delete();
	}
      });
    
    JPanel buttons = new JPanel();
    buttons.setLayout(new BoxLayout( buttons, BoxLayout.X_AXIS));    
    buttons.add( addButton);
    buttons.add( deleteButton);
    buttons.add( Box.createHorizontalGlue());

    getContentPane().add( tabbedPane);
    getContentPane().add( buttons);

    validate();
    pack();

    if( JMaxApplication.getConfig() == null)
      {
	tabbedPane.setEnabledAt( 0, false);
	tabbedPane.setEnabledAt( 1, false);
	addButton.setEnabled( false);
	deleteButton.setEnabled( false);
      }

    setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    addWindowListener(new WindowAdapter(){
	public void windowClosing(WindowEvent e)
	{
	  if( tabbedPane.getSelectedIndex() == tabbedPane.indexOfTab( "MIDI"))
	    midiPanel.close(false);    
	  else
	    audioPanel.close(false);    
	}
      });
  }

  private void makeMenuBar()
  {
    JMenuBar mb = new JMenuBar();

    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 7);
    mb.add( fileMenu); 
    mb.add( new DefaultProjectMenu( this)); 
    mb.add( new MaxWindowJMenu( "Windows", this));
    mb.add(Box.createHorizontalGlue());
    mb.add( new DefaultHelpMenu());
    
    setJMenuBar( mb);
  }

  void add()
  {
    Component selected = tabbedPane.getSelectedComponent();
    if( selected == midiPanel)
      midiPanel.add();
    else
      audioPanel.add();
  }

  void delete()
  {
    Component selected = tabbedPane.getSelectedComponent();
    
    if( selected == midiPanel)
      midiPanel.delete();
    else
      audioPanel.delete();
  }

  public static void newConfiguration()
  {
    try
      {
	if (singleInstance != null)
	  singleInstance.close();

	JMaxApplication.getConfig().send( FtsSymbol.get( "close"));

	new FtsConfig().requestOpenEditor();
      }
    catch ( IOException e)
      {
	JMaxApplication.reportException( e);
      }

  }

  public static void open( Frame frame)
  {
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int result = fileChooser.showOpenDialog( frame);

    if ( result == JFileChooser.APPROVE_OPTION)
      {
	File f = fileChooser.getSelectedFile();

	if ( f != null)
	  {
	    try
	      {
		FtsArgs args = new FtsArgs();
		if (singleInstance != null)
		  singleInstance.close();

		JMaxApplication.getConfig().send( FtsSymbol.get( "close"));

		// send message to fts to open the configuration file
		// ...
		args.addString(f.getAbsolutePath());
		JMaxApplication.getConfig().send(FtsSymbol.get("load"), args);		
	      }
	    catch ( IOException e)
	      {
		JMaxApplication.reportException( e);
	      }
	  }
      }
  }

  public void save()
  {
    String fileName = JMaxApplication.getConfig().getFileName();
    if( fileName != null)
      JMaxApplication.getConfig().save( fileName);
    else
      saveAs();
  }

  public void saveAs()
  {
    String dir = JMaxApplication.getProject().getDir();
    String name = dir+"/"+JMaxApplication.getProject().getName()+".jcfg";    
    
    fileChooser.setSelectedFile( new File( name));
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int result = fileChooser.showSaveDialog( this);

    if ( result == JFileChooser.APPROVE_OPTION)
      {
	String fileName = fileChooser.getSelectedFile().getAbsolutePath();		  		
	if( fileName != null)
	  JMaxApplication.getConfig().save( fileName);
      }
  }

  public void close()
  {
    if( config.isDirty())
      {
	String message = "Configuration File is not saved.\nDo you want to save it now?";
	String title =  "Config Not Saved";
	Object[] options = { "Save", "Don't save", "Cancel" };
	int result = JOptionPane.showOptionDialog( this, message, title, 
						   JOptionPane.YES_NO_CANCEL_OPTION,
						   JOptionPane.QUESTION_MESSAGE,
						   null, options, options[0]);
	
	if( result == JOptionPane.CANCEL_OPTION)
	  return;
	
	if( result == JOptionPane.YES_OPTION)
	  save();	
      }

    setVisible(false);

    singleInstance = null;
  }

  /************* interface EditorContainer ************************/
  public Editor getEditor()
  {
    return (Editor)tabbedPane.getSelectedComponent();
  }
  public Frame getFrame()
  {
    return this;
  }
  public Point getContainerLocation()
  {
    return getLocation();
  }
  public Rectangle getViewRectangle()
  {
    return getContentPane().getBounds();
  }
  /**************************************************************/

  private MidiConfigPanel midiPanel;
  private AudioConfigPanel audioPanel;
  private JTabbedPane tabbedPane;
  static Font tableFont = (Font)UIManager.get("Table.font");
  private static JFileChooser fileChooser = new JFileChooser(); 

  private FtsConfig config;

  private static ConfigurationEditor singleInstance;
}

