//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.console;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
/**
  the thread associated with a tcl console
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

class ConsoleThread extends Thread {
  Console itsConsole;

  public ConsoleThread(Console theConsole) {
    super("Console Thread");
    itsConsole = theConsole;
    /*
     * The console thread runs as a daemon so that it gets terminated 
     * automatically when all other user threads are terminated.
     */
    setDaemon(true);
  }
  
  /*
   * The AppletConsole thread loops waiting for notification from the
   * ConsoleKeyListener object, via the LineReadyNotify method.
   */
  
  public synchronized void run() {
    /* itsConsole.PutLine("\n  **** jmax console/ TCL interpreter started ****"); */
    /* itsConsole.Put("\n"); */
    while (true) {
      // try {
	//wait();
      suspend();
      itsConsole.ProcessLine();
	//}
	//catch (InterruptedException e) {
	//System.out.println("AppletConsole: wait error");		
	//}
    }
  }  
}

