//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.table.renderers;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;
import java.awt.*;

/** 
 * The "decoration layer" of the table editor (only the gray area for now) */
public class TopLayer extends AbstractLayer {

  /**
   * Constructor */
  public TopLayer(TableGraphicContext theTgc)
  {
    tgc = theTgc;
  }

  /**
   * Layer interface. */
  public void render(Graphics g, int order)
  {
      TableAdapter ta = tgc.getAdapter();
      int middleY = ta.getY(0);
      int end = ta.getX(tgc.getDataModel().getSize());

      //vertical black line at the end of the table area
      g.setColor(Color.black);
      g.drawLine(end, 0, end, tgc.getGraphicDestination().getSize().height );
      

      //unused vertical area
      g.setColor(Color.lightGray);
      g.fillRect(end+1, 0, tgc.getGraphicDestination().getSize().width-end, tgc.getGraphicDestination().getSize().height);
  }


  //--- Fields
  TableGraphicContext tgc;
}
