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

package ircam.jmax.editors.sequence.track;

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

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;

/**
* This implementation builds a SequencePanel to represent the data.
 */
public class TrackWindow extends JMaxEditor {
	
  //------------------- fields
  transient TrackPanel trackPanel;
  FtsTrackObject trackData;
	EditMenu editMenu;
  
  public final static int DEFAULT_WIDTH  = 800;
  public final static int DEFAULT_HEIGHT = 553;
  public final static int MAX_HEIGHT     = 800;
  public final static int EMPTY_HEIGHT   = 94;
  /**
		* Constructor with FtsSequenceObject
   */
  public TrackWindow(FtsTrackObject data)
  {
    super();
		
    trackData = data;
		
    TrackEditorFactoryTable.init();
    
    makeTitle();
		
    //... then the SequencePanel
    trackPanel = new TrackPanel( this, data);
				
    getContentPane().add( trackPanel);
    
    addWindowListener(new WindowListener(){
			public void windowOpened(WindowEvent e)
		  {
        if(trackData.editorObject != null)
        {
          Rectangle bounds = TrackWindow.this.getBounds();
          trackData.editorObject.setSize(bounds.width, bounds.height);
        }
      }
			public void windowClosed(WindowEvent e){}
			public void windowClosing(WindowEvent e)
		  { 
				trackPanel.close(true);
		  }
			public void windowDeiconified(WindowEvent e){}
			public void windowIconified(WindowEvent e){}
			public void windowActivated(WindowEvent e)
		  {
				TrackEditor editor = trackPanel.getTrackEditor();
				SequenceSelection.setCurrent( editor.getSelection());
			}
			public void windowDeactivated(WindowEvent e){}
		});
		
		addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
		  {
				Rectangle bounds = TrackWindow.this.getBounds();
        trackData.editorObject.setSize(bounds.width, bounds.height);
      }
			public void componentMoved(ComponentEvent e)
		  {
				Rectangle bounds = TrackWindow.this.getBounds();
				trackData.editorObject.setLocation(bounds.x, bounds.y);
		  }
		});
		
    trackData.addTrackStateListener(new TrackStateListener(){
			public void lock(boolean lock){}
			public void active(boolean active){}
			public void restoreEditorState(FtsTrackEditorObject editorState){}
			public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection){}
			public void updateMarkers(FtsTrackObject markers, SequenceSelection markersSelection){}
      public void ftsNameChanged(String name)
      {
        setWindowName(name);
      }
		});		
        
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    else
      makeSimpleMenuBar();
		
    validate();
    
    if(trackData.editorObject != null && !trackData.editorObject.haveContent())
    {
      Dimension size = trackPanel.getPreferredSize(); 
      size.height+=(getJMenuBar().getPreferredSize().height + SequenceRuler.RULER_HEIGHT + 7);
      setSize(size);
    }
  }
	
  private final void makeTitle(){
    setWindowName( trackData.getFtsName());
  } 
  
  public void setWindowName(String name)
  {
    String text = "track "+trackData.getType().getName();
    String title;
    
    if(name != null && !name.equals(""))
      title = text+" "+ name;
    else
      title = text+" #"+((FtsGraphicObject)trackData).getObjectID();
    
    if( !getTitle().equals(title))
    {
      setTitle( MaxWindowManager.getWindowManager().makeUniqueWindowTitle(title));    
      MaxWindowManager.getWindowManager().windowChanged(this);
    }
	}
  
	
  private final void makeMenuBar()
  {
    JMenuBar mb = new JMenuBar();
    mb.add( new FileMenu());
    editMenu = new EditMenu(this);
    mb.add(editMenu);
    mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this));
		
    setJMenuBar(mb);
  }
  
  public EditMenu getEditMenu()
  {
    return editMenu;
  }
	
  EditorMenu simpleMenu = null;
  public EditorMenu getSimpleMenu()
  {
    if(simpleMenu == null)
      simpleMenu = new TrackSimpleMenu(this);
    
    return simpleMenu; 
  }  
  
  // ------ JMaxEditor ---------------
  public Editor getEditor(){
    return trackPanel;
  }
  public Rectangle getViewRectangle(){
    return trackPanel.scrollTracks.getViewport().getViewRect();
  }
}








