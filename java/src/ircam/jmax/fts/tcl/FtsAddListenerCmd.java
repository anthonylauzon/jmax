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
  // This is an TCL event class to ask for post-poned eval
  // of property updates in TCL scripts; ... may be usefull

  static class EvalEvent extends tcl.lang.TclEvent
  {
    Interp interp;
    TclObject script;

    EvalEvent(Interp i, TclObject s)
    {
      interp = i;
      script = s;
      script.preserve();
    }

    public int processEvent(int flags)
    {
      try
	{
	  interp.eval(script, 0);
	  script.release();
	}
      catch (TclException e)
	{
	  // Ignore errors in callback scripts
	}

      return 1;
    }
  }

  // This is an adapter class for FTS property that execute TCL code
  // it actually check to avoid executing the tcl code if the value
  // havn't changed; it is an hack that should be generalized
  // or the problem should be handled more generally elsewhere

  static class FtsListener implements FtsPropertyHandler
  {
    Object oldValue = null;
    String tclFunction;
    TclObject owner;

    public  FtsListener(FtsObject obj, String property, String tclFunction, TclObject owner)
    {
      obj.watch(property, this);
      this.tclFunction = tclFunction;
      this.owner = owner;
      owner.preserve();
    }

    public void finalize()
    {
      owner.release();
    }

    public void propertyChanged(FtsObject obj, String name, Object value)
    {
      // Don't call the script unless the value is really changed

      if ((oldValue != null) && oldValue.equals(value))
	return;
      else
	oldValue = value;

      //calls tclFunction with three arguments: the object, the property name,
      // and the new value

      Interp interp = MaxApplication.getTclInterp();

      try
	{
	  TclObject list = TclList.newInstance();

	  TclList.append(interp, list, TclString.newInstance(tclFunction));
	  TclList.append(interp, list, owner);
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

	  EvalEvent evt = new EvalEvent(interp, list);
	  interp.getNotifier().queueEvent(evt, TCL.QUEUE_TAIL);
	  // evt.sync(); // ???
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
    if (argv.length == 5)
      {
	FtsObject obj = (FtsObject) ReflectObject.get(interp, argv[1]);
	String property = argv[2].toString();
	String function = argv[3].toString();
	TclObject owner = argv[4];

	new FtsListener(obj, property, function, owner);
      }
    else
      {
      	throw new TclNumArgsException(interp, 1, argv, "<obj> <prop> <TCL function> <owner>");
      }
  }
}



