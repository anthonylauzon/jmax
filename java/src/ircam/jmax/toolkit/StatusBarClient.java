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

import javax.swing.ImageIcon;

/**
 * The objects that use the StatusBar must implement this
 * interface */
public interface StatusBarClient {
  /**
   * The name that will be shown in the status bar. */
  abstract public String getName();
  /**
   * The icon that will be shown in the status bar.
   * Return null if this client has no icons. */
  abstract public ImageIcon getIcon();
}
