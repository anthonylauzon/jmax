package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * The context for an explode editing session.
 * it stores the source of graphic events, the graphic destination for 
 * the drawing operations, the selection, the adapter, the data model..
 */
public class GraphicContext {

  /**
   * sets the source of events that is, the components that generates
   * the awt events.
   */
  public void setGraphicSource(Component theGraphicSource) 
  {
    itsGraphicSource = theGraphicSource;
  }


  /**
   * gets the source
   */
  public Component getGraphicSource() 
  {
    return itsGraphicSource;
  }


  /**
   * set the destination, that is the component that will receive the
   * graphic (drawing) operations
   */
  public void setGraphicDestination(Component theGraphicDestination) 
  {
    itsGraphicDestination = theGraphicDestination;
  }

  /**
   * get the destination
   */
  public Component getGraphicDestination() 
  {
    return itsGraphicDestination;
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
   * set the Renderer to be used in this context
   */
  public void setRenderer(Renderer theRenderer) 
  {
    itsRenderer = theRenderer;
  }
  
  /**
   * gets the Renderer
   */
  public Renderer getRenderer() 
  {
    return itsRenderer;
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

  /**
   * set the logical time
   */
  public void setStatusBar(StatusBar theStatusBar) 
  {
    itsStatusBar = theStatusBar;
  }

  /**
   * returns the current logical time
   */
  public StatusBar getStatusBar() 
  {
    return itsStatusBar;
  }

  public void setFrame(Frame f) 
  {
    itsFrame = f;
  }

  public Frame getFrame() 
  {
    if (itsFrame != null) 
      return itsFrame;
    else return getFrame(itsGraphicDestination);
  }

  /**
   * utility function:
   * returns the Frame containing a component (why it's not in AWT?)
   */
  public static Frame getFrame(Component c) {
    for (Container p = c.getParent(); p != null; p = p.getParent()) {
      if (p instanceof Frame) {
	return (Frame) p;
      }
    }
    return null;
  }

  //-------------------------------------

  Component itsGraphicSource;
  Component itsGraphicDestination;
  Adapter itsAdapter;

  ExplodeDataModel itsDataModel;

  Renderer itsRenderer;
  Frame itsFrame;
  int itsLogicalTime;
  StatusBar itsStatusBar;
}
