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
package ircam.jmax.toolkit;

import java.awt.*;

/**
 * The set of functionalities offered by a statusbar.
 * @see InfoPanel
 */
public interface StatusBar {

  /** write a message in the status bar
   */
  abstract public void post(StatusBarClient theClient, String message);

  /**
   * returns the size on screen
   */
  abstract public Dimension getSize();

  /**
   * add a controller in the status bar */
  abstract public void addWidget(Component theWidget);

  /**
   * add a controller in the status bar in the given position*/
  abstract public void addWidgetAt(Component theWidget, int position);

  /**
   * remove a controller in the status bar */
  abstract public void removeWidget(Component theWidget);

}




