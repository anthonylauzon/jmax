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
package ircam.jmax.dialogs;

import ircam.jmax.*;

/** The Dialog module; the init function is called at init time
 *  by jmax, and it is used to preload some dialogs at init time;
 * Remember that since a static initializer is executed when the class
 * is first loaded, and a class is loaded when the first instance
 * is built, a static initilizer cannot be used to preload things.
 */

public class DialogsModule
{
  static public void initModule()
  {
    MaxFileChooser.makeFileChooser(); // prebuild the file chooser.
  }
}
