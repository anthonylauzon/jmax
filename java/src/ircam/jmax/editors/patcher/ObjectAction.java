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

import ircam.jmax.editors.patcher.objects.*;

/** An interface for actions intended to be mapped on objects sets.
  Work on selections and on the whole displayList 
  */

public interface ObjectAction
{
  public void processObject(GraphicObject object);
}
