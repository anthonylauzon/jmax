/**
  */

package ircam.jmax;

import tcl.lang.*;
import java.util.*;

import ircam.jmax.utils.*;
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
	  interp.eval(code);
	}
      catch (TclException e) {
	System.out.println("TCL error running hook " + name + " : " + interp.getResult());
      }
    }
  }

  MaxVector hookList = new MaxVector();

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
