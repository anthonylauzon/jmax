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




