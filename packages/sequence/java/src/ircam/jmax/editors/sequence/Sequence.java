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

    public final int DEFAULT_WIDTH = 800;
    public final int DEFAULT_HEIGHT = 565;


  /**
   * Constructor with the SequenceRemoteData
   * It creates the panels that will display the datas of the maxData
   */
    public Sequence(FtsSequenceObject data)
    {
    super();

    MaxWindowManager.getWindowManager().addWindow(this);

    sequenceData = data;

    TrackEditorFactoryTable.setFactoryFor(AmbitusValue.info, MidiTrackEditorFactory.instance);
    TrackEditorFactoryTable.setFactoryFor(FricativeValue.info, MonodimensionalTrackEditorFactory.instance);
    TrackEditorFactoryTable.setFactoryFor(LogicValue.info, MonodimensionalTrackEditorFactory.instance);
    TrackEditorFactoryTable.setFactoryFor(CueValue.info, MonodimensionalTrackEditorFactory.instance);
    TrackEditorFactoryTable.setFactoryFor(IntegerValue.info, MonodimensionalTrackEditorFactory.instance);
    
    makeTitle(data);

    // Build The Menus and Menu Bar
    makeMenuBar();

    //... then the SequencePanel
    itsSequencePanel = new SequencePanel(this, data);

    getContentPane().add(itsSequencePanel);
    setSize(new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));

    validate();
    setVisible(true);
  }

    /* Code to interact with the "appmatch" score recognition module. PLEASE, don't delete !
       void translateToAgrep(Writer w)
       {
       PrintWriter pw = new PrintWriter(w);
       
       try {
       for (int i = 0; i<sequenceData.length(); i++)
       {
       pw.print((char) (128+sequenceData.getEventAt(i).getPitch()));
      }
      }
      catch(Exception e){}
      pw.println();
      }*/
    

    private final void makeTitle(FtsSequenceObject maxData){
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Sequence " + maxData.getName()));
    MaxWindowManager.getWindowManager().windowChanged(this);
  } 


    private final void makeMenuBar(){
	JMenuBar mb = new JMenuBar();
	
	// Build the file menu
	
	mb.add( new DefaultFileMenu());
	
	// Build the edit menu
	mb.add(new EditMenu(this)); 
	
	// Build the track menu
	mb.add(new TrackMenu(this, sequenceData)) ; 	

	// Build the options menu
	mb.add(new OptionsMenu(this) );
	    
	// New Tool menu 
	mb.add(new ircam.jmax.toolkit.menus.MaxToolsJMenu("Tools")); 
	
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
	return getContentPane().getBounds();
    }
}


