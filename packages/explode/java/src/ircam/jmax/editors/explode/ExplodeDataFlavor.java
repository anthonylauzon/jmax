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

import java.awt.datatransfer.*;

/**
 * The clipboard flavor representing a selection of ScrEvent.
 * This is the only flavor supported by the Explode editor
 * for copy and paste operations (for now)
 */
class ExplodeDataFlavor extends DataFlavor {
  
  ExplodeDataFlavor(Class representation, String name)
  {
    super(representation, name);
  }

  static ExplodeDataFlavor getInstance()
  {
    if (sharedInstance == null)
      sharedInstance = new ExplodeDataFlavor(ircam.jmax.editors.explode.ExplodeSelection.class, "ExplodeSelection"); 
    return sharedInstance;
  }

  private static ExplodeDataFlavor sharedInstance;
}



