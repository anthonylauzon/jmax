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

package ircam.jmax.toolkit;

import java.util.*;

/**
 * the interface of the objects that can provide tools to a toolbar.
 */
public interface ToolbarProvider {

  /**
   * returns the collection of tools to be inserted in the toolbar
   */
  abstract public Enumeration getTools();

  /**
   * returns the tool that should be used as the default, if any
   */
  abstract public Tool getDefaultTool();

  /**
   * returns the graphic context of this provider
   */
  abstract GraphicContext getGraphicContext();
}

