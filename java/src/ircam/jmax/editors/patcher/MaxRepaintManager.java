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

package ircam.jmax.editors.patcher;

import javax.swing.*; 
import java.awt.*; 

public class MaxRepaintManager extends RepaintManager
{
  public void paintDirtyRegions()
  {
    super.paintDirtyRegions();
    Toolkit.getDefaultToolkit().sync();
  }
}
