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
public class CombTool extends Tool implements CombListener{
  
  /**
   * constructor */
  public CombTool(ImageIcon theImageIcon)
  {
    super("Comb", theImageIcon);

    itsComber = new CombDrawer(this);
    initPopupMenu();
  }

  JLabel rayLabel;
  JSlider raySlider;
  Box rayBox;
  void initPopupMenu()
  {
      itsMenu = new JPopupMenu();
      
      JMenu changeRayMenu = new JMenu("Change Comb Ray");
      rayLabel = new JLabel("  "+DEFAULT_RAY+"  ", JLabel.CENTER);
      rayLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
      
      raySlider = new JSlider(JSlider.VERTICAL, 1, 50, DEFAULT_RAY);
      raySlider.setMinorTickSpacing(1);
      raySlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
      raySlider.setPaintTicks(true);
      raySlider.addChangeListener(new ChangeListener(){
	  public void stateChanged(ChangeEvent e) {
	      JSlider source = (JSlider)e.getSource();	      
	      int ray = (int)source.getValue();
      
	      if(!source.getValueIsAdjusting())
		  setRay(ray);
	      rayLabel.setText(""+ray);
	  }
      });
      rayBox = new Box(BoxLayout.Y_AXIS);
      rayBox.add(raySlider);
      rayBox.add(rayLabel);
      rayBox.validate(); 
      changeRayMenu.add(rayBox);
      
      itsMenu.add(changeRayMenu);
      itsMenu.pack();
  }

  /**
   * The default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsComber;
  }

  public JPopupMenu getMenu()
  {
      return itsMenu;
  } 

  /**
   * LineListener interface. 
   * Called by the lineDrawer UI module at mouse down.
   */
  public void combStart(int x, int y, int modifiers) 
  {
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
    egc.getTrack().setProperty("active", Boolean.TRUE);
    //with Shift add to selection
    if((modifiers & InputEvent.SHIFT_MASK) == 0) egc.getSelection().deselectAll();

    egc.getTrack().getFtsTrack().beginUpdate(); //combing is undoable

    if(egc.getAdapter().isDrawable())    
	comb(x, y, modifiers);    
  }

  /**
   * LineListener interface. Called by the LineDrawer UI module.
   */
  public void comb(int x, int y, int modifiers) 
  {
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;	
    PartitionAdapter a = (PartitionAdapter)egc.getAdapter();
    TrackEvent aEvent;

    int yZero = a.getY(0);
    
    for (Enumeration e = gc.getRenderManager().
	     objectsIntersecting(x-itsRay, y-itsRay, 
				 2*itsRay, 2*itsRay); e.hasMoreElements();) 
	{
	    aEvent = (TrackEvent) e.nextElement();
	    if(a.getY(aEvent) < yZero)
		{	
		    if(a.getY(aEvent) < y+itsRay)
			if(y+itsRay <= yZero)
			    a.setY(aEvent, y+itsRay);
			else
			    if(a instanceof FloatAdapter)
				FloatMapper.getMapper().set(aEvent, (float)0.0);
			    else
				a.YMapper.set(aEvent, 0);
		}		
	    else
		if(a.getY(aEvent) > y-itsRay)
		    if(y-itsRay >= yZero)
			a.setY(aEvent, y-itsRay);
		    else
			{
			    if(a instanceof FloatAdapter)
				FloatMapper.getMapper().set(aEvent, (float)0.0);
			    else
				a.YMapper.set(aEvent, 0);
			}
	}
  }

  public void combEnd(int x, int y, int modifiers) 
  {
      comb(x, y, modifiers);
      gc.getGraphicDestination().repaint();
      ((SequenceGraphicContext)gc).getTrack().getFtsTrack().endUpdate();    	  
  }

  public void setRay(int ray)
  {
      itsRay = ray;
      itsComber.setRay(ray);
  }
  
  public boolean isCombDrawable()
  {
      return ((SequenceGraphicContext)gc).getAdapter().isDrawable();
  }
  //--- Fields
  CombDrawer itsComber;
  JPopupMenu itsMenu;  
  int itsRay = DEFAULT_RAY;
  public final static int DEFAULT_RAY = 10;
}

