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

package ircam.jmax.script.scm.silk;
import java.io.*;
import java.util.*;
import silk.*;
import ircam.jmax.script.pkg.*;

public class SilkSettingsEnvironment extends Environment implements SettingsTable {

    class SEEnumeration implements Enumeration {
	Object list;
	boolean shouldExpand = true;

	SEEnumeration(Object list, boolean expand) 
	{
	    /* Since I start "nextElement" with "rest" I inserted a
	     * dummy pair in front of the list */
	    this.list = cons(null, list);
	    shouldExpand = expand;
	}

	public boolean hasMoreElements() 
	{
	    return (rest(list) != null); 
	}

	public Object nextElement() 
	{
	    /* The straightforward way to run thru the list is :
	     * 
	     *   Object nextElement()  {
	     *     Object value = first(list);
	     *     ...
	     *     list = rest(list);
	     *   }
	     *
	     * However, the following code will not work:
	     *
	     * Pair list = new Pair("1", null);
	     * Enumeration elements = new SEEnumeration(list);
	     * Object first = elements.nextElement();
	     * // We are now on the last element of the list, i.e. null
	     * append(list, "2");
	     * if (elements.hasMoreElements()) {
	     *    // hasMoreElements returns zero because the enumeration
	     *    // is positioned on null. It's not aware of the fact that
	     *    // we've appended another element.
	     * }
	     *
	     */
	    list = rest(list);
	    Object value = (list instanceof Pair)? first(list) : list;
	    if (value instanceof char[]) {
		value = new String((char[]) value);
	    }
	    return (shouldExpand) ? expand((String) value) : value;
	}

	public String toString() {
	    return list.toString();
	}
    }

    public String expand(String s) 
    {
	String t = SettingsParser.expand(s, this);
	return t;
    }

    public String getSetting(String name) 
    {
	try {
	    Object value = lookup(name.intern());
	    if (value instanceof Pair) {
		value = first(value);
	    } 
	    if (value instanceof char[]) {
		value = new String((char[]) value);
	    }
	    return expand((String) value);
	} catch (Exception e) {
	    return "";
	}
    }

    public String getSettingExpression(String name) 
    {
	try {
	    Object value = lookup(name.intern());
	    if (value instanceof Pair) {
		value = first(value);
	    } 
	    if (value instanceof char[]) {
		value = new String((char[]) value);
	    }
	    return (String) value;
	} catch (Exception e) {
	    return "";
	}
    }

    protected char[] toSilkString(String s) 
    {
	char[] r = new char[s.length()];
	s.getChars(0, s.length(), (char[]) r, 0);
	return r;
    }

    public void setSetting(String name, Object value) 
    {
	if (value instanceof String) {
	    value = toSilkString((String) value);
	}
	try {
	    Object oldvalue = lookup(name.intern());
	    if (oldvalue instanceof Pair) {
		throw new RuntimeException("Can't change a list setting to an atomic setting: " + name);
	    }
	    set(name.intern(), value);
	} catch (Exception e) {
	    define(name.intern(), value);
	}	
    }

    protected void append(Pair list, Object value) 
    {
	if (list.rest == null) {
	    list.rest = new Pair(value, null);
	} else {
	    append((Pair) list.rest, value);
	}
    }

    public void appendSetting(String name, Object value) 
    {
	if (value instanceof String) {
	    value = toSilkString((String) value);
	}
	try {
	    Object oldvalue = lookup(name.intern());
	    if (oldvalue == null) {
	      set(name.intern(), cons(value, null));
	    } else if (!(oldvalue instanceof Pair)) {
		throw new RuntimeException("Can't append a setting to the atomic setting: " + name);
	    } else {
		append((Pair) oldvalue, value);
	    }
	} catch (Exception e) {
	    define(name.intern(), cons(value, null));
	}
    }

    public Enumeration enumerateSetting(String name) 
    {
	try {
	    return new SEEnumeration(lookup(name.intern()), true);
	} catch (Exception e) {
	    return null;
	}
    }

    public Enumeration enumerateSettingExpressions(String name) 
    {
	try {
	    return new SEEnumeration(lookup(name.intern()), false);
	} catch (Exception e) {
	    return null;
	}
    }

    public Enumeration settings() 
    {
	return new SEEnumeration(vars, false);
    }

    public void loadSettings(String file) throws Exception 
    {
	new MaxPackageDocument(file).hash(this);
    }

    public void loadSettings(File file) throws Exception 
    {
	new MaxPackageDocument(file).hash(this);
    }
}
