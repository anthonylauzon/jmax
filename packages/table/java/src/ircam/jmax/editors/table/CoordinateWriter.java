//
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.table;

import javax.swing.*;

import ircam.jmax.toolkit.*;

/**
 * An utility class to write a 3D position in the status bar.
 * Every TableGraphicContext has its own CoordinateWriter, 
 * accessible via the getWriter() method.
 */
public class CoordinateWriter implements StatusBarClient {
  
  /** contructor */
  CoordinateWriter(TableGraphicContext tgc)
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


