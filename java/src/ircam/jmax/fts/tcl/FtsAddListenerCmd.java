/*
 * FtsAddListenerCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.fts.tcl;


import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class implements the "addListener" TCL command, used to implement TCL/FTS callbacks.
 * WARNING: this is an experimental version.
 */
class FtsAddListenerCmd implements Command {

  /**
   * This procedure is invoked to execute a "addListener" operation in Ermes
   * the FtsObject <id> is "hooked" to a FtsListener to call when the object changes
   */
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc != 3) {	//addListener <objId> <TCL function name!>
      
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <objId> <TCL function>");
      
    }

    FtsServer currentServer =  MaxApplication.getFtsServer();
    FtsObject aFtsObject = currentServer.getObjectByFtsId(ca.intArg(1));
    FtsListener aFtsListener = new FtsListener((MaxInterp) interp, currentServer, ca.argv(2));

    aFtsObject.installPropertyHandler("value", aFtsListener);
    
    return "";
    
  }
}



/**
 * An utility class used to hook TCL functions to a changing value in FTS,
 * thus implementing a callback mechanism in TCL.
 * It is used in the "addListener" TCL command (JAddListenerCmd).
 */

class FtsListener implements FtsPropertyHandler {
  MaxInterp itsInterp;
  FtsServer itsServer;
  String itsTclFunction;

  public  FtsListener(MaxInterp theInterp, FtsServer theServer, String theTclFunction) {
    itsInterp = theInterp;
    itsServer = theServer;
    itsTclFunction = theTclFunction;
  }

  public void propertyChanged(String name, Object value) {
    //calls tclFunction with the (single) argument passed back by FTS
    
      try
	{
	  itsInterp.Eval(itsTclFunction + " " + value.toString());
	}
      catch (cornell.Jacl.EvalException e1)
	{
	  System.err.println("TCL Error in proc " + itsTclFunction + ":" + e1.info);
	}
  }
}
