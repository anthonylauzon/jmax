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
// Authors: Peter Hanappe.
// 

package ircam.jmax.script.pkg;
import java.util.*;
import java.io.*;
import ircam.jmax.mda.*;

public class MaxPackageDocument extends MaxDocument {
    protected MaxPackageData rootData;

    public MaxPackageDocument(String filename) throws Exception {
	this(new File(filename));
    }

    public MaxPackageDocument(File file) throws Exception {
	super(null, MaxPackageDocumentType.instance());
	this.file = file;
	FileReader in = null;
	in = new FileReader(file);
	try {
	    rootData = SettingsParser.parse(in);
	    rootData.setDocument(this);
	} catch (Exception e) {
	    throw e;
	} finally {
	    if (in != null) {
		in.close();
	    }
	}	
    }

    public MaxPackageDocument() {
	this(new MaxPackageData());
    }

    public MaxPackageDocument(MaxPackageData data) {
	super(null, MaxPackageDocumentType.instance());
	this.file = null;
	this.rootData = data;
	rootData.setDocument(this);
    }

    public void write(PrintWriter out) {
	int len = rootData.size();
	for (int i = 0; i < len; i++) {
	    out.println(rootData.elementAt(i));
	    out.flush();
	}
    }

    public void hash(SettingsTable table) {
	hash(rootData, table);
    }

    public static void hash(MaxPackageData data, SettingsTable table) {
	int len = data.size();
	for (int i = 0; i < len; i++) {
	    Object line = data.elementAt(i);
	    if (line instanceof Assignment) {
		Assignment a = (Assignment) line;
		if (a.type == Assignment.SET) {    
		    table.setSetting(a.name, a.value);
		} else {
		    table.appendSetting(a.name, a.value);
		}
	    }
	}
    }
}
