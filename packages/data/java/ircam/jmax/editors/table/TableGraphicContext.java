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
import javax.swing.*;

/**
 * The graphic context for a generic Table editing session.
 * It handles the data model, the Adapter, the CoordinateWriter.
 * @see GraphicContext
 * @see TableDataModel
 * @see TableAdapter
 * @see CoordinateWriter
 */ 
public class TableGraphicContext extends GraphicContext{
  
  /**
   * Constructor */
  public TableGraphicContext(TableDataModel dm)
  {
    itsModel = dm;
    itsFtsObject = (FtsTableObject)dm;
    /*itsWriter = new CoordinateWriter(this);*/
  }


  /**
   * Redefined to switch the selection ownership */
  public void activate()
  {
    if (TableSelection.getCurrent() != itsSelection)
      TableSelection.setCurrent(itsSelection);
  }


  /*
   * redefined to handle selection ownership when
   * the owner is closed (bug #171) */
  public void destroy()
  {
    TableSelection.ownerDied(itsSelection.getOwner());
  }

  //--- Fields & accessors ---//

  TableDataModel itsModel;
  FtsTableObject itsFtsObject;
  TableAdapter itsAdapter;
  CoordinateWriter itsWriter;
  //EditorToolbar toolbar;
  TableSelection itsSelection;
  TableToolManager toolManager;

  public TableDataModel getDataModel()
  {
    return itsModel;
  }
  public void setDataModel(TableDataModel theModel)
  {
    itsModel = theModel;
  }

  public FtsTableObject getFtsObject()
  {
    return itsFtsObject;
  }
  public void setFtsObject(FtsTableObject obj)
  {
    itsFtsObject = obj;
  }

  public TableAdapter getAdapter()
  {
    return itsAdapter;
  }

  public void setAdapter(TableAdapter theAdapter)
  {
    itsAdapter = theAdapter;
  }

  public CoordinateWriter getCoordWriter()
  {
    return itsWriter;
  }
  
  public void setCoordWriter(CoordinateWriter cw)
  {
    itsWriter = cw;
  }

  /*public void setToolbar(EditorToolbar t)
    {
    toolbar = t;
    }

    public EditorToolbar getToolbar()
    {
    return toolbar;
    }*/

  public void setToolManager( TableToolManager tm)
  {
    toolManager = tm;
  }
  public TableToolManager getToolManager()
  {
    return toolManager;
  }

  public void setSelection(TableSelection t)
  {
    itsSelection = t;
  }

  public TableSelection getSelection()
  {
    return itsSelection;
  }

  ////////////////////////////////////////////////////
  public int getFirstVisibleIndex()
  {
      return itsAdapter.getInvX(0);
  }
  public int getLastVisibleIndex()
  {      
      int last = itsAdapter.getInvX(getGraphicDestination().getSize().width);
      if((last > getFtsObject().getSize())&&( getFtsObject().getSize()!=0)) last = getFtsObject().getSize();
      return last;
  }
  public int getLastVisibleIndex(int newSize)
  {      
      int last = itsAdapter.getInvX(getGraphicDestination().getSize().width);
      if((last > newSize)&&( newSize!=0)) last = newSize;
      return last;
  } 
  public int getWindowHorizontalScope()
  {
      return (itsAdapter.getInvX(getGraphicDestination().getSize().width) - itsAdapter.getInvX(0));
  }
  
  public int getVisibleHorizontalScope()
  {
      return getLastVisibleIndex() - getFirstVisibleIndex() + 1;
  }

  public int getVisibleVerticalScope()
  {
    //return itsAdapter.getInvY(0) - itsAdapter.getInvY(getGraphicDestination().getSize().height);
    return itsAdapter.getInvY(getGraphicDestination().getSize().height) - itsAdapter.getInvY(0);
  }

  public int getVisiblePixelsSize()
  {
      int pixsize;
      int last = getLastVisibleIndex();

      if(last == getFtsObject().getSize())
	  pixsize = itsAdapter.getX(last);
      else 
	  pixsize = getGraphicDestination().getSize().width;	  
      return pixsize;
  }

  int itsVerticalMaximum = DEFAULT_V_MAXIMUM;
  final public static int DEFAULT_V_MAXIMUM = 135;

  public int getVerticalMaximum()
  {
      return itsVerticalMaximum;
  }
  public void setVerticalMaximum(int max)
  {
      itsVerticalMaximum = max;
  }

  /* display informations */

  public void display(String text)
  {
    display.setText(text);
  }

  public void displayInfo(String text)
  {
    info.setText(text);
  }
  public void setDisplay(JLabel label, JLabel infoLabel)
  {
    display = label;
    info = infoLabel;
  }

  JLabel display, info;
}








