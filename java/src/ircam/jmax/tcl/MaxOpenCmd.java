/*
 * MaxOpenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.editors.project.*;


/**

 * The generic "Open <filename>" TCL command.

 */

class MaxOpenCmd implements Command {

  
  
  /**
    
   * This procedure is invoked to execute a "Open" operation in Ermes
   * It calls the generic OpenFile procedure of the project manager
   * It is able to load everything that can be opened by the "open" menu
   */
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc < 2) {	//open <filename>
      
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <filename>");
      
    }

    ProjectWindow aProjectWindow = MaxApplication.getApplication().itsProjectWindow;
    String pathName, fileName;
    String fileSeparator = System.getProperty("file.separator");
    int lastSeparatorIndex = ca.argv(1).lastIndexOf(fileSeparator);
    if (lastSeparatorIndex == -1) {
      pathName = "./";
      fileName = ca.argv(1);
    }
    else {
      fileName = ca.argv(1).substring(lastSeparatorIndex+1);
      pathName = ca.argv(1).substring(0, lastSeparatorIndex)+"/";
    }
    System.out.println("opening "+pathName+fileName);
    aProjectWindow.OpenFile(fileName, pathName);
    return "";
  }

}



