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

public class MaxPackageDocumentType extends MaxDocumentType {
    static MaxPackageDocumentType type = null;

    public static MaxPackageDocumentType instance() {
	if (type == null) {
	    type = new MaxPackageDocumentType();
	    Mda.installDocumentType(type);
	}
	return type;
    }

    public MaxPackageDocumentType() {
	super("package");
	setPrettyName("package definition file");
    }

    /** Create a new empty document of the type, in the given context */
    public MaxDocument newDocument(MaxContext context) {
	return new MaxPackageDocument();
    }

    public boolean canMakeNewDocument() {
	return true;
    }

    public boolean canMakeNewDocument(MaxData data) {
	return (data instanceof MaxPackageData);
    }    
}
