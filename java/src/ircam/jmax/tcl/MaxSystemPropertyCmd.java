/*
 * MaxSystemPropertyCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */


package ircam.jmax.tcl;


import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import cornell.Jacl.*;

/**
 * The "systemProperty" TCL command.
 */

class MaxSystemPropertyCmd implements Command {

  
  
  /**
    
   * This procedure is invoked to get a property

   */
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc < 2) {	//systemProperty <property name>
      
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <property name>");
      
    }

    String property = MaxApplication.ermesProperties.getProperty(ca.argv(1));

    /* If the property is not defined, don't raise an error, return the empty string, 
       so a TCL script can test the existence of a systemProperty */

    if (property == null) 
      return "";
    else
      return property;
    
  }

}



