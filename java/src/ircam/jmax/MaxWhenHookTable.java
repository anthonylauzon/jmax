//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax;

import tcl.lang.*;
import java.util.*;

import ircam.jmax.utils.*;
import ircam.jmax.*;

/**
  */

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
