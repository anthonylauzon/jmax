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
  public void setGraphicEventSource(Component theGraphicEventSource) 
  {
    itsGraphicEventSource = theGraphicEventSource;
  }


  /**
   * gets the source
   */
  public Component getGraphicEventSource() 
  {
    return itsGraphicEventSource;
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
   * set the SelectionHandler associated to this context
   */
  public void setSelection(SelectionHandler theSelection) 
  {
    itsSelection = theSelection;
  }

  
  /**
   * returns the Selection (handler) 
   */
  public SelectionHandler getSelection() 
  {
    return itsSelection;
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
   * set the current tool
   */
  public void setTool(ScrTool theTool) 
  {
    itsTool = theTool;
  }

  /**
   * returns the current tool
   */
  public ScrTool getTool() 
  {
    return itsTool;
  }

  //-------------------------------------
  Component itsGraphicEventSource;
  Component itsGraphicDestination;
  Adapter itsAdapter;
  SelectionHandler itsSelection;
  ScrTool itsTool;
  ExplodeDataModel itsDataModel;

  Renderer itsRenderer;

}
