package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import javax.swing.event.*;

/**
 * The context for an explode editing session.
 * It stores the data model and other stuff related to Explode
 */
public class ExplodeGraphicContext extends GraphicContext {

  public ExplodeGraphicContext()
  {
    super();
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


  //---- Fields 


  ExplodeDataModel itsDataModel;

  Adapter itsAdapter;

  int itsLogicalTime;
}




