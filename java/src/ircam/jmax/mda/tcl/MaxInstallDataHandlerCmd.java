/*
 * MaxInstallDataHandlerCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */


package ircam.jmax.mda.tcl;

import tcl.lang.*;
import ircam.jmax.mda.*;

/**
 */

class MaxInstallDataHandlerCmd implements Command
{
  /**
   * This procedure is invoked to install new data handler for a given data type
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	MaxDataHandler aDataHandler;

	try
	  {
	    aDataHandler = (MaxDataHandler) Class.forName(argv[1].toString()).newInstance();
	  }
	catch (ClassNotFoundException e)
	  {
	    throw new TclException(interp, "Class not found for Data handler " +argv[1].toString());
	  }
	catch (java.lang.InstantiationException e)
	  {
	    throw new TclException(interp, "Error instantiating Data handler " +argv[1].toString());
	  }
	catch (java.lang.IllegalAccessException e)
	  {
	    throw new TclException(interp, "Illegal access Error instantiating Data handler " +argv[1].toString());
	  }

	MaxDataHandler.installDataHandler(aDataHandler);

	//everything is OK... proceed

	interp.setResult(ReflectObject.newInstance(interp, aDataHandler));
      }
    else
      throw new TclNumArgsException(interp, 2, argv, "data_type_name data_editor_name");
  }
}



