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
import ircam.jmax.toolkit.*;

public class PatcherPrintManager
{
  static private PatcherPrintManager printManager = new PatcherPrintManager();

  /****************************************************************************/
  /*                                                                          */
  /*           ACTIONS                                                        */
  /*                                                                          */
  /****************************************************************************/
  
  static public void Print(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    RepaintManager.currentManager(sketch).setDoubleBufferingEnabled(false);

    PrintJob aPrintjob = container.getFrame().getToolkit().getPrintJob( container.getFrame(), "Printing Patcher", MaxApplication.getProperties());

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








