/**
  A registration service for .tcl templates.
  The class is used thru the Template TCL Command.
  */

package ircam.jmax;

import tcl.lang.*;
import java.util.*;
import ircam.jmax.*;

class MaxWhenHookTable
{
  class HookEntry
  {
    String name;
    String code;

    HookEntry(String name, String code)
    {
      this.name = name;
      this.code = code;
    }

    void runHook(String name, Interp interp)
    {
      if (this.name.equals(name))
	try
	{
	  System.err.println("runHook: eval of :>>" + code + "<<");
	  interp.eval(code);
	}
      catch (TclException e) {
	MaxApplication.getPostStream().println("TCL error running hook " + name + " : " + e);
      }
    }
  }

  Vector hookList = new Vector();

  MaxWhenHookTable()
  {
  }

  void addHook(String name, String code)
  {
    hookList.addElement(new HookEntry(name, code));
  }

  void runHooks(String name)
  {
    int i;

    for (i = 0; i < hookList.size(); i++)
      {
	HookEntry e = (HookEntry) hookList.elementAt(i);

	e.runHook(name, MaxApplication.getTclInterp());
      }
  }
}
