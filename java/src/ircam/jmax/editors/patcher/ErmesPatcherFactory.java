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
package ircam.jmax.editors.patcher;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The factory of patcher editors...
//
public class ErmesPatcherFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsPatcherData;
  }

  //
  // Creates a new instance of patcher editor starting from the MaxDocument to edit
  //

  public MaxDataEditor newEditor(MaxData theData) 
  {
    return new ErmesDataEditor( (FtsPatcherData)theData);
  }
}
