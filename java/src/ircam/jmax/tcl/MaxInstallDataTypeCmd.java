/*
 * MaxInstallDataTypeCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */


package ircam.jmax.tcl;


import java.io.*;
import java.util.*;
import tcl.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The "systemProperty" TCL command.
 */

class MaxInstallDataTypeCmd implements Command
{
  /**
   * This procedure is invoked to install new data types & default factories in jMax
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	MaxDataType aDataType;
	try {
	  aDataType = (MaxDataType) Class.forName(argv[1].toString()).newInstance();
	} catch (Exception e) {
	  System.out.println("error installing Data type " +argv[1].toString());
	  throw new TclException(1);
	}
	MaxDataType.installDataType(aDataType);
	try {
	  MaxDataEditorFactory aEditorFactory = (MaxDataEditorFactory) Class.forName(argv[2].toString()).newInstance();
	} catch (Exception e) {
	  System.out.println("error installing default editor " +argv[2].toString()+ " for data type "+argv[1].toString());
	  throw new TclException(1);
	}
	//everything is OK... proceed
	interp.setResult(ReflectObject.newInstance(interp, aDataType));
      }
    else
      throw new TclNumArgsException(interp, 2, argv, "data_type_name data_editor_name");
  }
}



