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

import tcl.lang.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The explode extension; install the explode data type,
 * register the remote data class, installs
 * the explode editor factory
 */
public class ExplodeExtension extends tcl.lang.Extension
{

  /**
   * overrides tcl.lang.Extension.init()
   */
  public void init(Interp interp)
  {
    Mda.installEditorFactory( new ExplodeFactory());

    Fts.registerRemoteDataClass( "explode_data", ircam.jmax.editors.explode.ExplodeRemoteData.class);
  }

}



