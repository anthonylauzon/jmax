//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

import ircam.jmax.editors.sequence.menus.*;

/**
 * This implementation builds a SequencePanel to represent the data.
 */
public class SequenceWindow extends JFrame implements EditorContainer{

  //------------------- fields
  SequencePanel itsSequencePanel;
  FtsSequenceObject sequenceData;
   EditMenu editMenu;
  
  public final static int DEFAULT_WIDTH  = 800;
  public final static int DEFAULT_HEIGHT = 553;
  public final static int MAX_HEIGHT     = 800;
  public final static int EMPTY_HEIGHT   = /*94*/92;
  /**
   * Constructor with FtsSequenceObject
   */
  public SequenceWindow(FtsSequenceObject data)
  {
    super();
        
    sequenceData = data;

    TrackEditorFactoryTable.init();

    makeTitle();
    
    sequenceData.requestSequenceName();
    
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
    pack();

    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    
    validate();
    pack();
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

  public EditMenu getEditMenu()
  {
     return editMenu;
  }

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();
    
    mb.add( new FileMenu());
    editMenu = new EditMenu(this);
    mb.add(editMenu); 
    mb.add(new TrackMenu(this, sequenceData)) ; 	
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








