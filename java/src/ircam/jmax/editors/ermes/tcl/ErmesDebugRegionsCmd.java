/*
 * NewObjCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;


class ErmesDebugRegionsCmd implements Command
{
    /**
     * This procedure is invoked to debug the regions in java
     */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
	ErmesSketchPad pad;

	/* SOrry, itsSketchWindowList do not exist any more */
       
	pad = ((ErmesSketchWindow)MaxApplication.getCurrentWindow()).itsSketchPad;
	if (argv.length > 1) {
	  System.err.println("Element region:");
	  pad.GetElementRegion().writeRegionOnErrorStream();
	  System.err.println("HSegmRgn region:");	  
	  pad.GetHSegmRgn().writeRegionOnErrorStream();
	  System.err.println("VSegmRgn region:");	  
	  pad.GetVSegmRgn().writeRegionOnErrorStream();
	}
	pad.paintAllRegions();
  }
}





