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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.renderers.*;
import ircam.jmax.editors.table.menus.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A JPanel that is able to show the content of a TableDataModel.*/
public class ScalePanel extends JPanel
{

    /**
     * Constructor. */
    public ScalePanel(TableGraphicContext gc)
    {
	this.gc = gc;
	addComponentListener( new ComponentAdapter() {
		public void componentResized(ComponentEvent e)
		{
		    updateScale();
		}
	    });
      
	gc.getAdapter().addYZoomListener(new ZoomListener() {
		public void zoomChanged(float zoom, float oldZoom) 
		{
		  updateScale();
		}
	    });
      
	setSize(scaleDimension);
    }

    public void updateScale()
    {
	computeScaleParameters();
    }
    
    public void paintComponent(Graphics g)
    {
	if (step == 0 || Math.abs(startValue) > 36535) 
	    return; //"emergency exit!!!
	  
	TableAdapter ta = gc.getAdapter();
	g.setColor(violetColor);
	g.setFont(scalePanelFont);
      
	for (int i = startValue; i > ta.getInvY(gc.getGraphicDestination().getSize().height); i-= step)
	    {
		g.drawString(""+i, 1, ta.getY(i));
		g.drawLine(getWidth()-5, ta.getY(i), getWidth(), ta.getY(i));
	    }
    }
    
    /** The main computing routine.
     * It is called when the window is resized, scrolled or zoomed*/
    void computeScaleParameters()
    {
	TableAdapter ta = gc.getAdapter();
	int range = gc.getVisibleVerticalScope();
	
	if (range < 10) 
	    step = 5;
	else if (range <50)
	    step = 10;
	else if (range <100)
	    step = 25;
	else if (range <500)
	    step = 25;
	else if (range <1000)
	    step = 50; 
	else if (range <2000)
	    step = 100;
	else if (range <4000)
	    step = 200;
	else if (range <12000)
	    step = 500;
	else if (range <20000)
	    step = 1000;
	else if (range <50000)
	    step = 2000;
	else 
	    {
		startValue = (ta.getInvY(0)/5000)*5000; 
		step = 5000;
	    }
      
	startValue = (ta.getInvY(0)/step)*step;      
    }  

    public Dimension getMinimumSize()
    {
	return scaleDimension;
    }
    public Dimension getPreferredSize()
    {
	return scaleDimension;
    }
    //--- Fields
    int startValue = 0;
    int step = 0;
    Font scalePanelFont = new Font(getFont().getName(), Font.BOLD, 10);
    Color violetColor = new Color(102,102,153);
    static Dimension scaleDimension = new Dimension(30, 200);
    TableGraphicContext gc;
}

