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

public class SettingsParser {

    public static String expand(String expr, SettingsTable table) {
	if (expr.indexOf('$') < 0) {
	    return expr;
	}
	return expand(new StringReader(expr), table);
    }

    public static String expand(Reader in, SettingsTable table) {
	StringBuffer v = new StringBuffer();
	StringBuffer var = new StringBuffer();
	try {
	    while (true) {
		int c = in.read();
		if (c < 0) {
		    return v.toString();
		}
		if (c == '$') {
		    c = in.read();
		    if (c == '(') {
			var.setLength(0);
			while (true) {
			    c = in.read();
			    if (c < 0) {
				v.append(var.toString());
				v.append("<Error: Missing closing parenthesis>");
				return v.toString();
			    } else if (c == ')') {
				String s = expand(var.toString(), table);
				String t = table.getSetting(s);
				v.append(t);
				break;
			    } else {
				var.append((char) c);
			    }
			}
		    } else {
			v.append('$');
			v.append((char) c);
		    }
		} else {
		    v.append((char) c);
		}
	    }
	} catch (Exception e) {
	    String m = (e.getMessage() == null) ? e.getClass().getName() : e.getMessage();
	    v.append("<").append(m).append(">");
	    return v.toString();
	}
    }

    public static MaxPackageData parse(Reader in) throws Exception {
	String line;
	MaxPackageData lines = new MaxPackageData();
	LineNumberReader lin = new LineNumberReader(in);
	while (true) {
	    line = lin.readLine();
	    if (line == null) {
		return lines;
	    }
	    try {
		lines.addElement(parseLine(line));
	    } catch (Exception e) {
		throw new Exception(Integer.toString(lin.getLineNumber()));
	    }
	}
    }

    public static Object parseLine(String line) throws Exception {
	if (line.trim().length() == 0) {
	    return new EmptyLine(line);
	}
	if (line.charAt(0) == '#') {
	    return new CommentLine(line);
	}
	StringTokenizer st = new StringTokenizer(line, "=:");
	if (st.countTokens() != 2) {
	    throw new Exception(); 
	}
	String left = st.nextToken().trim();
	String right = st.nextToken().trim();
	if (line.indexOf('=') > 0) {
	    return new Assignment(left, Assignment.SET, right);
	} else {
	    return new Assignment(left, Assignment.APPEND, right);
	}
    }
}











