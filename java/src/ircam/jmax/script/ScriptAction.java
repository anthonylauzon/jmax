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

package ircam.jmax.script;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class ScriptAction extends EditorAction
{
    private Script itsScript;
    private String itsName;
    private int itsModifiers;
    private int itsMnemonic;

    public ScriptAction(Script script, String name,  int modifiers, int mnemonic) {
	itsScript = script;
	itsName = name;
	itsModifiers = modifiers;
	itsMnemonic = mnemonic;
    }

    public String getName() {
	return itsName;
    }

    public int getModifiers() {
	return itsModifiers;
    }

    public int getMnemonic() {
	return itsMnemonic;
    }

    public void doAction(EditorContainer container)
    {
	final EditorContainer theContainer = container;
	/* We have to spin of a new thread otherwise the script will
           be evaluated in the awt thread */
	new Thread(new Runnable() {
	    public void run() {
		try {
		    itsScript.eval(theContainer);
		} catch (Exception e) {
		    System.out.println("Dear user, please contact the author of the script " + itsName);
		    System.out.println(itsName + " caused an error: " + e.getMessage());
		}}}, "Script Thread").start();
    }
}

