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

package ircam.jmax.script.scm.kawa;

import java.lang.reflect.*;
import gnu.mapping.*;
import kawa.lang.*;

/**
 * KawaJavaMethod
 *
 * An addation of Peter Norvig's JavaMethod class for Kawa.
 */
public class KawaJavaMethod extends ProcedureN {
    Class[] argClasses;
    Class thisClass;
    Method method;
    boolean isStatic;
    boolean returnsString;
    boolean returnsBoolean;
    String methodName;
    String className;

    public static class MethodProcedure extends ProcedureN {
	public Object applyN(Object[] args) {
	    String methodName = "<unknown>";
	    try {
		methodName = args[0].toString();
		Object y = args[1];
		return new KawaJavaMethod(methodName, y.toString(), rest(rest(args)));
	    } catch (Exception e) { 
		System.out.println("Can't get method " + methodName + ": " + e.getMessage()); 
	    }
	    return Boolean.FALSE; 
	}
    }

    public static Procedure getProcedure() {
	return new MethodProcedure();
    }

    public KawaJavaMethod(String methodName, Object targetClassName, Object[] argClassNames) throws Exception {
	argClasses = classArray(argClassNames);
	thisClass = toClass(targetClassName); 
	className = (String) targetClassName;
	method = thisClass.getMethod(methodName, argClasses);
	if (method == null) {
	    throw new Exception("Method is null");
	}
	this.methodName = methodName;
	isStatic = Modifier.isStatic(method.getModifiers());
	returnsString = (method.getReturnType() == "".getClass());
	returnsBoolean = (method.getReturnType() == Boolean.TYPE);
    }

    protected static Object first(Object[] a) {
	return a[0];
    }

    protected static Object[] rest(Object[] a) {
	Object[] b;
	if (a != null) {
	    int len = a.length - 1;
	    b = new Object[len];
	    for (int i = 0; i < len; i++) {
		b[i] = a[i + 1];
	    }
	} else {
	    b = new Object[0];
	}
	return b;
    }

    /** Apply the method to a list of arguments. **/
    public Object applyN (Object[] args) {
	try {
	    Object r;
	    Object obj;
	    Object[] arg;

	    if (method == null) {
		System.out.println("Method " + methodName + " is null");
	    }
	    if (isStatic) {
		obj = null;
		arg = KawaInterpreter.convertToJava(args, argClasses);
	    } else {
		obj = KawaInterpreter.convertToJava(first(args), thisClass);
		arg = KawaInterpreter.convertToJava(rest(args), argClasses);
	    }
	    r = method.invoke(obj, arg);
	    return KawaInterpreter.convertToKawa(r);
	} catch (Exception e) { 
	    String m = (e.getMessage() == null) ? e.getClass().getName() : e.getMessage();
	    System.out.println("Can't invoke Java method " + className + "." + methodName + ": " + m); 
	    return Boolean.FALSE;
	}
    }

    public static Class toClass(Object arg) throws ClassNotFoundException { 
	if (arg instanceof Class) {
	    return (Class) arg;
	} else if (arg instanceof FString) {
	    arg = arg.toString();
	} 
	if (arg.equals("void")) {
	    return java.lang.Void.TYPE;
	} else if (arg.equals("boolean")) {
	    return Boolean.TYPE;
	} else if (arg.equals("char")) {
	    return Character.TYPE;
	} else if (arg.equals("byte")) {
	    return Byte.TYPE;
	} else if (arg.equals("short")) {
	    return Short.TYPE;
	} else if (arg.equals("int")) {
	    return Integer.TYPE;
	} else if (arg.equals("long")) {
	    return Long.TYPE;
	} else if (arg.equals("float")) {
	    return Float.TYPE;
	} else if (arg.equals("double")) {
	    return Double.TYPE;
	} else {
	    return Class.forName((String)arg);
	}
    }

    /** Convert a list of class names into an array of Classes. **/
    public Class[] classArray(Object[] args) throws ClassNotFoundException {
	int n = args.length;
	Class[] array = new Class[n];
	for(int i = 0; i < n; i++) {
	    array[i] = toClass(args[i]);
	}
	return array;
    }
}
