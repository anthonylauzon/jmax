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
// Authors: Maurizio De Cecco, Francois Dechelle, Riccardo Borghesi, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

import ircam.jmax.editors.sequence.menus.*;

/**
 * This implementation builds a SequencePanel to represent the data.
 */
public class Sequence extends JFrame implements EditorContainer{

    //------------------- fields
    SequencePanel itsSequencePanel;
    FtsSequenceObject sequenceData;

    public final static int DEFAULT_WIDTH  = 800;
    public final static int DEFAULT_HEIGHT = 553;
    public final static int MAX_HEIGHT     = 800;
    public final static int EMPTY_HEIGHT   = 78;
  /**
   * Constructor with FtsSequenceObject
   */
    public Sequence(FtsSequenceObject data)
    {
	super();

	MaxWindowManager.getWindowManager().addWindow(this);

	sequenceData = data;

	initTrackEditorFactoryTable();
	
	makeTitle();

	sequenceData.requestSequenceName();

	// Build The Menus and Menu Bar
	makeMenuBar();

	//... then the SequencePanel
	itsSequencePanel = new SequencePanel(this, data);
	
	getContentPane().add(itsSequencePanel);
	setSize(new Dimension(DEFAULT_WIDTH, EMPTY_HEIGHT));

	addWindowListener(new WindowListener(){
		public void windowOpened(WindowEvent e){}
		public void windowClosed(WindowEvent e){}
		public void windowClosing(WindowEvent e)
		{
		    MaxWindowManager.getWindowManager().removeWindow(getFrame());
		}
		public void windowDeiconified(WindowEvent e){}
		public void windowIconified(WindowEvent e){}
		public void windowActivated(WindowEvent e)
		{
		    TrackEditor current = itsSequencePanel.getCurrentTrackEditor();
		    if(current!=null)
			SequenceSelection.setCurrent(current.getSelection());
		}
		public void windowDeactivated(WindowEvent e){}
	    });
	
	validate();
	setVisible(true);
    }

    private final void initTrackEditorFactoryTable()
    {
	TrackEditorFactoryTable.setFactoryFor(AmbitusValue.info, MidiTrackEditorFactory.instance);
	TrackEditorFactoryTable.setFactoryFor(IntegerValue.info, IntegerTrackEditorFactory.instance);
	TrackEditorFactoryTable.setFactoryFor(FloatValue.info, FloatTrackEditorFactory.instance);
	TrackEditorFactoryTable.setFactoryFor(MessageValue.info, MessageTrackEditorFactory.instance);
	TrackEditorFactoryTable.setFactoryFor(MidiValue.info, IntegerTrackEditorFactory.instance);
    }

    private final void makeTitle(){
	setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Sequence"));
	MaxWindowManager.getWindowManager().windowChanged(this);
    } 

    public void setName(String name)
    {
	setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Sequence " + name));
	MaxWindowManager.getWindowManager().windowChanged(this);
    }

    private final void makeMenuBar(){
	JMenuBar mb = new JMenuBar();
	
	// Build the file menu	
	mb.add( new FileMenu());
	
	// Build the edit menu
	mb.add(new EditMenu(this)); 
	
	// Build the track menu
	mb.add(new TrackMenu(this, sequenceData)) ; 	

	// Build the options menu
	//mb.add(new OptionsMenu(this) );
	    
	// New Tool menu 
	//mb.add(new ircam.jmax.toolkit.menus.MaxToolsJMenu("Tools")); 
	
	// New Window Manager based Menu
	mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this)); 
	
	setJMenuBar(mb);
    }
    
    // ------ editorContainer interface ---------------
    public Editor getEditor(){
	return itsSequencePanel;
    }
    public Frame getFrame(){
	return this;
    }
    public Point getContainerLocation(){
	return getLocation();
    }
    public Rectangle getViewRectangle(){
	return itsSequencePanel.scrollTracks.getViewport().getViewRect();
    }
}








