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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.fts.*;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;

/**
 * The class used to implement the behaviour of the "line" tool of the 
 * Table editor. It uses a LineDrawer interaction module.
 */
public class LinerTool extends Tool implements LineListener{
  
  /**
   * constructor */
  public LinerTool(ImageIcon theImageIcon)
  {
    super("liner", theImageIcon);

    itsLiner = new LineDrawer(this);
    initPopupMenu();
  }

  JLabel stepLabel;
  JSlider stepSlider;
  Box stepBox;

  void initPopupMenu()
  {
      itsMenu = new JPopupMenu();
     
      JMenu changeStepMenu = new JMenu("Change Liner Step");
      stepLabel = new JLabel("  "+DEFAULT_STEP+"  ", JLabel.CENTER);
      stepLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

      stepSlider = new JSlider(JSlider.VERTICAL, 1, 100, DEFAULT_STEP/10);
      //stepSlider.setMajorTickSpacing(10);
      stepSlider.setMinorTickSpacing(1);
      //stepSlider.setPreferredSize(new Dimension(40, 300));
      stepSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
      stepSlider.setPaintTicks(true);
      stepSlider.addChangeListener(new ChangeListener(){
	  public void stateChanged(ChangeEvent e) {
	      JSlider source = (JSlider)e.getSource();	      
	      int step = (int)source.getValue();

	      if(!source.getValueIsAdjusting())
		  setStep(step*10);
	      stepLabel.setText(""+step*10);
	}
    });
    stepBox = new Box(BoxLayout.Y_AXIS);
    stepBox.add(stepSlider);
    stepBox.add(stepLabel);
    stepBox.validate(); 
    changeStepMenu.add(stepBox);
    
    itsMenu.add(changeStepMenu);
    itsMenu.pack();
  }

  /**
   * The default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsLiner;
  }

  public JPopupMenu getMenu()
  {
      return itsMenu;
  }  

  /**
   * LineListener interface. 
   * Called by the lineDrawer UI module at mouse down.
   */
  public void lineStart(int x, int y, int modifiers) 
  {
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;	
    egc.getTrack().setProperty("selected", Boolean.TRUE);

    if(egc.getDataModel().isLocked()) return;

    //with Shift add to selection
    if((modifiers & InputEvent.SHIFT_MASK) == 0) egc.getSelection().deselectAll();

    if(egc.getAdapter().isDrawable())
    {	
	egc.getTrack().getFtsTrack().beginUpdate(); //the interpolation is undoable
	startingPoint.setLocation(x, y);
    }
  }

  /**
   * LineListener interface. Called by the LineDrawer UI module.
   */
  public void lineEnd(int x, int y) 
  {
      SequenceGraphicContext egc = (SequenceGraphicContext)gc;
      if(egc.getDataModel().isLocked()) return;
      
      drawLine(startingPoint.x, startingPoint.y, x, y);
  }

  /**
   * An utility function to draw a line between two given points in the given
   * graphic context. The points are expressed in screen coordinates.
   */
  public void drawLine(int x1, int y1, int x2, int y2)
  {
    PartitionAdapter a = (PartitionAdapter)((SequenceGraphicContext)gc).getAdapter();

    if (x1 > x2) //the line is given 'a l'inverse'
      {
	int temp;
	temp = y1;
	y1 = y2;
	y2 = temp;
	temp = x1;
	x1 = x2;
	x2 = temp;
      }

    int start = (int)a.getInvX(x1); 
    int end = (int)a.getInvX(x2);
    
    interpolate(start, end, y1, y2);    
  }

    public void interpolate(int x1, int x2, int y1, int y2)
    {
	float coeff;
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;	
	//ValueInfo info = (ValueInfo)(egc.getTrack().getTrackDataModel().getTypes().nextElement());
	ValueInfo info = egc.getTrack().getTrackDataModel().getType();
	
	if (x1 != x2) 
	    coeff = ((float)(y2 - y1))/(x2 - x1);
	else coeff = 0;

	float step = ((PartitionAdapter)egc.getAdapter()).getWidth((double)itsXStep);

	float i = x1;	
	while(i<=x2) 
	{
	    EventValue value = (EventValue)info.newInstance();
	    UtilTrackEvent aEvent = new UtilTrackEvent(value);

	    egc.getAdapter().setY(aEvent, ((int)((i-x1)*coeff)+y1));
	    egc.getTrack().getFtsTrack().requestEventCreationWithoutUpload(i, info.getName(), 
									   value.getPropertyCount(), 
									   value.getPropertyValues());
	    i+=itsXStep;
	}	  
	egc.getTrack().getFtsTrack().requestUpload();
    }


    public void setStep(int step)
    {
	itsXStep = step;
    }

    public boolean isLineDrawable()
    {
	return ((SequenceGraphicContext)gc).getAdapter().isDrawable();
    }

  //--- Fields
  LineDrawer itsLiner;
  JPopupMenu itsMenu;
  Point startingPoint = new Point();
  int itsXStep = DEFAULT_STEP;
  public final static int DEFAULT_STEP = 50;
}










