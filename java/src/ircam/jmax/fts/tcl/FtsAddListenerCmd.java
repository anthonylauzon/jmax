/*
 * FtsAddListenerCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.fts.tcl;


import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class implements the "addListener" TCL command, used to implement TCL/FTS callbacks.
 * WARNING: this is an experimental version.
 */

class FtsAddListenerCmd implements Command
{
  static class FtsListener implements FtsPropertyHandler
  {
    String tclFunction;

    public  FtsListener(FtsObject obj, String property, String tclFunction)
    {
      obj.watch(property, this);
      this.tclFunction = tclFunction;
    }

    public void propertyChanged(FtsObject obj, String name, Object value)
    {
      //calls tclFunction with three arguments: the object, the property name,
      // and the new value

      Interp interp = MaxApplication.getTclInterp();

      try
	{
	  TclObject list = TclList.newInstance();

	  TclList.append(interp, list, TclString.newInstance(tclFunction));
	  TclList.append(interp, list, ReflectObject.newInstance(interp, obj));
	  TclList.append(interp, list, TclString.newInstance(name));
	  if (value instanceof Integer)
	    TclList.append(interp, list, TclInteger.newInstance(((Integer)value).intValue()));
	  else if (value instanceof Float)
	    TclList.append(interp, list, TclDouble.newInstance(((Float)value).doubleValue()));
	  else if (value instanceof String)
	    TclList.append(interp, list, TclString.newInstance((String)value));
	  else 
	    TclList.append(interp, list, ReflectObject.newInstance(interp, value));


	  interp.eval(list, 0);
	}
      catch (TclException e1)
	{
	  System.out.println("TCL Error in proc " + tclFunction + ":" + interp.getResult());
	}
    }
  }
  
  /**
   * This procedure is invoked to execute a "addListener" operation in Ermes
   * the FtsObject <id> is "hooked" to a FtsListener to call when the object changes
   * 
   * addListener <obj>  <property> <TCL function>
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 4)
      {
	FtsObject obj = (FtsObject) ReflectObject.get(interp, argv[1]);
	String property = new String(argv[2].toString());
	String function = new String(argv[3].toString());

	new FtsListener(obj, property, function);
      }
    else
      {
      	throw new TclNumArgsException(interp, 1, argv, "<objId> <TCL function>");
      }
  }
}


