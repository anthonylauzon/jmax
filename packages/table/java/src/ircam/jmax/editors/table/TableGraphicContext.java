
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

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
    itsWriter = new CoordinateWriter(this);
  }

  //--- Fields & accessors ---//

  TableDataModel itsModel;
  TableAdapter itsAdapter;
  CoordinateWriter itsWriter;

  public TableDataModel getDataModel()
  {
    return itsModel;
  }

  public void setDataModel(TableDataModel theModel)
  {
    itsModel = theModel;
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

}


