package ircam.jmax.editors.patcher;

import java.awt.*; 
import javax.swing.*; 

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import ircam.jmax.editors.patcher.tcl.*;

/** The ermes module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class ErmesModule
{
  static public void initModule()
  {
    // Optionally set the syncPaint flag

    if (MaxApplication.getProperty("syncPaint") != null)
      ErmesSketchPad.setSyncPaint(true);

    // Install the customized Repaint Manager

    RepaintManager.setCurrentManager(new MaxRepaintManager());

    // Install the local mda entities

    Mda.installEditorFactory(new ErmesPatcherFactory());

    // Register the find panel and the fpe panel

    FindPanel.registerFindPanel();
    FpePanel.registerFpePanel();

    // Install the tcl commands

    TclErmesPackage.installPackage();
  }
}



