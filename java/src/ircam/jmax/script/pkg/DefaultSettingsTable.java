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

public class DefaultSettingsTable implements SettingsTable
{
    final static int NOTHING = 0;
    final static int EXPAND = 1;

    Pair names = null;
    Pair values = null;

    class Pair 
    {
	Object car;
	Object cdr;

	Pair(Object car) 
	{
	    this(car, null);
	}

	Pair(Object car, Object cdr) 
	{
	    this.car = car;
	    this.cdr = cdr;
	}

	void append(Object value) 
	{
	    if (cdr == null) {
		cdr = new Pair(value);
	    } else {
		((Pair)cdr).append(value);
	    }
	}
    }
 
    class SHEnumeration implements Enumeration 
    {
	Pair list;
	int op;

	SHEnumeration(Pair list, int op) 
	{
	    /* the list is put into a pair because nextElement starts
	     * with a rest(list). */
	    this.list = new Pair(null, list);
	    this.op = op;
	}

	public boolean hasMoreElements() 
	{
	    return (rest(list) != null); 
	}

	public Object nextElement() 
	{
	    list = (Pair) rest(list);
	    Object value = first(list);
	    return convert(value);
	}

	Object convert(Object obj) 
	{
	    if (obj == null) {
		return null;
	    }
	    switch (op) {
	    case EXPAND: return expand((String)obj);
	    case NOTHING: return obj;
	    default: return obj;
	    }
	}
    }

    public Object get(String name) 
    {
	String s = name.intern();
	Pair n = names;
	Pair v = values;
	while (n != null) {
	    if (n.car == s) {
		return v.car;
	    }
	    n = (Pair) rest(n);
	    v = (Pair) rest(v);
	}
	return null;
    }

    protected void put(String name, Object value) 
    {
	names = new Pair(name.intern(), names);
	value = new Pair(value, values);
    }

    protected Object rest(Object obj) 
    {
	return (obj instanceof Pair) ? ((Pair) obj).cdr : null;
    }

    protected Object first(Object obj) 
    {
	return (obj instanceof Pair) ? ((Pair) obj).car : obj;
    }

    public DefaultSettingsTable() {
    }

    public String expand(String s) 
    {
	return SettingsParser.expand(s, this);
    }

    public String getSetting(String name) 
    {
	Object obj = first(get(name));
	return (obj == null) ? "" : expand((String) obj);
    }

    public String getSettingExpression(String name) 
    {
	Object obj = (Pair) first(get(name));
	return (obj == null) ? "" : (String) obj;
    }

    public void setSetting(String name, Object value) 
    {
	String s = name.intern();
	Pair n = names;
	Pair v = values;
	Pair p = null;
	Object obj;
	while (n != null) {
	    if (first(n) == s) {
		 p = v;
		 obj = first(v);
		 break;
	    }
	    n = (Pair) rest(n);
	    v = (Pair) rest(v);
	}
	if (p == null) {
	    put(name, value);
	} else if (p instanceof Pair) {
	    throw new RuntimeException("Can't change a list setting to an atomic setting: " + name);
	} else {
	    p.car = value;
	}
    }

    public void appendSetting(String name, Object value) {
	Object obj = get(name);
	if (obj == null) {
	    put(name, new Pair(value));
	} else if (!(obj instanceof Pair)) {
	    throw new RuntimeException("Can't append a setting to the atomic setting: " + name);
	} else {
	    ((Pair) obj).append(value);
	}
    }

    public Enumeration enumerateSetting(String name) 
    {
	return new SHEnumeration((Pair) get(name), EXPAND);
    }

    public Enumeration enumerateSettingExpressions(String name) 
    {
	return new SHEnumeration((Pair) get(name), NOTHING);
    }

    public Enumeration settings() 
    {
	return new SHEnumeration(names, NOTHING);
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

