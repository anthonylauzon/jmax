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
package ircam.jmax.toolkit;

import java.util.*;

/**
 * The interface of the objects that listen to ToolChange events
 * (for example, the ones rised by a toolbar)
 */
public interface ToolListener extends EventListener{
  /**
   * the single callback
   */
  abstract public void toolChanged(ToolChangeEvent newTool);

}
