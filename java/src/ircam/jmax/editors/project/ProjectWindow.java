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

package ircam.jmax.editors.project; 

import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;  
import javax.swing.*;
import javax.swing.tree.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.project.menus.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.script.pkg.*;

import ircam.jmax.script.ScriptMenu;

public class ProjectWindow extends JFrame implements EditorContainer, ItemListener
{
    final static String FILES = "Project files";
    final static String PROJECT = "Project settings";
    final static String AUDIO = "Audio settings";
    final static String SERVER = "Server settings";
    final static String PATHS = "Access paths";
    final static String PACKAGES = "Required packages";
    final static String OTHER = "Other settings";

    protected JPanel itsSettingsPanel;
    protected JLabel itsMessageLabel;
    protected FilePanel itsFilePanel;
    protected ProjectPanel itsProjectPanel;
    protected AudioPanel itsAudioPanel;
    protected ServerPanel itsServerPanel;
    protected PathPanel itsPathPanel;
    protected PackagePanel itsPackagePanel;
    protected OtherPanel itsOtherPanel;

    protected ProjectEditor editor;

    public Editor getEditor()
    {
	return editor;
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
	return itsSettingsPanel.getVisibleRect();
    }

    public ProjectWindow(ProjectEditor editor) 
    {
	super(editor.getProject().getName());
	MaxWindowManager.getWindowManager().addWindow(this);
	this.editor = editor;

	JPanel mainPanel = new JPanel();
	mainPanel.setLayout(new BorderLayout());

	String settings[] = { FILES, PROJECT, AUDIO, SERVER, PATHS, PACKAGES, OTHER };
	JComboBox c = new JComboBox(settings);
	c.setEditable(false);
	c.addItemListener(this);

	//Put the JComboBox in a JPanel to get a nicer look. (Java tutorial)
	JPanel cbp = new JPanel();
	cbp.add(c);
	mainPanel.add(cbp, BorderLayout.NORTH);

	itsSettingsPanel = new JPanel();
	itsSettingsPanel.setLayout(new CardLayout());

	itsFilePanel = new FilePanel(editor);
	itsProjectPanel = new ProjectPanel(editor);
	itsAudioPanel = new AudioPanel(editor);
	itsServerPanel = new ServerPanel(editor);
	itsPathPanel = new PathPanel(editor);
	itsPackagePanel = new PackagePanel(editor);
	itsOtherPanel = new OtherPanel(editor);

	itsSettingsPanel.add(itsFilePanel, FILES);
	itsSettingsPanel.add(itsProjectPanel, PROJECT);
	itsSettingsPanel.add(itsAudioPanel, AUDIO);
	itsSettingsPanel.add(itsServerPanel, SERVER);
	itsSettingsPanel.add(itsPathPanel, PATHS);
	itsSettingsPanel.add(itsPackagePanel, PACKAGES);
	itsSettingsPanel.add(itsOtherPanel, OTHER);
	mainPanel.add(itsSettingsPanel, BorderLayout.CENTER);

//  	JPanel buttons = new JPanel();
//  	buttons.setLayout(new GridLayout(1, 0));
//  	buttons.add(new JButton("Save"));
//  	buttons.add(new JButton("Apply"));
//  	buttons.add(new JButton("Merge"));
//  	mainPanel.add(buttons, BorderLayout.SOUTH);

	itsMessageLabel = new JLabel("This is a message");

	getContentPane().add(mainPanel, BorderLayout.CENTER);
	getContentPane().add(itsMessageLabel, BorderLayout.SOUTH);

	makeMenuBar();

	pack();

	setVisible(true);
    }

    public void itemStateChanged(ItemEvent evt) 
    {
	CardLayout cl = (CardLayout)(itsSettingsPanel.getLayout());
	cl.show(itsSettingsPanel, (String) evt.getItem());
    }

    private final void makeMenuBar()
    {    
	JMenuBar mb = new JMenuBar();
	mb.add(new FileMenu());
	mb.add(new EditMenu(this));
	mb.add(new ScriptMenu("package"));
	mb.add(new ircam.jmax.toolkit.menus.MaxToolsJMenu("Tools"));
	mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this));
	setJMenuBar(mb);
    }

}


