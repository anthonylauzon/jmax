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

package ircam.jmax.editors.table;

import javax.swing.*;

import ircam.jmax.toolkit.*;

/**
 * An utility class to write a 2D position in the status bar.
 * Every TableGraphicContext has its own CoordinateWriter, 
 * accessible via the getWriter() method.
 */
public class CoordinateWriter implements StatusBarClient {
  
  /** contructor */
  public CoordinateWriter(TableGraphicContext tgc)
  {
    this.tgc = tgc;
  }

  /** makes the coordinates conversion an write the coordinates in the
   * status bar */
  public void postCoordinates(int x, int y)
  {
      int index = tgc.getAdapter().getInvX(x);
      int value = tgc.getAdapter().getInvY(y);
      
      if (index < tgc.getDataModel().getSize() &&
	  index >=0)
	{
	  tgc.getStatusBar().post(this, tgc.getAdapter().getInvX(x)+" "+tgc.getAdapter().getInvY(y));
	}
  }
  
  /** StatusBarClient interface */
  public String getName()
  {
    return "coord.";
  }
  
  /** StatusBarClient interface */

  public ImageIcon getIcon()
  {
    return null;
  }

  //--- Fields
  TableGraphicContext tgc;
}


