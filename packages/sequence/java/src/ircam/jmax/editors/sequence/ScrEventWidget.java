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
import javax.swing.*;
import javax.swing.event.*;
import ircam.jmax.toolkit.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * A panel that edits the fields of a TrackEvent.
 * It uses the NumericPropertyEditor class.
 * It is used in the Sequence statusBar. Simplified version... */
public class ScrEventWidget extends Box implements ListSelectionListener, TrackDataListener, ActionListener{

  /**
   * Constructor. It builds up the single widgets corresponding to
   * the properties of the object to edit. */
  public ScrEventWidget(int direction, SequenceGraphicContext gc)
  {
    super(direction);

    this.gc = gc;

    //-- TIME
    timeEditor = new NumericalWidget("time", 6, NumericalWidget.EDITABLE_FIELD);

    timeEditor.addActionListener(this);

    add(timeEditor);

    //-- PITCH
    pitchEditor = new NumericalWidget("pitch", 3, NumericalWidget.EDITABLE_FIELD);

    pitchEditor.addActionListener(this);

    add(pitchEditor);

    //-- DURATION
    durationEditor = new NumericalWidget("dur.", 6, NumericalWidget.EDITABLE_FIELD);

    durationEditor.addActionListener(this);

    add(durationEditor);

    //-- VELOCITY
    velocityEditor = new NumericalWidget("vel.", 3, NumericalWidget.EDITABLE_FIELD);

    velocityEditor.addActionListener(this);

    add(velocityEditor);


    //-- CHANNEL
    channelEditor = new NumericalWidget("ch.", 2, NumericalWidget.EDITABLE_FIELD);

    channelEditor.addActionListener(this);

    add(channelEditor);

    gc.getSelection().addListSelectionListener(this);
    gc.getDataModel().addListener(this);

  }
  

  /**
   * List selection listener interface */
  public void valueChanged(ListSelectionEvent e) 
  {
    if (gc.getSelection().isSelectionEmpty())
      setTarget(null);
    else setTarget(identifyTarget(e));
  }

  private TrackEvent identifyTarget(ListSelectionEvent e)
  {
    int count = 0;
    for (int i = gc.getSelection().getMinSelectionIndex(); i <= gc.getSelection().getMaxSelectionIndex(); i++)
      if (gc.getSelection().isSelectedIndex(i))
	count += 1;

    if (count == 1)
      return (gc.getDataModel().getEventAt(gc.getSelection().getMinSelectionIndex()));
    else return null;
  }
  
  /**
   * SequenceDataListener interface */
  public void objectDeleted(Object whichObject, int oldIndex)
  {
    if (target == whichObject) setTarget(null);
  }
  public void objectAdded(Object whichObject, int index){}
  public void objectsAdded(int maxTime){}
  public void objectChanged(Object whichObject){
    if (target == whichObject) refresh();
  }
  public void objectMoved(Object whichObject, int oldIndex, int newIndex){
    objectChanged(whichObject);
  }

  /** set the target TrackEvent to edit.
   * null means no objects */
  public void setTarget(TrackEvent e)
  {

    target = e;

    if (e!= null)
      {
	  timeEditor.setValue((int)e.getTime());//??
	pitchEditor.setValue(((Integer)e.getProperty("pitch")).intValue());
	durationEditor.setValue(((Integer)e.getProperty("duration")).intValue());
	velocityEditor.setValue(((Integer)e.getProperty("velocity")).intValue());
	channelEditor.setValue(((Integer)e.getProperty("channel")).intValue());
      }
    else 
      {
	timeEditor.setValue("");
	pitchEditor.setValue("");
	durationEditor.setValue("");
	velocityEditor.setValue("");
	channelEditor.setValue("");
      }
  }

  private void refresh()
  {
    setTarget(target);
  }


  /** Action listener interface. 
   * This class is a listener for the edit fields of its NumericalWidgets.
   * The information is used here to set the same value for 
   * all the events in a selection*/
  public void actionPerformed(ActionEvent e)
  {
    if (gc.getSelection().isSelectionEmpty()) return;
    
    int value;
    TrackEvent temp;
    
    try {
      value = Integer.parseInt(((JTextField) e.getSource()).getText());
    } catch (Exception ex) {
      return;
    }

    //((UndoableData) gc.getDataModel()).beginUpdate();


    Enumeration en;

    if (e.getSource() == timeEditor.getCustomComponent()) 
      {  // modify the time of a selection
	for (en = gc.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (TrackEvent) en.nextElement();
	    temp.move((double)value);
	  }
      }
    else if (e.getSource() == pitchEditor.getCustomComponent()) 
      {  // modify the pitch of a selection
	for (en = gc.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (TrackEvent) en.nextElement();
	    temp.setProperty("pitch", new Integer(value));
	  }
      }
    else if (e.getSource() == durationEditor.getCustomComponent()) 
      {  // modify the duration of a selection
	for (en = gc.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (TrackEvent) en.nextElement();

	    temp.setProperty("duration", new Integer(value));
	  }
      }
    else if (e.getSource() == velocityEditor.getCustomComponent()) 
      {  // modify the velocity of a selection
	for (en = gc.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (TrackEvent) en.nextElement();
	    temp.setProperty("velocity", new Integer(value));
	  }
      }
    else if (e.getSource() == channelEditor.getCustomComponent()) 
      {  // modify the channel of a selection
	for (en = gc.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (TrackEvent) en.nextElement();
	    temp.setProperty("channel", new Integer(value));
	  }
      }

    //((UndoableData) gc.getDataModel()).endUpdate();

  }


  //-- Fields
  SequenceGraphicContext gc;
    //  Dimension dim = new Dimension();
  int totalWidth = 0;

  TrackEvent target;
  NumericalWidget timeEditor;
  NumericalWidget pitchEditor;
  NumericalWidget durationEditor;
  NumericalWidget velocityEditor;
  NumericalWidget channelEditor;

}
