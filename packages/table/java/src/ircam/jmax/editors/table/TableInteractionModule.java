//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

/** The base class for the interaction modules used in Table.
 * It eliminates the need to cast the Tool's GraphicContext to
 * TableGraphicContext.
 * @see InteractionModule
 */
public class TableInteractionModule extends InteractionModule{

  /**
   * Returns the GraphicContext of this tool as TableGraphicContext */
  public TableGraphicContext getGc()
  {
    return (TableGraphicContext) gc;
  }
}
