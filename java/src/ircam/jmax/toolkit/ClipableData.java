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

import java.awt.datatransfer.*;

/**
 * The interface of the data (models) that supports clipboard operations */
public interface ClipableData {

  /**
   * ask the model to cut the selected data */
  public abstract void cut();

  /**
   * ask the model to copy the selected elements */
  public abstract void copy();

  /**
   * ask the model to paste the content of the clipboard */
  public abstract void paste();

}

