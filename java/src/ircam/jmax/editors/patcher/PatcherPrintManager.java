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
package ircam.jmax.editors.patcher;

import java.awt.*;
import javax.swing.*;

import ircam.jmax.*;

public class PatcherPrintManager
{
  static private PatcherPrintManager printManager = new PatcherPrintManager();

  /****************************************************************************/
  /*                                                                          */
  /*           ACTIONS                                                        */
  /*                                                                          */
  /****************************************************************************/
  
  static public void Print(ErmesSketchWindow sketchWindow)
  {
    ErmesSketchPad sketch = sketchWindow.getSketchPad();
    
    RepaintManager.currentManager(sketch).setDoubleBufferingEnabled(false);

    PrintJob aPrintjob = sketchWindow.getToolkit().getPrintJob( sketchWindow, "Printing Patcher", MaxApplication.getProperties());

    if (aPrintjob != null)
      {
	Graphics aPrintGraphics = aPrintjob.getGraphics();

	if (aPrintGraphics != null)
	  {
	    sketch.print( aPrintGraphics);
	    aPrintGraphics.dispose();
	  }
	aPrintjob.end();
      }

    RepaintManager.currentManager(sketch).setDoubleBufferingEnabled(true);
  }
}








