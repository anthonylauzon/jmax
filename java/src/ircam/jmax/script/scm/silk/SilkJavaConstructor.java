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

/** SilkJavaConstructor
 *
 * An addation of Peter Norvig's JavaMethod class for constructors.
 */
package ircam.jmax.script.scm.silk;
import java.lang.reflect.*;
import silk.*;

public class SilkJavaConstructor extends Procedure {
    Class[] argClasses;
    Constructor constructor;
    String className;

    public static class Factory extends Procedure {
	public Object apply(Scheme interpreter, Object args) {
	    return new SilkJavaConstructor(new String((char[]) first(args)), rest(args));
	}
    }

    public static Procedure getFactory() {
	return new Factory();
    }

    public SilkJavaConstructor(Object targetClassName, Object argClassNames) {
	//this.name = targetClassName + "." + methodName;
	try {
	    className = toClass(targetClassName).getName();
	    argClasses = classArray(argClassNames);
	    constructor = toClass(targetClassName).getConstructor(argClasses);
	} catch (ClassNotFoundException e) { 
	    error("Bad class, can't find class " + targetClassName); 
	} catch (NoSuchMethodException e) { 
	    error("Can't get constructor for " + targetClassName); 
	}    
    }

    /** Apply the constructor to a list of arguments. **/
    public Object apply(Scheme interpreter, Object args) {
	try {
	    return constructor.newInstance(toArray(args));
	} catch (Exception e) { 
	    e.printStackTrace(); 
	}
	return error("Bad Java Constructor application:"); 
    }

    public static Class toClass(Object arg) throws ClassNotFoundException { 
	if (arg instanceof Class) {
	    return (Class) arg;
	} else if (arg instanceof char[]) {
	    arg = new String((char[]) arg);
	} else if (!(arg instanceof String)) {
	    ;
	} 

	if (arg.equals("void"))    return java.lang.Void.TYPE;
	else if (arg.equals("boolean")) return java.lang.Boolean.TYPE;
	else if (arg.equals("char"))    return java.lang.Character.TYPE;
	else if (arg.equals("byte"))    return java.lang.Byte.TYPE;
	else if (arg.equals("short"))   return java.lang.Short.TYPE;
	else if (arg.equals("int"))     return java.lang.Integer.TYPE;
	else if (arg.equals("long"))    return java.lang.Long.TYPE;
	else if (arg.equals("float"))   return java.lang.Float.TYPE;
	else if (arg.equals("double"))  return java.lang.Double.TYPE;
	else return Class.forName((String)arg);
    }

    /** Convert a list of Objects into an array.  Peek at the argClasses
     * array to see what's expected.  That enables us to convert between
     * Double and Integer, something Java won't do automatically. **/
    public Object[] toArray(Object args) {
	int n = length(args);
	int diff = n - argClasses.length;
	if (diff != 0) {
	    error(Math.abs(diff) + " too " + ((diff>0) ? "many" : "few") + " args to " + this);
	}
	Object[] array = new Object[n];
	for(int i = 0; i < n && i < argClasses.length; i++) {
	    Object a = first(args);
	    if (argClasses[i] == java.lang.Integer.TYPE) {
		array[i] = new Integer((int)num(a));
	    } else if (argClasses[i].getName().equals("java.lang.String")) {
		if (a instanceof char[]) {
		    /* Convert from a Scheme string to a Java string. */
		    array[i] = new String((char[]) a);
		} else if (a instanceof String) {
		    array[i] = a;
		} else {
		    /* Error. Let the invocation throw an exception. */
		    array[i] = a;
		}
	    }else
		array[i] = first(args);
	    args = rest(args);
	}
	return array;
    }

    /** Convert a list of class names into an array of Classes. **/
    public Class[] classArray(Object args) throws ClassNotFoundException {
	int n = length(args);
	Class[] array = new Class[n];
	for(int i = 0; i < n; i++) {
	    array[i] = toClass(first(args));
	    args = rest(args);
	}
	return array;
    }
}

