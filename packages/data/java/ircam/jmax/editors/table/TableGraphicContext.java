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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import javax.swing.*;

/**
 * The graphic context for a generic Table editing session.
 * It handles the data model, the Adapter.
 * @see GraphicContext
 * @see TableDataModel
 * @see TableAdapter
 */ 
public class TableGraphicContext extends GraphicContext{
  
  /**
   * Constructor */
  public TableGraphicContext(TableDataModel dm)
  {
    itsModel = dm;
    itsFtsObject = (FtsTableObject)dm;
    
    itsVerticalMaximum = itsFtsObject.isIvec() ? IVEC_DEFAULT_V_MAXIMUM : FVEC_DEFAULT_V_MAXIMUM*100;
    itsVerticalMinimum = itsFtsObject.isIvec() ? IVEC_DEFAULT_V_MINIMUM : FVEC_DEFAULT_V_MINIMUM*100;
  }

  /**
   * Redefined to switch the selection ownership */
  public void activate()
  {
    if (TableSelection.getCurrent() != itsSelection)
      TableSelection.setCurrent(itsSelection);
  }

  public boolean isIvec()
  {
    return itsFtsObject.isIvec();
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

  public double getVisibleVerticalScope()
  {
    return (itsAdapter.getInvY(0) - itsAdapter.getInvY(getGraphicDestination().getSize().height));
  }

  public boolean isHorizontalScrollbarVisible()
  {
    return ((TableDisplay)getGraphicDestination()).panel.itsHorizontalControl.isVisible();
  }
 
  public void scrollTo(int x)
  {
    ((TableDisplay)getGraphicDestination()).panel.itsHorizontalControl.setValue( x);
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

  float itsVerticalMaximum, itsVerticalMinimum;
  final public static float IVEC_DEFAULT_V_MAXIMUM = 135;
  final public static float IVEC_DEFAULT_V_MINIMUM = -135;
  final public static float FVEC_DEFAULT_V_MAXIMUM = 1;
  final public static float FVEC_DEFAULT_V_MINIMUM = -1;

  public float getVerticalMaximum()
  {
    return itsVerticalMaximum;
  }
  public float getVerticalMaxValue()
  {
    if( isIvec())
      return (float)itsVerticalMaximum;
    else
      return (float)(itsVerticalMaximum/100);
  }
  public void setVerticalMaximum(float max)
  {
    if( isIvec())
      itsVerticalMaximum = max;
    else
      itsVerticalMaximum = max*100;
  }
  public float getVerticalMinimum()
  {
    return itsVerticalMinimum;
  }
  public float getVerticalMinValue()
  {
    if( isIvec())
      return (float)itsVerticalMinimum;
    else
      return (float)(itsVerticalMinimum/100);
  }
  public void setVerticalMinimum(float min)
  {
    if( isIvec())
      itsVerticalMinimum = min;
    else
      itsVerticalMinimum = min*100;
  }

  public float getVerticalRange()
  {
    return (itsVerticalMaximum - itsVerticalMinimum);
  }
  /* display informations */

  public void display(String text)
  {
    if( display != null)
      display.setText(text);
  }

  public void displayInfo(String text)
  {
    if( info != null)
      info.setText(text);
  }
  public void setDisplay(JLabel label, JLabel infoLabel)
  {
    display = label;
    info = infoLabel;
  }

  /*public boolean isCompletelyUpdated()
  {
    return (getFtsObject().getLastUpdatedIndex() == getFtsObject().getSize());
  }*/
  
  JLabel display, info;
}

