//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax;

import java.util.*;

import ircam.jmax.utils.*;
import ircam.jmax.*;
import ircam.jmax.script.*;

/**
  */

class MaxWhenHookTable
{
  class HookEntry
  {
    String name;
    Script code;

    HookEntry(String name, Script code)
    {
      this.name = name;
      this.code = code;
    }

    void runHook(String name, Interpreter interp)
    {
      if (this.name.equals(name))
	try
	{
	    Object result = code.eval();
	}
      catch (ScriptException e) {
	System.out.println("Interpreter error running hook " + name + " : " + e.getMessage());
      }
    }
  }

  MaxVector hookList = new MaxVector();

  MaxWhenHookTable()
  {
  }

  void addHook(String name, Script code)
  {
    hookList.addElement(new HookEntry(name, code));
  }

  void runHooks(String name)
  {
    int i;

    for (i = 0; i < hookList.size(); i++)
      {
	HookEntry e = (HookEntry) hookList.elementAt(i);

	e.runHook(name, MaxApplication.getInterpreter());
      }
  }
}
