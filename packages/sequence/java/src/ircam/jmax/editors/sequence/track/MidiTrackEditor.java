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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;
import java.awt.*;
import java.beans.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * A view for a Midi sequence (sequence composed of events whose Value field is
 * a Midi value) */
public class MidiTrackEditor extends JPanel implements TrackDataListener, ListSelectionListener, PopupProvider, TrackEditor
{
    public MidiTrackEditor(Geometry geometry, Track track)
    {
	if(track.getProperty("maximumPitch")==null)
	    track.setProperty("maximumPitch", new Integer(AmbitusValue.DEFAULT_MAX_PITCH));
	if(track.getProperty("minimumPitch")==null)
	    track.setProperty("minimumPitch", new Integer(AmbitusValue.DEFAULT_MIN_PITCH));

	setLayout(new BorderLayout());
	setBackground(Color.white);

	setOpaque(false);

	this.track = track;
	this.geometry = geometry;

	//-- prepares the CENTER panel (the score)
	// a simple panel, that just uses a Renderer as its paint method...
	// it takes care of the popup showing.

	itsScore = new ScorePanel(this);

	gc = prepareGraphicContext(geometry, track);

	add(itsScore, BorderLayout.CENTER);

	//---- simple things for simple minds

	geometry.addTranspositionListener(new TranspositionListener() {
	    public void transpositionChanged(int newTranspose)
		{
		    itsScore.repaint();
		}
	});
	// make this panel repaint when the selection status change
	// either in content or in ownership.
	selection.addListSelectionListener(this);

	selection.setOwner(new SelectionOwner() {
	    public void selectionDisactivated()
		{
		    itsScore.repaint();
		}
	    public void selectionActivated()
		{
		    itsScore.repaint();
		}
	});

	track.getTrackDataModel().addListener(this);

	track.getTrackDataModel().addLockListener(new LockListener(){
		public void lock(boolean lock)
		{
		    for (Enumeration e = oldElements.elements(); e.hasMoreElements();) 
			((TrackEvent) e.nextElement()).setHighlighted(false);

		    oldElements.removeAllElements();
		    getTrack().setProperty("locked", new Boolean(lock));
		}
	    });
	
	track.getTrackDataModel().addHighlightListener(new HighlightListener(){
		public void highlight(Enumeration elements, double time)
		{
		    TrackEvent temp;
		    boolean first = true;
		    
		    Rectangle clipRect = gc.getTrackClip().intersection(gc.getScrollManager().getViewRectangle());
		    Graphics g = itsScore.getGraphics();  
		    g.setClip(clipRect.x, clipRect.y, clipRect.width, clipRect.height);
		    
		    for (Enumeration e = oldElements.elements(); e.hasMoreElements();) 
			{
			    temp = (TrackEvent) e.nextElement();
			    temp.setHighlighted(false);
			    temp.getRenderer().render(temp, g, false, gc);			    
			}
		    oldElements.removeAllElements();

		    for (Enumeration e = elements; e.hasMoreElements();) 
			{
			    temp = (TrackEvent) e.nextElement();
			    if(first)
				{
				    gc.getScrollManager().makeVisible(temp);
				    first = false;
				}
			    //temp.getRenderer().render(temp, g, true, gc);
			    temp.setHighlighted(true);
			    temp.getRenderer().render(temp, g, Event.HIGHLIGHTED, gc);
			    oldElements.addElement(temp);			    
			}
		}
	    });

	component = this;
    }

    public void reinit()
    {
	track.setProperty("maximumPitch", new Integer(AmbitusValue.DEFAULT_MAX_PITCH));
	track.setProperty("minimumPitch", new Integer(AmbitusValue.DEFAULT_MIN_PITCH));	
	setViewMode(PIANOROLL_VIEW);		
	((FtsTrackObject)track.getTrackDataModel()).setUntitled();
    }

    public JMenu getToolsMenu()
    {
	return gc.getToolbar().itsMenu;
    }
    public JPopupMenu getMenu()
    {
	MidiTrackPopupMenu.getInstance().update(this);
	return MidiTrackPopupMenu.getInstance();
    }

    public int trackCount()
    {
	return gc.getFtsSequenceObject().trackCount();
    }

    private SequenceGraphicContext prepareGraphicContext(Geometry geometry, Track track)
    {
	selection = new SequenceSelection(track.getTrackDataModel());
	
	//--- make this selection the current one when the track is activated
	track.getPropertySupport().addPropertyChangeListener(new MidiTrackPropertyChangeListener());
	gc = new SequenceGraphicContext(track.getTrackDataModel(), selection, track); //loopback?
	gc.setGraphicSource(itsScore);
	gc.setGraphicDestination(itsScore);
	PartitionAdapter ad = new PartitionAdapter(geometry, gc);
	track.getPropertySupport().addPropertyChangeListener(ad);
	gc.setAdapter(ad);

	renderer = new ScoreRenderer(gc);
	return gc;
    }

    /**
     * Callback from the toolbar when a new tool have been
     * selected by the user
     */ 
    public void toolChanged(ToolChangeEvent e) 
    {
	//SUSPENDED: requires knowledge of the status bar
	/*if (e.getTool() != null) 
	  {
	  itsStatusBar.post(e.getTool(), "");
	  }*/
    }
    
    /**
     * called when the database is changed: DataTrackListener interface
     */
    
    public void objectChanged(Object spec, String propName, Object propValue) 
    {
	repaint();
    }
    
    public void objectAdded(Object spec, int index) 
    {
	repaint();
    }

    public void objectsAdded(int maxTime) 
    {
	repaint();
    }
    
    public void objectDeleted(Object whichObject, int index) 
    {
	repaint();
    }
    
    public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
    {
	repaint();
    }
    
    public void trackNameChanged(String oldName, String newName) 
    {
	track.setProperty("trackName", newName);
    }

    /**
     * ListSelectionListener interface
     */
    
    public void valueChanged(ListSelectionEvent e)
    {
	repaint();
    }

    /* avoid to paint the white background twice*/   
    public void update(Graphics g) {}
    
    
    /**
     * get the lenght (in milliseconds) of the window
     */
    public int windowTimeWidth() 
    {
	return (int) (gc.getAdapter().getInvX(itsScore.getSize().width) - gc.getAdapter().getInvX(ScoreBackground.KEYEND)) - 1;
	
    }


  /**
   * from the StatusBarClient interface
   */
  public String getName() 
  {
    return "";
  }


  /**
   * from the StatusBarClient interface
   */
  public ImageIcon getIcon() 
  {
    return null;
  }

    public SequenceSelection getSelection()
    {
	return selection;
    }


    public void dispose()
    {
	if(listDialog != null)
	    listDialog.dispose();
    }


    public Component getComponent()
    {
      return component;
    }

    public void setComponent(Component c)
    {
      component = c;
    }

    public SequenceGraphicContext getGraphicContext()
    {
	return gc;
    }

    // good old f*****g HTML-style...

    public Dimension getPreferredSize()
    {
	return new Dimension(800, DEFAULT_HEIGHT);
    }

    public Track getTrack()
    {
	return track;
    }

    public void updateNewObject(Object obj){};

    public SequenceGraphicContext getGc()
    {
	return gc;
    }
    public void setViewMode(int viewType)
    {
	if(viewMode!=viewType)
	{
	    viewMode=viewType;
	    renderer.setViewMode(viewMode);
	    //((PartitionAdapter)gc.getAdapter()).setViewMode(viewMode);
	    track.setProperty("viewMode", new Integer(viewType));
	    repaint();
	}    
    }
    public int getViewMode()
    {
      return viewMode;
    }

    public int getDefaultHeight()
    {
	return ((PartitionAdapter)gc.getAdapter()).getRangeHeight();
    }
    public void showListDialog()
    {
	if(listDialog==null) 
	    createListDialog();
	listDialog.setVisible(true);
    }

    private void createListDialog()
    {
	listDialog = new SequenceTableDialog(track, gc.getFrame(), gc);
    }

    public boolean isDisplayLabels()
    {
	return ((PartitionAdapter)gc.getAdapter()).isDisplayLabels();
    }
    public void setDisplayLabels(boolean display)
    {
	((PartitionAdapter)gc.getAdapter()).setDisplayLabels(display);
	itsScore.repaint();
    }

    class ScorePanel extends PopupToolbarPanel
    {
	MidiTrackEditor editor;
	ScorePanel(MidiTrackEditor editor)
	{
	  super(editor);
	  this.editor = editor; 
	}

	protected void processMouseEvent(MouseEvent e)
	{
	    if(e.isPopupTrigger())
	    {
		if(editor.renderer.getViewMode()==NMS_VIEW)
		{
		  TrackEvent event = null;
		  int x = e.getX();
		  int y = e.getY();
		  event = (TrackEvent)editor.renderer.firstObjectContaining(x,y);
		  if(event!=null)
		    {
		      MidiEventPopupMenu popup = (MidiEventPopupMenu)event.getValue().getPopupMenu();
		      if(popup!=null)
		      {
			popup.update(event, editor.gc);
			popup.show (e.getComponent(), x-10, y-10);
		      }		      
		      return;
		    }
		}
	    }
	    super.processMouseEvent(e);
	}

	public void processKeyEvent(KeyEvent e)
	{
	    if(SequenceTextArea.isDeleteKey(e))
		{
		    if(e.getID()==KeyEvent.KEY_PRESSED)
			{
			    ((UndoableData)track.getTrackDataModel()).beginUpdate();
			    editor.getSelection().deleteAll();
			}	    
		}
	    else if((e.getKeyCode() == KeyEvent.VK_TAB)&&(e.getID()==KeyEvent.KEY_PRESSED))
		if(e.isControlDown())
		    editor.getSelection().selectPrevious();
		else
		    editor.getSelection().selectNext();

	    super.processKeyEvent(e);
	    requestFocus();
	}
   
	public void paint(Graphics g) 
	{
	  Rectangle r = g.getClipBounds();
	  renderer.render(g, r); //et c'est tout	
	}
    }

    class MidiTrackPropertyChangeListener implements PropertyChangeListener 
    {
	public void propertyChange(PropertyChangeEvent e)
	{
	    if (e.getPropertyName().equals("active") && e.getNewValue().equals(Boolean.TRUE))
		SequenceSelection.setCurrent(selection);
	}
    }


    //--- MidiTrack fields
    Geometry geometry;
    SequenceGraphicContext gc;
    Track track;
    ScoreRenderer renderer;
    Component component;
    SequenceSelection selection;
    
    ScorePanel itsScore;

    MaxVector oldElements = new MaxVector();
    SequenceTableDialog listDialog = null;
    /*****************/

    int viewMode = PIANOROLL_VIEW;
    public static int DEFAULT_HEIGHT = 430;
    static public final int PIANOROLL_VIEW = 0;
    static public final int NMS_VIEW = 1;
}




