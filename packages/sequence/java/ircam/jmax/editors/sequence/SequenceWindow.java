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
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

import ircam.jmax.editors.sequence.menus.*;

/**
* This implementation builds a SequencePanel to represent the data.
 */
public class SequenceWindow extends JMaxEditor {
  
  //------------------- fields
  SequencePanel itsSequencePanel;
  FtsSequenceObject sequenceData;
  EditMenu editMenu;
  
  public final static int DEFAULT_WIDTH  = 800;
  public final static int DEFAULT_HEIGHT = 553;
  public final static int MAX_HEIGHT     = 800;
  public final static int EMPTY_HEIGHT   = 92;
  /**
    * Constructor with FtsSequenceObject
   */
  public SequenceWindow(FtsSequenceObject data)
  {
    super();
    
    sequenceData = data;
    
    TrackEditorFactoryTable.init();
    sequenceData.requestSequenceName();
    
    itsSequencePanel = new SequencePanel(this, data);    
    getContentPane().add(itsSequencePanel);
    
    makeSequenceWindow();
    
    setSize(new Dimension(DEFAULT_WIDTH + TrackContainer.BUTTON_WIDTH, EMPTY_HEIGHT));
  }
  
  public SequenceWindow(SequenceWindow seqCopy)
  {
    super();
    
    sequenceData = seqCopy.sequenceData;
    
    itsSequencePanel = seqCopy.itsSequencePanel;   
    itsSequencePanel.setContainer(this);
    Rectangle bounds = seqCopy.getBounds();
    seqCopy.getContentPane().remove(itsSequencePanel);
    getContentPane().add(itsSequencePanel);
    seqCopy.dispose();
    System.gc();
    
    makeSequenceWindow();
    setBounds(bounds);
  }
  
  private void makeSequenceWindow()
  {
    makeTitle();
    makeListeners();
    
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    else
      makeSimpleMenuBar();
    validate();
  }
  
  private void makeListeners()
  {
    addWindowListener(new WindowListener(){
      public void windowOpened(WindowEvent e)
      {
        /*if(!sequenceData.isUploading())
        {
          Rectangle bounds = SequenceWindow.this.getBounds();
          sequenceData.setSize(bounds.width, bounds.height);
        }*/
      }
      public void windowClosed(WindowEvent e){}
      public void windowClosing(WindowEvent e)
      {
        MaxWindowManager.getWindowManager().removeWindow(getFrame());
        itsSequencePanel.close(true);
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
    
    /*addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
		  {
        if(!sequenceData.isUploading())
        {
          Rectangle bounds = SequenceWindow.this.getBounds();
          sequenceData.setSize(bounds.width, bounds.height);
        }
      }
			public void componentMoved(ComponentEvent e)
		  {
        if(!sequenceData.isUploading())
        {
          Rectangle bounds = SequenceWindow.this.getBounds();
          sequenceData.setLocation(bounds.x, bounds.y);
        }
      }
		});*/
    
    sequenceData.addTrackListener( new TrackListener(){
      public void trackAdded(Track track, boolean isUploading){}
      public void tracksAdded(int maxTime){}
      public void trackRemoved(Track track){}
      public void trackChanged(Track track){}
      public void trackMoved(Track track, int oldPosition, int newPosition){}   
      public void ftsNameChanged(String name)
      {
        setWindowName(name);
      }      
      public void sequenceStartUpload(){};
      public void sequenceEndUpload(){}; 
      public void sequenceClear(){}; 
    });
  }
  
  private final void makeTitle(){
    setWindowName( sequenceData.getFtsName());
  } 
  
  public void setWindowName(String name)
  {
    if(name != null && !name.equals(""))
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("MultiTrack " + name));
    else
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("MultiTrack #"+((FtsGraphicObject)sequenceData).getObjectID()));

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
  
  EditorMenu simpleMenu = null;
  public EditorMenu getSimpleMenu()
  {
    if(simpleMenu == null)
      simpleMenu = new SequenceSimpleMenu(this);
    
    return simpleMenu; 
  }  
  
  // ------ JMaxEditor  ---------------
  public Editor getEditor(){
    return itsSequencePanel;
  }
  public Rectangle getViewRectangle(){
    return itsSequencePanel.scrollTracks.getViewport().getViewRect();
  }
}








