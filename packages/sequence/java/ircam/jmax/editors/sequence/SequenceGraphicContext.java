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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*; 
import ircam.jmax.editors.sequence.renderers.*; 
import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;

import java.awt.*;
import javax.swing.event.*;

/**
 * The context for an sequence editing session.
 * It stores the data model and other stuff related to Sequence
 */
public class SequenceGraphicContext extends GraphicContext {

  /**
   * Constructor */
  public SequenceGraphicContext(TrackDataModel model, SequenceSelection s, TrackEditor editor)
  {
    super();
    setDataModel(model);
    itsSelection = s;
    itsEditor = editor;
  }

  /**
   * Change the selection ownership when this graphic context become active... troppo */
  public void activate()
  {
      //SequenceSelection.setCurrent(itsSelection);
  }

  /**
   * Returns the lenght (in msec) rapresented in the current window */
  public int getTimeWindow()
  {
    return (int)(itsGraphicDestination.getSize().width/
			   (itsAdapter.getGeometry().getXZoom()));
  }


  /**
   * sets the adapter to be used in this context
   */
  public void setAdapter(Adapter theAdapter) 
  {
    itsAdapter = theAdapter;
  }

  /**
   * gets the current adapter
   */
  public Adapter getAdapter() 
  {
    return itsAdapter;
  }

  /**
   * sets the DataModel of this context
   */
  public void setDataModel(TrackDataModel theDataModel) 
  {
    itsDataModel = theDataModel;
  }

  /**
   * returns the data model
   */
  public TrackDataModel getDataModel() {
    return itsDataModel;
  }

  /**
   * returns the current logical time
   */
  public int getLogicalTime() 
  {
    return -itsAdapter.getGeometry().getXTransposition();
  }

    /*
     * returns visible rectangle of the associated track editor
     */
  public Rectangle getTrackClip()
  {
    return new Rectangle( ScoreBackground.KEYEND, 0, ((EditorContainer)getFrame()).getViewRectangle().width-ScoreBackground.KEYEND - TrackContainer.BUTTON_WIDTH - 2, getGraphicDestination().getSize().height);
  }

  public void setToolManager(ToolManager t)
  {
    toolManager = t;
  }

  public ToolManager getToolManager()
  {
    return toolManager;
  }

  public void setSelection(SequenceSelection s)
  {
    itsSelection = s;
  }

  public SequenceSelection getSelection()
  {
    return itsSelection;
  }


  public Track getTrack()
  {
    return itsEditor.getTrack();
  }
  
  public TrackEditor getTrackEditor()
  {
    return itsEditor;
  }

  // IMPLEMENTATION NOTES:
  /*
   * This is not very elegant.
   * The idea behind is that the status bar cannot be static (A tool will 
   * infact write in every window...). Construct a SequenceGraphicContext
   * With a statusBar argument, on the other side, imposes the knowledge
   * of that statusBar to the object that builds the SequenceGraphicContext
   * (in our case, the tracks...*/
  public StatusBar getStatusBar()
  {
    return ((SequenceEditor)((EditorContainer) getFrame()).getEditor()).getStatusBar();
  }
  
  public FtsGraphicObject getFtsObject()
  {
    return ((SequenceEditor)((EditorContainer) getFrame()).getEditor()).getFtsObject();
  }

  public  EditorToolbar getToolbar()
  {
    return ((SequenceEditor)((EditorContainer) getFrame()).getEditor()).getToolbar();
  }

  public void setScrollManager(ScrollManager manager)
  {
    scrollManager = manager;
  }
  public ScrollManager getScrollManager()
  {
    return scrollManager;
  }

  //---- Fields 
  
  TrackDataModel itsDataModel;
  
  SequenceSelection itsSelection;
  
  Adapter itsAdapter;
  
  int itsLogicalTime;
  
  ToolManager toolManager;
  
  ScrollManager scrollManager;

  TrackEditor itsEditor;
}




