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
// Authors: Peter Hanappe
// 

package ircam.jmax.script.scm.silk;

import java.lang.reflect.*;
import java.util.*;
import silk.*;

/**
 * SilkJavaInvoke
 *
 * Invoke a method on a Java object with given arguments.
 */
public class SilkJavaInvoke extends Procedure {
    final static char VIRTUAL = 'V';
    final static char STATIC = 'S';
    final static char CONSTRUCTOR = 'C';

    char type;

    public SilkJavaInvoke(char type) {
	this.type = type;
    }

    /** Apply the method to a list of arguments. **/
    public Object apply(Scheme interpreter, Object list) {
	String methodName = "<unknown method>";
	Object obj;
	Object[] arg;
	Method m;
	Class claz;
	Object tmp;
	try {	
	    if (type == VIRTUAL) {
		obj = first(list);
		claz = obj.getClass();
	    } else if (type == STATIC) {
		obj = null;
		tmp = first(list);
		if (tmp instanceof char[]) {
		    claz = Class.forName(new String((char[]) tmp));
		} else if (tmp instanceof Class) {
		    claz = (Class) tmp;
		} else {
		    claz = Class.forName(tmp.toString());
		}
	    } else {
		throw new Exception("Unknown invocation type " + type);
	    }
	    tmp = second(list);
	    methodName = (tmp instanceof char[]) ? new String((char[]) tmp) : tmp.toString();
	    arg = listToVector(rest(rest(list)));
	    m = dispatch(methodName, claz, arg);
	    if (m == null) {
		throw new RuntimeException("Can't find the method");
	    }
	    Object r = m.invoke(obj, arg);
	    return r;
	} catch (Exception e) {
	    String mess = (e.getMessage() != null) ? e.getMessage() : e.getClass().getName();
	    throw new RuntimeException("Can't invoke method " + methodName + ": " + mess);
	}
    }

    protected Method dispatch(String methodName, Class claz, Object[] arg) {
	Method[] methodArray = claz.getMethods();
	Vector methodVector = new Vector();
	int len, i;
	len = methodArray.length;
	for (i = 0; i < len; i++) {
	    if (methodName.equals(methodArray[i].getName())) {
		methodVector.addElement(methodArray[i]);
	    }
	}
	if (methodVector.size() == 0) {
	    return null;
	} else if (methodVector.size() == 1) {
	    return (Method) methodVector.firstElement();
	} else {
	    len = methodVector.size();
	    for (i = 0; i < len; i++) {
		Method m = (Method) methodVector.elementAt(i);
		Class[] cl = m.getParameterTypes();
		if (cl.length != arg.length) {
		    methodVector.removeElement(m);
		} else {
		    for (int k = 0; k < arg.length; k++) {
			if (!canCoerce(cl[k], arg[k])) {
			    methodVector.removeElement(m);
			    break;
			}
		    }
		}
	    }
	    if (methodVector.size() == 0) {
		return null;
	    } else {
		return (Method) methodVector.firstElement();
	    } 
	}
    }

    protected boolean canCoerce(Class c, Object o) {
	return false 
	    || ((o instanceof Boolean) && (c == Boolean.TYPE))
	    || ((o instanceof Character) && (c == Character.TYPE))
	    || ((o instanceof Short) && (c == Short.TYPE))
	    || ((o instanceof Integer) && (c == Integer.TYPE))
	    || ((o instanceof Long) && (c == Long.TYPE))
	    || ((o instanceof Float) && (c == Float.TYPE))
	    || ((o instanceof Double) && (c == Double.TYPE))
	    || (c.isInstance(o));
    }
}
