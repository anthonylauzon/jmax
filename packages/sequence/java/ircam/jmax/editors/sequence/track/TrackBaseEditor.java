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

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.beans.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

public abstract class TrackBaseEditor extends PopupToolbarPanel implements TrackDataListener, ListSelectionListener, TrackEditor
{
  public TrackBaseEditor(Geometry geom, Track trk)
  {
    super();

    this.track = trk;
    this.geometry = geom;

    setBackground(Color.white);
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));

    setOpaque(false);

    gc = createGraphicContext(geometry, track);

    setDisplayer();

    track.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
	public void propertyChange(PropertyChangeEvent e)
	{	  
	  if (e.getPropertyName().equals("selected") && e.getNewValue().equals(Boolean.TRUE))
	    SequenceSelection.setCurrent(selection);
	}
      });

    geometry.addTranspositionListener(new TranspositionListener() {
	public void transpositionChanged(int newTranspose)
	{
	  repaint();
	}
      });

    selection.addListSelectionListener(this);

    selection.setOwner(new SelectionOwner() {
	public void selectionDisactivated()
	{
	  repaint();
	}
	public void selectionActivated()
	{
	  repaint();
	}
      });

    track.getTrackDataModel().addListener(this);
    
    track.getTrackDataModel().addTrackStateListener(new TrackStateListener(){
	public void lock(boolean lock)
	{
	  for (Enumeration e = oldElements.elements(); e.hasMoreElements();) 
	    ((TrackEvent) e.nextElement()).setHighlighted(false);
	  
	  oldElements.removeAllElements();
	  getTrack().setProperty("locked", new Boolean(lock));
	}
	public void active(boolean active)
	{
	  getTrack().setProperty("active", (active) ? Boolean.TRUE : Boolean.FALSE);
	}
      });
	
    track.getTrackDataModel().addHighlightListener(new HighlightListener(){
	public void highlight(Enumeration elements, double time)
	{
	  TrackEvent temp;
	  boolean first = true;
	  
	  Rectangle clipRect = gc.getTrackClip().intersection(gc.getScrollManager().getViewRectangle());
	  Graphics g = getGraphics();
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
	      temp.setHighlighted(true);
	      temp.getRenderer().render(temp, g, Event.HIGHLIGHTED, gc);
	      oldElements.addElement(temp);			    
	    }
	}
      });
    
    addMouseListener(new MouseListener(){
	public void mouseClicked(MouseEvent e){}
	public void mousePressed(MouseEvent e){}
	public void mouseReleased(MouseEvent e){}
	public void mouseEntered(MouseEvent e)
	{
	  requestFocus();
	}
	public void mouseExited(MouseEvent e){
	  gc.getDisplayer().display("");
	}
      });
    addMouseMotionListener(new MouseMotionListener(){
	public void mouseMoved(MouseEvent e)
	{
	  double time = gc.getAdapter().getInvX(e.getX());
	  if(time < 0) time = 0;	      
	  int val =  gc.getAdapter().getInvY(e.getY());
	      
	  gc.getDisplayer().display(Displayer.numberFormat.format(time)+" , "+
				    Displayer.numberFormat.format(val));	
	}
	public void mouseDragged(MouseEvent e){}
      });

    component = this;
  }

  void setDisplayer()
  {
    JPanel labelPanel = new JPanel();
    labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
    labelPanel.setOpaque(false);
    
    displayLabel = new JLabel();
    displayLabel.setFont( Displayer.displayFont);
    displayLabel.setForeground( Color.gray);
    displayLabel.setPreferredSize( new Dimension(102, 15));
    displayLabel.setMaximumSize( new Dimension(102, 15));
    displayLabel.setMinimumSize( new Dimension(102, 15));
    
    labelPanel.add( Box.createRigidArea( new Dimension(5, 0)) );
    labelPanel.add( displayLabel);
    labelPanel.add( Box.createHorizontalGlue());
    
    add(labelPanel);
    add(Box.createVerticalGlue());
    
    gc.setDisplayer( new Displayer(){
	public void display(String text)
	{
	  displayLabel.setText( text);
	}
      });	
  }

  public abstract void reinit();
  abstract void createPopupMenu();
  abstract SequenceGraphicContext createGraphicContext(Geometry geometry, Track track);
  public abstract int getDefaultHeight();

  public JPopupMenu getMenu()
  {    
    if( popup ==null)
      createPopupMenu();
    
    popup.update();
    return popup;
  }

  public int trackCount()
  {
    if( gc.getFtsObject() instanceof FtsSequenceObject)
      return ((FtsSequenceObject)gc.getFtsObject()).trackCount();
    else
      return 1;
  }

    /**
     * Callback from the toolbar when a new tool have been
     * selected by the user
     */ 
  public void toolChanged(ToolChangeEvent e){}
    
  /*******************************************************************************
   * DataTrackListener interface
   ******************************************************************************/
  
  boolean uploading  = false;
  public void objectChanged(Object spec, String propName, Object propValue) 
  {
    repaint();
  }
  
  public void objectAdded(Object spec, int index) 
  {
    if( !uploading)
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
  
  public void trackCleared() 
  {
    repaint();
  }
  public void startTrackUpload( TrackDataModel track, int size)
  {
    uploading  = true;
  }
  public void endTrackUpload( TrackDataModel track)
  {
    uploading  = false;
  }
  public void startPaste(){}
  public void endPaste(){}
    
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex) 
  {
    repaint();
  }
  
  public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
  
  public void trackNameChanged(String oldName, String newName) 
  {
    track.setProperty("trackName", newName);
  }
  
  /*****************************************************************************
   * ListSelectionListener interface
   *****************************************************************************/
    
  public void valueChanged(ListSelectionEvent e)
  {
    repaint();
  }

  /* avoid to paint the white background twice*/   
  public void update(Graphics g) {}
  
  
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

  public Dimension getPreferredSize()
  {
    return new Dimension(SequenceWindow.DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, DEFAULT_HEIGHT);
  }
  
  public Track getTrack()
  {
    return track;
  }
  
  public void updateNewObject(Object obj){};

  public void setViewMode(int viewType)
  {
    if(viewMode!=viewType)
      {
	viewMode=viewType;
	track.setProperty("viewMode", new Integer(viewType));
	repaint();
      }    
  }
  public int getViewMode()
  {
    return viewMode;
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

  public void processKeyEvent(KeyEvent e)
  {
    if(SequenceTextArea.isDeleteKey(e))
      {
	if(e.getID()==KeyEvent.KEY_PRESSED)
	  {
	    ((UndoableData)track.getTrackDataModel()).beginUpdate();
	    getSelection().deleteAll();
	  }	    
      }
    else if((e.getKeyCode() == KeyEvent.VK_TAB)&&(e.getID()==KeyEvent.KEY_PRESSED))
      if(e.isControlDown())
	getSelection().selectPrevious();
      else
	getSelection().selectNext();
    
    super.processKeyEvent(e);
    requestFocus();
  }

  public void paintComponent(Graphics g) 
  {
    Rectangle r = g.getClipBounds();
    renderer.render(g, r);
  }
  
  //--- MidiTrack fields
  transient Geometry geometry;
  transient SequenceGraphicContext gc;
  transient Track track;
  transient AbstractRenderer renderer;
  transient Component component;
  transient SequenceSelection selection;
    
  transient MaxVector oldElements = new MaxVector();
  transient SequenceTableDialog listDialog = null;
  
  public int DEFAULT_HEIGHT = 430;
  public int viewMode;
  TrackBasePopupMenu popup = null;
  JLabel displayLabel;
}



