/*
 * MaxInstallDataTypeCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */


package ircam.jmax.mda.tcl;


import tcl.lang.*;

import ircam.jmax.mda.*;

/**
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

	try
	  {
	    aDataType = (MaxDataType) Class.forName(argv[1].toString()).newInstance();
	  }
	catch (ClassNotFoundException e)
	  {
	    throw new TclException(interp,"Class not found for Data type " + argv[1].toString());
	  }
	catch (java.lang.InstantiationException e)
	  {
	    throw new TclException(interp, "Error installing Data type " +argv[1].toString());
	  }
	catch (java.lang.IllegalAccessException e)
	  {
	    throw new TclException(interp, "Illegal Access Error installing Data type " +argv[1].toString());
	  }

	MaxDataType.installDataType(aDataType);

	try
	  {
	    MaxDataEditorFactory aEditorFactory = (MaxDataEditorFactory) Class.forName(argv[2].toString()).newInstance();
	    aDataType.setDefaultEditorFactory(aEditorFactory);
	  }
	catch (ClassNotFoundException e)
	  {
	    throw new TclException(interp,"Class not found installing default editor " +
				   argv[2].toString() +
				   " for data type "  +
				   argv[1].toString());
	  }
	catch (java.lang.InstantiationException e)
	  {
	    throw new TclException(interp, "Error installing default editor " +
				   argv[2].toString() +
				   " for data type " +
				   argv[1].toString());
	  }
	catch (java.lang.IllegalAccessException e)
	  {
	    throw new TclException(interp, "Illegal Access Error installing default editor " +
				   argv[2].toString() +
				   " for data type " +
				   argv[1].toString());
	  }

	//everything is OK... proceed
	interp.setResult(ReflectObject.newInstance(interp, aDataType));
      }
    else
      throw new TclNumArgsException(interp, 2, argv, "data_type_name data_editor_name");
  }
}



