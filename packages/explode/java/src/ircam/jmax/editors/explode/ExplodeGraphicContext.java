//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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




