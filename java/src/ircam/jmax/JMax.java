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
import ircam.jmax.script.pkg.*;
import ircam.jmax.fts.*;
import java.util.*;
import java.io.*;

public class JMax 
{
    /* The jMax package */
    protected static MaxPackage jmax;

    protected static void getPackage() {
	if (jmax == null) {
	    if (MaxApplication.getPackageLoader() == null) { 
		throw new RuntimeException("No package handler defined in Max application");
	    }
	    jmax = MaxApplication.getPackageLoader().getPackage("jmax");
	}
    }

    public static String getSetting(String key) {
	if (jmax == null) {
	    getPackage();
	}
	return jmax.getSettings().getSetting(key);
    }

    public static String setSetting(String key, String value) {
	if (jmax == null) {
	    getPackage();
	}
	String s = getSetting(key);
	jmax.getSettings().setSetting(key, value);
	return s;
    }

    public static Enumeration enumerateSetting(String key) {
	if (jmax == null) {
	    getPackage();
	}
	return jmax.getSettings().enumerateSetting(key);
    }
    
    static {
	setSetting("jmax-version", MaxVersion.getMaxVersion());	
	
	/* Initialize all the submodules */
	ircam.jmax.mda.MdaModule.initModule();
	ircam.jmax.fts.FtsModule.initModule();
	ircam.jmax.dialogs.DialogsModule.initModule();
	ircam.jmax.editors.console.ConsoleModule.initModule();
	ircam.jmax.editors.patcher.ErmesModule.initModule(true);
	ircam.jmax.editors.control.ControlModule.initModule();
    }
}





