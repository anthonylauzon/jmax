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
import java.util.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

/** Implement the editor Script Menu */

public class ScriptMenu extends EditorMenu
{
    public ScriptMenu()
    {
	super("Script");
	setHorizontalTextPosition(AbstractButton.LEFT);

	int size = MaxDocumentType.countGlobalScripts();
	for (int i = 0; i < size; i++) {
	    ScriptAction a = (ScriptAction) MaxDocumentType.getGlobalScript(i);
	    add(a, a.getName(), a.getModifiers(), a.getMnemonic());
	}
    }

    public ScriptMenu(String type)
    {
	super("Script");
	setHorizontalTextPosition(AbstractButton.LEFT);

	int size = MaxDocumentType.countGlobalScripts();
	for (int i = 0; i < size; i++) {
	    ScriptAction a = MaxDocumentType.getGlobalScript(i);
	    add(a, a.getName(), a.getModifiers(), a.getMnemonic());
	}
	MaxDocumentType doctype = Mda.getDocumentTypeByName(type);
	if (doctype != null) {
	    size = doctype.countScripts();
	    if (size > 0) {
		addSeparator();
	    }
	    for (int i = 0; i < size; i++) {
		ScriptAction a = doctype.getScript(i);
		add(a, a.getName(), a.getModifiers(), a.getMnemonic());
	    }
	}
    }
}




