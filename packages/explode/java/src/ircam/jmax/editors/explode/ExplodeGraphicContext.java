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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import javax.swing.event.*;

/**
 * The context for an explode editing session.
 * It stores the data model and other stuff related to Explode
 */
public class ExplodeGraphicContext extends GraphicContext {

  /**
   * Constructor */
  public ExplodeGraphicContext(ExplodeDataModel model, ExplodeSelection s)
  {
    super();
    setDataModel(model);
    itsSelection = s;
  }

  /**
   * Change the selection ownership when this graphic context become active */
  public void activate()
  {
    ExplodeSelection.setCurrent(itsSelection);
  }

  /**
   * Returns the lenght (in msec) rapresented in the current window */
  public int getTimeWindow()
  {
    return (int)(itsGraphicDestination.getSize().width/
			   (itsAdapter.getXZoom()));
  }


  /**
   * sets the adapter to be used in this context
   */
  public void setAdapter(Adapter theAdapter) 
  {
    itsAdapter = theAdapter;
  }

  /**
   * gets the current adapter
   */
  public Adapter getAdapter() 
  {
    return itsAdapter;
  }

  /**
   * sets the DataModel of this context
   */
  public void setDataModel(ExplodeDataModel theDataModel) 
  {
    itsDataModel = theDataModel;
  }

  /**
   * returns the data model
   */
  public ExplodeDataModel getDataModel() {
    return itsDataModel;
  }

  /**
   * set the logical time
   */
  public void setLogicalTime(int theTime) 
  {
    itsLogicalTime = theTime;
  }

  /**
   * returns the current logical time
   */
  public int getLogicalTime() 
  {
    return itsLogicalTime;
  }

  public void setToolbar(EditorToolbar t)
  {
    toolbar = t;
  }

  public EditorToolbar getToolbar()
  {
    return toolbar;
  }

  public void setSelection(ExplodeSelection s)
  {
    itsSelection = s;
  }

  public ExplodeSelection getSelection()
  {
    return itsSelection;
  }

  //---- Fields 

  ExplodeDataModel itsDataModel;

  ExplodeSelection itsSelection;

  Adapter itsAdapter;

  int itsLogicalTime;

  EditorToolbar toolbar;
}




