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
  public static void registerConfigEditor()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
	public String getToolName() { return "Configuration";}
	public void open() { ConfigurationEditor.open();}
      });
  }

  public static ConfigurationEditor open()
  {    
    if (configEditor == null)
      configEditor = new ConfigurationEditor();
    else
      configEditor.update();
    
    configEditor.setVisible(true);
    
    return configEditor;
  }

  public static ConfigurationEditor getInstance()
  {
    return configEditor;
  }

  protected ConfigurationEditor()
  {
    super( "Configuration Editor");
    getContentPane().setLayout(new BoxLayout( getContentPane(), BoxLayout.Y_AXIS));    

    makeMenuBar();    
    
    midiPanel  = new MidiConfigPanel( this, JMaxApplication.getConfig().getMidiConfig());
    audioPanel = new AudioConfigPanel( this, JMaxApplication.getConfig().getAudioConfig());

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
	  Add();
	}
      });
    JButton deleteButton = new JButton("Delete");
    deleteButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  Delete();
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
    mb.add( new DefaultProjectMenu()); 
    mb.add( new MaxWindowJMenu( "Windows", this));
    mb.add(Box.createHorizontalGlue());
    mb.add( new DefaultHelpMenu());
    
    setJMenuBar( mb);
  }

  void update()
  {
    midiPanel.update( JMaxApplication.getConfig().getMidiConfig());
    audioPanel.update( JMaxApplication.getConfig().getAudioConfig());
  } 

  void Add()
  {
    Component selected = tabbedPane.getSelectedComponent();
    if( selected == midiPanel)
      midiPanel.Add();
    else
      audioPanel.Add();
  }

  void Delete()
  {
    Component selected = tabbedPane.getSelectedComponent();
    
    if( selected == midiPanel)
      midiPanel.Delete();
    else
      audioPanel.Delete();
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
  private static ConfigurationEditor configEditor = null;
  private JTabbedPane tabbedPane;
  static Font tableFont = (Font)UIManager.get("Table.font");
  private JFileChooser fileChooser = new JFileChooser(); 
}

