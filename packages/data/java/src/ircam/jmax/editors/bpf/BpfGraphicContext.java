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

package ircam.jmax.editors.bpf;

import ircam.jmax.editors.bpf.renderers.*; 
import ircam.jmax.toolkit.*;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * The context for an sequence editing session.
 * It stores the data model and other stuff related to Sequence
 */
public class BpfGraphicContext extends GraphicContext {

  /**
   * Constructor */
  public BpfGraphicContext(BpfDataModel model)
  {
      super();
      setDataModel(model);
      itsSelection = new BpfSelection(model, this);
  }

  /**
   * Returns the lenght (in msec) rapresented in the current window */
  public int getTimeWindow()
  {
    return (int)((itsGraphicDestination.getSize().width - BpfAdapter.DX)/
		 (itsAdapter.getGeometry().getXZoom()));
  }


  /**
   * sets the adapter to be used in this context
   */
  public void setAdapter(BpfAdapter theAdapter) 
  {
    itsAdapter = theAdapter;
  }

  /**
   * gets the current adapter
   */
  public BpfAdapter getAdapter() 
  {
    return itsAdapter;
  }

  /**
   * sets the DataModel of this context
   */
  public void setDataModel(BpfDataModel theDataModel) 
  {
    itsDataModel = theDataModel;
  }

  /**
   * returns the data model
   */
  public BpfDataModel getDataModel() {
    return itsDataModel;
  }

  /**
   * returns the current logical time
   */
  public int getLogicalTime() 
  {
    return -itsAdapter.getGeometry().getXTransposition();
  }

    public int getMaximumTime()
    {
	if(scrollManager.isScrollbarVisible())
	    return scrollManager.getMaximumTime();
	else
	    return (int)itsAdapter.getInvX(getGraphicDestination().getSize().width);
    }
    /*
     * returns visible rectangle of the associated track editor
     */
   public Rectangle getEditorClip()
    {
	return new Rectangle(0, 0, ((Bpf)getFrame()).getViewRectangle().width - 2, 
			     getGraphicDestination().getSize().height);
    }

  public void setToolManager(BpfToolManager t)
  {
    toolManager = t;
  }

  public BpfToolManager getToolManager()
  {
    return toolManager;
  }

  public void setSelection(BpfSelection s)
  {
    itsSelection = s;
  }

  public BpfSelection getSelection()
  {
    return itsSelection;
  }

  public FtsBpfObject getFtsObject()
  {
      //return ((Bpf) getFrame()).bpfData;
      return (FtsBpfObject)itsDataModel;
  }

    public void setScrollManager(ScrollManager manager)
    {
	scrollManager = manager;
    }
    public ScrollManager getScrollManager()
    {
	return scrollManager;
    }

    public void display(String text)
    {
	display.setText(text);
    }
    public void displayInfo(String text)
    {
	info.setText(text);
    }
    public void displaySelectionInfo()
    {
	if(getSelection().size()==0)
	    info.setText("");
	else
	    {		
		BpfPoint first = getSelection().getFirstInSelection();
		BpfPoint last = getSelection().getLastInSelection();
		BpfPoint prev = getFtsObject().getPreviousPoint(first);

		String text = "("+PointRenderer.numberFormat.format(first.getTime())+" , "+
		    PointRenderer.numberFormat.format(first.getValue())+" )  ";
		
		if(getSelection().size() > 1)
		    text = text+"["+PointRenderer.numberFormat.format(last.getTime()-first.getTime())+"]";

		if(prev!=null)
		    text = PointRenderer.numberFormat.format(first.getTime()-prev.getTime())+" --> "+text;

		info.setText(text);
	    }
    }

    public void setDisplay(JLabel displayLabel, JLabel infoLabel)
    {
	display = displayLabel;
	info = infoLabel;
    }
  //---- Fields 
    
    BpfDataModel itsDataModel;

    BpfSelection itsSelection;

    BpfAdapter itsAdapter;

    int itsLogicalTime;

    int maximumTime;

    BpfToolManager toolManager;

    ScrollManager scrollManager;
    
    JLabel display, info;
}




