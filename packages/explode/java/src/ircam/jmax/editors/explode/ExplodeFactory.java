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
package ircam.jmax.editors.explode;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;

/**
 * the MaxDataEditorFactory specialized to build explode editors
 */
public class ExplodeFactory implements MaxDataEditorFactory {

  /**
   * overrides MaxDataEditorFactory.canEdit()
   */
  public boolean canEdit( MaxData maxData)
  {
    return maxData instanceof ExplodeRemoteData;
  }
  
  /**
   * overrides MaxDataEditorFactory.newEditor()
   */
  public MaxDataEditor newEditor( MaxData maxData) 
  {
    return new ExplodeDataEditor((ExplodeRemoteData) maxData);
  }
}




