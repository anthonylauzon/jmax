
package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import ircam.jmax.toolkit.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * A panel that edits the fields of a ScrEvent.
 * It uses the NumericPropertyEditor class.
 * It is used in the Explode statusBar */
class ScrEventWidget extends Box implements SelectionListener, ExplodeDataListener, ActionListener{

  /**
   * Constructor. It builds up the single widgets corresponding to
   * the properties of the object to edit. */
  public ScrEventWidget(ExplodeGraphicContext theGc)
  {
    super(BoxLayout.X_AXIS);

    gc = theGc;

    //-- TIME
    timeEditor = new NumericalWidget("time", 6, NumericalWidget.EDITABLE_FIELD);

    timeEditor.addActionListener(this);

    add(timeEditor);
    totalWidth += timeEditor.getSize().width;

    //-- PITCH
    pitchEditor = new NumericalWidget("pitch", 3, NumericalWidget.EDITABLE_FIELD);

    pitchEditor.addActionListener(this);

    add(pitchEditor);
    totalWidth += pitchEditor.getSize().width;

    //-- DURATION
    durationEditor = new NumericalWidget("dur.", 6, NumericalWidget.EDITABLE_FIELD);

    durationEditor.addActionListener(this);

    add(durationEditor);
    totalWidth += durationEditor.getSize().width;

    //-- VELOCITY
    velocityEditor = new NumericalWidget("vel.", 3, NumericalWidget.EDITABLE_FIELD);

    velocityEditor.addActionListener(this);

    add(velocityEditor);
    totalWidth += velocityEditor.getSize().width;


    //-- CHANNEL
    channelEditor = new NumericalWidget("ch.", 2, NumericalWidget.EDITABLE_FIELD);

    channelEditor.addActionListener(this);

    add(channelEditor);
    totalWidth += channelEditor.getSize().width;

    dim.setSize(totalWidth, HEIGHT);

    setSize(dim.width, dim.height);

    ExplodeSelection.getSelection().addSelectionListener(this);
    gc.getDataModel().addListener(this);
  }
  
  /**
   * selection listener interface */
  public void objectSelected()
  {
    setTarget(identifyTarget());
  }

  public void objectDeselected()
  {
    setTarget(identifyTarget());
  }

  public void groupSelected()
  {
    setTarget(null);
  }

  public void groupDeselected()
  {
    setTarget(identifyTarget());
  }
  
  private ScrEvent identifyTarget()
  {
    if (ExplodeSelection.getSelection().size() == 1) 
      {
	ScrEvent aEvent = (ScrEvent) ExplodeSelection.getSelection().getSelected().nextElement();
	return aEvent;
      }
    else return null;
  }
  
  /**
   * ExplodeDataListener interface */
  public void objectDeleted(Object whichObject)
  {
    if (target == whichObject) setTarget(null);
  }
  public void objectAdded(Object whichObject){}
  public void objectChanged(Object whichObject){
    if (target == whichObject) refresh();
  }

  /** set the target ScrEvent to edit.
   * null means no objects */
  public void setTarget(ScrEvent e)
  {

    target = e;

    if (e!= null)
      {
	timeEditor.setValue(e.getTime());
	pitchEditor.setValue(e.getPitch());
	durationEditor.setValue(e.getDuration());
	velocityEditor.setValue(e.getVelocity());
	channelEditor.setValue(e.getChannel());
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
    if (ExplodeSelection.getSelection().size() == 0) return;
    
    int value;
    ScrEvent temp;
    
    try {
      value = Integer.parseInt(((JTextField) e.getSource()).getText());
    } catch (Exception ex) {
      return;
    }

    gc.getDataModel().beginUpdate();


    Enumeration en;

    if (e.getSource() == timeEditor.getCustomComponent()) 
      {  // modify the time of a selection
	for (en = ExplodeSelection.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (ScrEvent) en.nextElement();
	    temp.setTime(value);
	  }
      }
    else if (e.getSource() == pitchEditor.getCustomComponent()) 
      {  // modify the pitch of a selection
	for (en = ExplodeSelection.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (ScrEvent) en.nextElement();
	    temp.setPitch(value);
	  }
      }
    else if (e.getSource() == durationEditor.getCustomComponent()) 
      {  // modify the duration of a selection
	for (en = ExplodeSelection.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (ScrEvent) en.nextElement();
	    temp.setDuration(value);
	  }
      }
    else if (e.getSource() == velocityEditor.getCustomComponent()) 
      {  // modify the velocity of a selection
	for (en = ExplodeSelection.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (ScrEvent) en.nextElement();
	    temp.setVelocity(value);
	  }
      }
    else if (e.getSource() == channelEditor.getCustomComponent()) 
      {  // modify the channel of a selection
	for (en = ExplodeSelection.getSelection().getSelected(); en.hasMoreElements();)
	  {
	    temp = (ScrEvent) en.nextElement();
	    temp.setChannel(value);
	  }
      }

    gc.getDataModel().endUpdate();

  }

  public Dimension getPreferredSize()
  {
    return dim;
  }

  public Dimension getMinimumSize()
  {
    return dim;
  }


  //-- Fields
  ExplodeGraphicContext gc;
  Dimension dim = new Dimension();
  int totalWidth = 0;

  ScrEvent target;
  NumericalWidget timeEditor;
  NumericalWidget pitchEditor;
  NumericalWidget durationEditor;
  NumericalWidget velocityEditor;
  NumericalWidget channelEditor;

  public static final int HEIGHT = 20;
}
