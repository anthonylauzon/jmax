//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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
    //itsWriter = new CoordinateWriter(this);
  }


  /**
   * Redefined to switch the selection ownership */
  public void activate()
  {
    if (TableSelection.getCurrent() != itsSelection)
      TableSelection.setCurrent(itsSelection);
  }

  //--- Fields & accessors ---//

  TableDataModel itsModel;
  TableAdapter itsAdapter;
  CoordinateWriter itsWriter;
  EditorToolbar toolbar;
  TableSelection itsSelection;

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

  public void setToolbar(EditorToolbar t)
  {
    toolbar = t;
  }

  public EditorToolbar getToolbar()
  {
    return toolbar;
  }

  public void setSelection(TableSelection t)
  {
    itsSelection = t;
  }

  public TableSelection getSelection()
  {
    return itsSelection;
  }
}


